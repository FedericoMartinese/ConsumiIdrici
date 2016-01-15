#include "datautility.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

//ONLY FOR DEBUG AND TEST
#define TESTFILENAME "consumption_some.csv"
#define FILENAME "consumption_all.csv"
//

#define MINSECSPRECISION 60 //1 minuto di precisione minima


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
                QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nella lettura dei dati. Formato dati errati (riga " + QString::number(c) + ")." , QMessageBox::Abort | QMessageBox::Ignore);
                if (msg.exec() == QMessageBox::Abort) {
                    inputFile.close();
                    inputRecords.clear();
                    return inputRecords;
                }
            }
            record rec;
            bool ok;


            //converte la stringa in formato datetime. sono presenti " all'inizio e alla fine
            //toUTC è necessario per gestire ora legale/solare (ad esempio 29-03-2015 02:00)
            //che viene poi convertita in CET dalla funzione UTCtoDayLightSavTime
            rec.date = UTCtoDayLightSavTime(QDateTime::fromString(params[0], "\"yyyy-MM-dd HH:mm:ss\"").toUTC());
            rec.value = params[1].toDouble(&ok);
            rec.clientID = params[2];


            if (!rec.date.isValid() || !ok || rec.clientID.isEmpty()) { //conversioni ai tipi non riuscite
                QMessageBox msg(QMessageBox::Critical, "Consumi idrici", "Errore nella lettura dei dati. Dati errati (riga " + QString::number(c) + ")." , QMessageBox::Abort | QMessageBox::Ignore);
                if (msg.exec() == QMessageBox::Abort) {
                    inputFile.close();
                    inputRecords.clear();
                    return inputRecords;
                }
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

record getLastRecord(QString clientID, const std::vector<record> *data) { //return -1 consumption if client not found
    record last;
    last.date = QDateTime(QDate(2015,1,1),QTime(0,0)); //solo se i dati tutti del 2015
    last.clientID = clientID;
    last.value = -1; //client not found special value
    if (data == NULL) return last;
    for (record rec : *data) {
        if (rec.clientID == clientID && rec.date >= last.date)
            last = rec;
    }
    return last;
}

bool getPeriodConsumption(QString clientID, const std::vector<record> *data, QDateTime firstDate, QDateTime lastDate, double &periodConsumption) {
    if (data == NULL || !firstDate.isValid() || !lastDate.isValid() || firstDate.secsTo(lastDate) < 0) return false;

    /*
    bool found = false;
    record fPrev, fNext, lPrev, lNext;
    fPrev.value = -1;
    fNext.value = -1;
    lPrev.value = -1;
    lNext.value = -1;


    for (record rec : *data) {
        if (rec.clientID == clientID) {
            found = true;

            //ricerca della registrazione precedente alla prima data più vicina a essa (eventualmente coincidente)
            if (rec.date.secsTo(firstDate) >= 0 && (fPrev.value == -1 || rec.date.secsTo(fPrev.date) <= 0)) //fPrev <= rec <= firstDate o rec <= firstDate e fPrev non ancora trovato
                fPrev = rec;

            //ricerca della registrazione successiva alla seconda data più vicina ad essa (eventualmente coincidente)
            if (rec.date.secsTo(firstDate) <= 0 && (fNext.value == -1 || rec.date.secsTo(fNext.date) >= 0))  //firstDate >= rec >= fNext
                fNext = rec;

            //seconda data
            if (rec.date.secsTo(lastDate) >= 0 && (lPrev.value == -1 || rec.date.secsTo(lPrev.date) <= 0))
                lPrev = rec;

            if (rec.date.secsTo(lastDate) <= 0 && (lNext.value == -1 || rec.date.secsTo(lNext.date) >= 0))
                lNext = rec;

        }
    }

    // consumo a una data = (differenza di tempo tra le registrazioni più vicine) * (differenza di consumi tra le registrazioni più vicine) /
    // (differenza di tempo tra la data e la registrazione precedente più vicina) + (consumo alla registrazione precedente più vicina)

    double consAtFDate = (firstDate.toMSecsSinceEpoch() - fPrev.date.toMSecsSinceEpoch()) * (fNext.value - fPrev.value) / (fNext.date.toMSecsSinceEpoch() - fPrev.date.toMSecsSinceEpoch()) + fPrev.value;
    double consAtLDate = (lastDate.toMSecsSinceEpoch() - lPrev.date.toMSecsSinceEpoch()) * (lNext.value - lNext.value) / (lNext.date.toMSecsSinceEpoch() - lNext.date.toMSecsSinceEpoch()) + lPrev.value;

    periodConsumption = consAtLDate - consAtFDate;*/

    double consAtFirstDate = getConsAtDateSorted(clientID, firstDate, data); //getConsAtDate(clientID, firstDate, data);
    double consAtLastDate = getConsAtDateSorted(clientID, lastDate, data);   //getConsAtDate(clientID, lastDate, data);
    periodConsumption = consAtLastDate - consAtFirstDate;

    return consAtFirstDate >= 0 && consAtLastDate >= 0;
}

double getConsAtDateSorted(QString clientID, QDateTime date, const std::vector<record> *sortedData) {
    if (sortedData == NULL) return -1;

    record prev, next;
    bool found = false, initialized = false;

    for (record rec : *sortedData) {

        if (rec.clientID > clientID)
            break; //cliente non trovato
        else if (rec.clientID == clientID) {
            found = true;
            if (!initialized) {
                prev = rec;
                next = rec;
                initialized = true;
            }

            if (/*rec.date == date || */ qAbs(rec.date.secsTo(date))<=MINSECSPRECISION) { //se viene trovata una registrazione molto vicina è superfluo continuare la ricerca
                return rec.value;
            }

            if (rec.date < date /* && rec.date > prev.date*/) {//inutile il secondo controllo perché file ordinato, rec sicuramente >= prev
                prev = rec;
                next = rec; //si spostano gli iteratori avanti
            } else {
                next = rec; //dato che è ordinato è sicuramente il primo dopo la data cercata
                break;
            }
        }
    }

    if (!found) return -1;
    if (prev == next) return prev.value; //non ci sono registrazioni prima (dopo) la data cercata. si suppone che fino a (da) quel momento non ci siano stati consumi

    // se vengono trovate una registrazione precedene e una successiva alla data il consumo previsto al momento richiesto è
    // consumo a una data = (differenza di tempo tra le registrazioni più vicine) * (differenza di consumi tra le registrazioni più vicine) /
    // (differenza di tempo tra la data e la registrazione precedente più vicina) + (consumo alla registrazione precedente più vicina)
    // se una delle due non viene trovata, la registrazione precedente e quella successiva coincidono, si presume che non ci siano stati consumi successivi o precedenti e restituisce il valore delle registrazioni
    // si suppone consumo costante tra le due data
    return (date.toMSecsSinceEpoch() - prev.date.toMSecsSinceEpoch()) * (next.value - prev.value) / (next.date.toMSecsSinceEpoch() - prev.date.toMSecsSinceEpoch()) + prev.value;
}


//se la sorted funziona questa va eliminata
double getConsAtDate(QString clientID, QDateTime date, const std::vector<record> *data) {
    if (data == NULL) return -1;

    record prev, next;
    bool found = false, inizialized = false;

    for (record rec : *data) {
        if (rec.clientID == clientID) {
            found = true;
            if (!inizialized) {
                prev = rec;
                next = rec;
                inizialized = true;
            }

            //GESTIRE CONVERSIONE UTC GMT CET...


            if (qAbs(rec.date.secsTo(date))<=MINSECSPRECISION) //se viene trovata una registrazione molto vicina è superfluo continuare la ricerca
                return rec.value;



            /*
            PREV REC DATE   ->  REC
            PREV DATE REC   ->  PREV
            REC DATE PREV   ->  REC
            REC PREV DATE   ->  PREV
            DATE REC PREV   ->  REC
            DATE PREV REC   ->  PREV

            REC < PREV e DATE < PREV o PREV < REC e REC < DATE
            */

            //ricerca della data precedente più vicina alla data richiesta. se non esistono date precedenti si sceglie la prima delle successive
            if ((rec.date.secsTo(prev.date) >= 0 && date.secsTo(prev.date) >= 0) || (prev.date.secsTo(rec.date) >= 0 && rec.date.secsTo(date) >= 0))
                prev = rec;


            /*
            NEXT REC DATE   ->  REC
            NEXT DATE REC   ->  REC
            REC DATE NEXT   ->  NEXT
            REC NEXT DATE   ->  NEXT
            DATE REC NEXT   ->  REC
            DATE NEXT REC   ->  NEXT

            REC > NEXT e DATE > NEXT o NEXT > REC e REC > DATE
            */

            //ricerca della data successiva più vicina alla data richiesta. se non esistono date successive si sceglie l'ultima delle precedenti
            if ((rec.date.secsTo(next.date) <= 0 && date.secsTo(next.date) <= 0) || (next.date.secsTo(rec.date) <= 0 && rec.date.secsTo(date) <= 0))
                next = rec;

        }
    }

    if (!found) return -1;


    // se vengono trovate una registrazione precedene e una successiva alla data il consumo previsto al momento richiesto è
    // consumo a una data = (differenza di tempo tra le registrazioni più vicine) * (differenza di consumi tra le registrazioni più vicine) /
    // (differenza di tempo tra la data e la registrazione precedente più vicina) + (consumo alla registrazione precedente più vicina)
    // se una delle due non viene trovata, la registrazione precedente e quella successiva coincidono, si presume che non ci siano stati consumi successivi o precedenti e restituisce il valore delle registrazioni
    if (prev.date.secsTo(next.date) == 0)
        return prev.value;
    else
        return (date.toMSecsSinceEpoch() - prev.date.toMSecsSinceEpoch()) * (next.value - prev.value) / (next.date.toMSecsSinceEpoch() - prev.date.toMSecsSinceEpoch()) + prev.value;

}

std::vector<double> getHistogramData(QString clientID, const std::vector<record> *data, QDateTime firstDate, QDateTime lastDate, plotMode mode) {
    std::vector<double> hdata;

    int xNum; //numero di colonne

    switch (mode) {
    case YEAR: xNum = 12; break;
    case MONTH_BY_DAYS:
        xNum = firstDate.date().daysInMonth(); break;
    case MONTH_BY_WEEKS:
        xNum = lastDate.date().weekNumber() - firstDate.date().weekNumber() + 1; break;
    case DAY:
        xNum = 24; break;
    default: return hdata;
    }

   if (!firstDate.isValid() || !lastDate.isValid())
       return hdata;

   double consBefore =getConsAtDateSorted(clientID, firstDate.addSecs(-1), data); // getConsAtDate(clientID, firstDate.addDays(-1), data); //consumo prima del periodo
   if (consBefore<0) consBefore=0; //se non viene trovato

    for (int i=0; i<xNum; ++i) {
        switch (mode) {
        case YEAR: firstDate = firstDate.addMonths(1); break;
        case MONTH_BY_DAYS: firstDate = firstDate.addDays(1); break;
        case MONTH_BY_WEEKS:
        {
            int t = firstDate.date().weekNumber();
            while (firstDate.date().weekNumber() == t)
                firstDate = firstDate.addDays(1);
        }
            break;
        case DAY:
            firstDate = firstDate.addSecs(60*60); break; //1 hour
        }

        double test =getConsAtDateSorted(clientID, firstDate, data); getConsAtDate(clientID, firstDate, data);
        //double testm = (i == 0 ? consBefore : hdata[i-1]);

        hdata.push_back(test - consBefore);
        consBefore = test;

    }

    return hdata;
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
