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

std::map<QString, consumptionSet> InputFile::read(QWidget *parent) const {
    //legge il file di input e restituisce un vector di record

    std::map<QString, consumptionSet> clients;

    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly)) {

        QProgressDialog progress(parent);
        progress.setLabelText("Lettura file: " + QFileInfo(fileName).fileName());
        progress.setRange(0, inputFile.size());
        progress.setModal(true);
        progress.setCancelButton(0); //elimina il pulsante annulla
        progress.show();
        qApp->processEvents();

        QTextStream in(&inputFile);
        size_t c = 1;

        qint64 temp = QDateTime::currentDateTime().toMSecsSinceEpoch();;

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList params = line.split(',');
            if (params.length() != 3) { //parametri non suddivisi correttamente o in numero errato
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
            bool ok;
            try {
                consumption cons(UTCtoDayLightSavTime(QDateTime::fromString(params[0], "\"yyyy-MM-dd HH:mm:ss\"").toUTC()), params[1].toDouble(&ok));

                if (!ok || params[2].length() == 0) throw "Cast failed";

                //aggiunge consumi al cliente (se non è presente il cliente nella mappa lo crea e posiziona in ordine)
                clients[params[2]].insert(cons);
            } catch (...) {
                QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nella lettura dei dati. Dati errati (riga " + QString::number(c) + ")." , QMessageBox::Abort | QMessageBox::Ignore);
                if (msg.exec() == QMessageBox::Abort) {
                    inputFile.close();
                    clients.clear();
                    return clients;
                }
            }

            progress.setValue(inputFile.pos()); //essendo modale chiama già processEvents (vedi documentazione).
            //chiamare la processEvents allunga di ~10 secondi l'apertura del file grande


            /*if (progress.wasCanceled()) { pulsante annulla tolto
                inputFile.close();
                clients.clear();
                return clients;
            }*/
            ++c;
        }
        inputFile.close();

        temp = QDateTime::currentDateTime().toMSecsSinceEpoch() - temp;
        QMessageBox msg(QMessageBox::Information, "Lettura", QString::number(temp) );
        msg.exec();

    } else {
        QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nell'apertura del file." , QMessageBox::Ok);
        msg.exec();
    }
    return clients;

}

QDateTime InputFile::UTCtoDayLightSavTime(QDateTime date, int UTC_offset /*fuso orario invernale default +1*/) const {
    if (date.secsTo(dstStartDay) > 0 || date.secsTo(dstEndDay) < 0)
        return date.addSecs(UTC_offset * 3600);
    else
        return date.addSecs( (UTC_offset + 1) * 3600);
}



