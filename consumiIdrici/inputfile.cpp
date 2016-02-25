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
    std::map<QString, ConsumptionSet> usersData;

    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly)) {

        QProgressDialog progress(parent);
        progress.setLabelText("Lettura file: " + QFileInfo(fileName).fileName());
        progress.setWindowTitle("Consumi idrici");
        progress.setRange(0, inputFile.size());
        progress.setModal(true);
        progress.setCancelButton(0); //elimina il pulsante annulla
        progress.show();
        qApp->processEvents();

        QTextStream in(&inputFile);
        size_t c = 1;

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList params = line.split(',');

            //non serve essendoci il try-catch
            /*if (params.length() != 3) { //parametri non suddivisi correttamente o in numero errato
                QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nella lettura dei dati. Formato dati errati (riga " + QString::number(c) + ")." , QMessageBox::Abort | QMessageBox::Ignore);
                if (msg.exec() == QMessageBox::Abort) {
                    inputFile.close();
                    usersData.clear();
                    return usersData;
                }
            }*/

            //converte la stringa in formato datetime. sono presenti " all'inizio e alla fine
            //toUTC è necessario per gestire ora legale/solare (ad esempio 29-03-2015 02:00)
            //che viene poi convertita in CET dalla funzione UTCtoDayLightSavTime
            bool ok;
            try {
                Consumption cons(UTCtoDayLightSavTime(QDateTime::fromString(params[0], "\"yyyy-MM-dd HH:mm:ss\"").toUTC()), params[1].toDouble(&ok));

                if (!ok || params[2].length() == 0) throw "Cast failed";

                //aggiunge consumi all'utente (se non è presente l'utente nella mappa lo crea e posiziona in ordine)
                usersData[params[2]].insert(cons);
            } catch (...) {
                QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nella lettura dei dati. Dati errati alla riga " + QString::number(c), QMessageBox::Abort | QMessageBox::Ignore);
                if (msg.exec() == QMessageBox::Abort) {
                    inputFile.close();
                    usersData.clear();
                    return usersData;
                }
            }

            progress.setValue(inputFile.pos()); //essendo modale chiama già processEvents (vedi documentazione).
            //chiamare qui la processEvents allunga di ~10 secondi l'apertura del file grande


            /*if (progress.wasCanceled()) { button annulla tolto
                inputFile.close();
                usersData.clear();
                return usersData;
            }*/
            ++c;
        }
        inputFile.close();

    } else {
        QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nell'apertura del file." , QMessageBox::Ok);
        msg.exec();
    }
    return usersData;

}

QDateTime InputFile::UTCtoDayLightSavTime(QDateTime date, int UTC_offset /*fuso orario invernale default +1*/) const {
    if (date < dstStartDay || date > dstEndDay) //(date.secsTo(dstStartDay) > 0 || date.secsTo(dstEndDay) < 0)
        return date.addSecs(UTC_offset * 3600);
    else
        return date.addSecs( (UTC_offset + 1) * 3600);
}



