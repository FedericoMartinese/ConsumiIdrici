#include "datautility.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

//ONLY FOR DEBUG AND TEST
#define TESTFILENAME "consumption_some.csv"
#define FILENAME "consumption_all.csv"
//

std::vector<record> readFile(QString fileName) {
    //legge il file di input e restituisce un vector di record

    std::vector<record> inputRecords;

    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly)) {

        QTextStream in(&inputFile);
        int c = 1;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList params = line.split(',');
            if (params.length() != 3) { //parametri non suddivisi correttamente o in numero errato
                QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nella lettura dei dati. Formato dati errati (riga " + QString::number(c) + ")." , QMessageBox::Ok);
                msg.exec();
                break;
            }
            record rec;
            bool ok;


            rec.date = QDateTime::fromString(params[0], "\"yyyy-MM-dd HH:mm:ss\""); //converte la stringa in formato datetime. sono presenti " all'inizio e alla fine
            rec.value = params[1].toDouble(&ok);
            rec.clientID = params[2].toStdString();

            if (!rec.date.isValid() || !ok || rec.clientID.empty()) { //conversioni ai tipi non riuscite
                QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nella lettura dei dati. Formato dati errati (riga " + QString::number(c) + ")." , QMessageBox::Ok);
                msg.exec();
                break;
            }

            inputRecords.push_back(rec); //aggiunta record al vettore

            ++c;
        }
        inputFile.close();

    } else {
        QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nell'apertura del file." , QMessageBox::Ok);
        msg.exec();
    }
    return inputRecords;
}