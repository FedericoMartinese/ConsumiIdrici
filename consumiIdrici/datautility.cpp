#include "datautility.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

//ONLY FOR DEBUG AND TEST
#define TESTFILENAME "consumption_some.csv"
#define FILENAME "consumption_all.csv"
//


std::vector<clientConsumptions> readFile(QString fileName) {
    //legge il file di input e restituisce un vector di record

    INSERT SORT ALL^INTERNO DEL VETTORE INPUTRECORDS E INSERT SORT ALL^INTERNO DI OGNI VETTORE CONS
    std::vector<clientConsumptions> inputRecords;

    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly)) {

        QTextStream in(&inputFile);
        int c = 1;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList params = line.split(',');
            if (params.length() != 3) { //parametri non suddivisi correttamente o in numero errato
                QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nella lettura dei dati. Formato dati errati (riga " + QString::number(c) + ")." , QMessageBox::Abort | QMessageBox::Ignore);
                if (msg.exec() == QMessageBox::Abort) {
                    inputFile.close();
                    inputRecords.clear();
                    return inputRecords;
                }
            }          

            //converte la stringa in formato datetime. sono presenti " all'inizio e alla fine
            //toUTC è necessario per gestire ora legale/solare (ad esempio 29-03-2015 02:00)
            //che viene poi convertita in CET dalla funzione UTCtoDayLightSavTime
            QString clientID = params[2];
            bool ok;
            consumption cons(UTCtoDayLightSavTime(QDateTime::fromString(params[0], "\"yyyy-MM-dd HH:mm:ss\"").toUTC()), params[1].toDouble(&ok));

            if (!cons.isValid() || !ok) { //conversioni ai tipi non riuscite
                QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nella lettura dei dati. Dati errati (riga " + QString::number(c) + ")." , QMessageBox::Abort | QMessageBox::Ignore);
                if (msg.exec() == QMessageBox::Abort) {
                    inputFile.close();
                    inputRecords.clear();
                    return inputRecords;
                }
            }

            std::size_t clientPosition = findClient(inputRecords, clientID); //cerca il client nel vector. nel caso in cui non ci sia lo aggiunge
            if (clientPosition == inputRecords.size()) //se non trovato
                inputRecords.push_back(clientConsumptions(clientID)); //lo aggiunge

            inputRecords[clientPosition].addCons(cons); //aggiunta record al vettore

            ++c;
        }
        inputFile.close();

    } else {
        QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nell'apertura del file." , QMessageBox::Ok);
        msg.exec();
    }
    return inputRecords;
}

std::size_t findClient(std::vector<clientConsumptions> v, QString clientID) { // return v.size() if not found
    size_t i = 0;
    for (clientConsumptions cc : v) {
        if (cc.clientID() == clientID)
            break; //trovato
        ++i;
    }

    return i;
}

QDateTime UTCtoDayLightSavTime(QDateTime date, int UTC_offset /*fuso orario invernale default +1*/) {
    if (date.secsTo(dstStartDay) > 0 || date.secsTo(dstEndDay) < 0)
        return date.addSecs(UTC_offset * 3600);
    else
        return date.addSecs( (UTC_offset + 1) * 3600);
}

/* cercare di utilizzarla per dichiarare le const nel .h
QDate getLastSunday(int month, int year) {
    QDate sunday(year, month, QDate(year,month,1).daysInMonth());
    while (sunday.dayOfWeek() != 7) {
        sunday.addDays(-1);
    }
    return sunday;
}*/


