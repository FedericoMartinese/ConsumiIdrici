#include "datautility.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

//ONLY FOR DEBUG AND TEST
#define TESTFILENAME "consumption_some.csv"
#define FILENAME "consumption_all.csv"
//


std::map<QString, consumptionSet> readFile(QString fileName) {
    //legge il file di input e restituisce un vector di record

    std::map<QString, consumptionSet> clients;

    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly)) {

        QTextStream in(&inputFile);
        int c = 1;
        while (!in.atEnd()) {
            std::string line = in.readLine().toStdString();

            QString params[3];
            //QStringList params = line.split(',');

            try {
                int v1,v2;
                v1 = line.find(',');
                v2 = line.find(',', v1+1);
                params[0] = QString::fromStdString(line.substr(0,v1));
                params[1] = QString::fromStdString(line.substr(v1+1, v2-v1-1));
                params[2] = QString::fromStdString(line.substr(v2+1));
            } catch (...) { //parametri non suddivisi correttamente o in numero errato
                QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nella lettura dei dati. Formato dati errati (riga " + QString::number(c) + ")." , QMessageBox::Abort | QMessageBox::Ignore);
                if (msg.exec() == QMessageBox::Abort) {
                    inputFile.close();
                    clients.clear();
                    return clients;
                }
            }          

            //converte la stringa in formato datetime. sono presenti " all'inizio e alla fine
            //toUTC è necessario per gestire ora legale/solare (ad esempio 29-03-2015 02:00)
            //che viene poi convertita in CET dalla funzione UTCtoDayLightSavTime
            QString clientID = params[2];
            bool ok;
            try {
                consumption cons(UTCtoDayLightSavTime(QDateTime::fromString(params[0], "\"yyyy-MM-dd HH:mm:ss\"").toUTC()), params[1].toDouble(&ok));

                if (!ok) throw "Cast failed";

                //aggiunge consumi al cliente (se non è presente il cliente nella mappa lo crea e posiziona in ordine)
                clients[clientID].insert(cons);
            } catch (...) {
                QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nella lettura dei dati. Dati errati (riga " + QString::number(c) + ")." , QMessageBox::Abort | QMessageBox::Ignore);
                if (msg.exec() == QMessageBox::Abort) {
                    inputFile.close();
                    clients.clear();
                    return clients;
                }
            }

            ++c;
        }
        inputFile.close();

    } else {
        QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nell'apertura del file." , QMessageBox::Ok);
        msg.exec();
    }
    return clients;
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


