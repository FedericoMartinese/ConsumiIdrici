#include "inputfile.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QProgressDialog>
#include <QFileInfo>
#include <QApplication>
#include <dates.h>
InputFile::InputFile(QString fileName) : fileName(fileName)
{
}

std::map<QString, ConsumptionSet> InputFile::read(QWidget *parent) const {
    //legge il file e restituisce una struttura dati contenente tutti i consumi, suddivisi per utente, ordinati per data e controllando che tutti i valori siano validi e coerenti
    std::map<QString, ConsumptionSet> usersData;

    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly)) {
        //maschera che mostra la percentuale di avanzamento della lettura ed evita che il programma freezi
        QProgressDialog progress(parent, Qt::WindowTitleHint);
        progress.setLabelText("Lettura file: " + QFileInfo(fileName).fileName());
        progress.setWindowTitle("Consumi idrici");
        progress.setRange(0, inputFile.size());
        progress.setModal(true);
        progress.setCancelButton(0); //nasconde il pulsante annulla
        progress.show();
        qApp->processEvents();

        QTextStream in(&inputFile);
        std::size_t c = 1; //contatore righe per messaggi di errore

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList params = line.split(','); //i parametri sono suddivisi da una virgola

            bool ok;
            //conversione della riga letta nella struttura dati
            try {
                if (params.length() != 3) throw "Incorrect number of parameters";

                //converte la stringa in formato datetime. sono presenti " all'inizio e alla fine
                //toUTC() è necessario per gestire ora legale/solare (ad esempio 29-03-2015 02:00)
                //che viene poi convertita in CET dalla funzione UTCtoDayLightSavTime
                //se i dati sono invalidi il costruttore di consumption genera un'eccezione
                Consumption cons(UTCtoDayLightSavTime(QDateTime::fromString(params[0], "\"yyyy-MM-dd HH:mm:ss\"").toUTC()), params[1].toDouble(&ok));

                if (!ok || params[2].length() == 0) throw "Cast failed";

                //aggiunge consumi all'utente (se non è presente l'utente nella mappa lo crea e posiziona in ordine)
                usersData[params[2]].insert(cons);
            } catch (...) {
                //in caso di errori l'utente può abortire la lettura del file (nel caso viene svuotata la struttura dati)
                //o ignorare la riga con dati non validi (in questo caso non viene inserita la riga e si continua con la lettura)
                QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nella lettura dei dati. Dati errati alla riga " + QString::number(c), QMessageBox::Abort | QMessageBox::Ignore);
                if (msg.exec() == QMessageBox::Abort) {
                    inputFile.close();
                    usersData.clear();
                    return usersData;
                }
            }

            progress.setValue(inputFile.pos()); //essendo modale chiama già processEvents (vedi documentazione).
            //chiamare qui la processEvents allunga di ~10 secondi l'apertura del file grande


            /*if (progress.wasCanceled()) { //button annulla tolto
                inputFile.close();
                usersData.clear();
                return usersData;
            }*/
            ++c;
        }
        inputFile.close();

    } else { //apertura del file non riuscita
        QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nell'apertura del file." , QMessageBox::Ok);
        msg.exec();
    }
    return usersData;

}

QDateTime InputFile::UTCtoDayLightSavTime(QDateTime date, int UTC_offset /*fuso orario invernale default +1*/) const {
    //converte le date UTC in CET tenendo conto dell'ora legale/solare
    if (date < dstStartDay || date > dstEndDay)
        return date.addSecs(UTC_offset * 3600); //aggiunge UTC_offset ore (default 1)
    else
        return date.addSecs( (UTC_offset + 1) * 3600); //aggiunge UTC_offset+1 ore (default 2)
}



