#include "clientconsumptions.h"
#define MINSECSPRECISION 60 //1 minuto di precisione minima

clientConsumptions::clientConsumptions()
{
}

clientConsumptions::clientConsumptions(QString clientID) : m_clientID(clientID) {}


void clientConsumptions::addCons(consumption cons) const {
    m_cons.insert(cons);
}

QString clientConsumptions::clientID() const {
    return m_clientID;
}

std::set<consumption, consCompare> clientConsumptions::cons() const {
    return m_cons;
}

bool clientConsumptions::isValid() const {
    return !m_clientID.isEmpty();
}

consumption clientConsumptions::getLast() const {
    if (m_cons.size() == 0)
        return consumption();

    return *--m_cons.end();
}


double clientConsumptions::getPeriodConsumption(QDateTime firstDate, QDateTime lastDate) const {
    if (firstDate > lastDate) return -1;
    if (m_cons.size() == 0) return 0;

    consumption fPrev(*m_cons.begin()), fNext(*m_cons.begin()), lPrev(*m_cons.begin()), lNext(*m_cons.begin());
    double fCons = -1, lCons = -1;
    bool fNextFound = false;

    for (consumption rec : m_cons) {
        if (qAbs(rec.date().secsTo(firstDate))<=MINSECSPRECISION) { //se viene trovata una registrazione molto vicina è superfluo continuare la ricerca
            fCons = rec.value();
        }
        if (qAbs(rec.date().secsTo(lastDate))<=MINSECSPRECISION) {
            lCons = rec.value();
        }

        if (rec.date() <= firstDate) {
            fPrev = rec;
            fNext = rec; //si spostano gli iteratori avanti
        } else if (!fNextFound){
            fNext = rec; //dato che è ordinato è sicuramente il primo dopo la data cercata
            fNextFound = true; //essendo ordinati viene preso solo il primo ma si continua la ricerca
        }

        if (rec.date() < lastDate) {
            lPrev = rec;
            lNext = rec;
        } else {
            lNext = rec;
            break; //viene preso solo il primo, ricerca terminata
        }


        if (fCons >= 0 && lCons >=0)
            return lCons - fCons; //trovate entrambe le date con buona precisione
    }

    if (fCons < 0) { //se non trovato con buona precisione
        if (fPrev == fNext) //non ci sono registrazioni prima (dopo) la data cercata. si suppone che fino a (da) quel momento non ci siano stati consumi
            fCons = fPrev.value();
        else
            fCons = (firstDate.toMSecsSinceEpoch() - fPrev.date().toMSecsSinceEpoch()) * (fNext.value() - fPrev.value()) / (fNext.date().toMSecsSinceEpoch() - fPrev.date().toMSecsSinceEpoch()) + fPrev.value();
    }

    if (lCons < 0) {
        if (lPrev == lNext)
            lCons = lPrev.value();
        else
            lCons = (lastDate.toMSecsSinceEpoch() - lPrev.date().toMSecsSinceEpoch()) * (lNext.value() - lPrev.value()) / (lNext.date().toMSecsSinceEpoch() - lPrev.date().toMSecsSinceEpoch()) + lPrev.value();
    }
    // se vengono trovate una registrazione precedene e una successiva alla data il consumo previsto al momento richiesto è
    // consumo a una data = (differenza di tempo tra le registrazioni più vicine) * (differenza di consumi tra le registrazioni più vicine) /
    // (differenza di tempo tra la data e la registrazione precedente più vicina) + (consumo alla registrazione precedente più vicina)
    // se una delle due non viene trovata, la registrazione precedente e quella successiva coincidono, si presume che non ci siano stati consumi successivi o precedenti e restituisce il valore delle registrazioni
    // si suppone consumo costante tra le due data

    return lCons - fCons;
}

double clientConsumptions::getConsAtDate(QDateTime date) const{
    if (m_cons.size() == 0) return 0;

    consumption prev(*m_cons.begin()), next(*m_cons.begin());

    for (consumption rec : m_cons) {

            if (qAbs(rec.date().secsTo(date))<=MINSECSPRECISION) { //se viene trovata una registrazione molto vicina è superfluo continuare la ricerca
                return rec.value();
            }

            if (rec.date() < date /* && rec.date > prev.date*/) {//inutile il secondo controllo perché file ordinato, rec sicuramente >= prev
                prev = rec;
                next = rec; //si spostano gli iteratori avanti
            } else {
                next = rec; //dato che è ordinato è sicuramente il primo dopo la data cercata
                break;
            }
    }

    if (prev == next) return prev.value(); //non ci sono registrazioni prima (dopo) la data cercata. si suppone che fino a (da) quel momento non ci siano stati consumi

    // se vengono trovate una registrazione precedene e una successiva alla data il consumo previsto al momento richiesto è
    // consumo a una data = (differenza di tempo tra le registrazioni più vicine) * (differenza di consumi tra le registrazioni più vicine) /
    // (differenza di tempo tra la data e la registrazione precedente più vicina) + (consumo alla registrazione precedente più vicina)
    // se una delle due non viene trovata, la registrazione precedente e quella successiva coincidono, si presume che non ci siano stati consumi successivi o precedenti e restituisce il valore delle registrazioni
    // si suppone consumo costante tra le due data
    return (date.toMSecsSinceEpoch() - prev.date().toMSecsSinceEpoch()) * (next.value() - prev.value()) / (next.date().toMSecsSinceEpoch() - prev.date().toMSecsSinceEpoch()) + prev.value();
}

std::vector<double> clientConsumptions::getHistogramData(QDateTime begin, QDateTime end, histogramStep step) const {
    std::vector<double> hdata;

   if (!begin.isValid() || !end.isValid() || begin >= end)
       return hdata;

   double consBefore = getConsAtDate(begin.addSecs(-1)); //consumo prima
   if (consBefore < 0) consBefore = 0; //se non viene trovato

   while (begin <= end) {
       switch (step) {
       case HOUR: begin = begin.addSecs(3600); break;
       case DAY: begin = begin.addDays(1); break;
       case MONTH: begin = begin.addMonths(1); break;
       }

     double totCons = getConsAtDate(begin);
     hdata.push_back(totCons - consBefore);
     consBefore = totCons;
   }

    return hdata;
}

bool clientConsumptions::operator < (clientConsumptions const& other) const {
    return m_clientID < other.clientID();
}

bool clientConsumptions::operator <= (clientConsumptions const& other) const {
    return m_clientID <= other.clientID(); //*this < other || *this == other;
}

bool clientConsumptions::operator > (clientConsumptions const& other) const {
    return m_clientID > other.clientID();
}

bool clientConsumptions::operator >= (clientConsumptions const& other) const {
    return m_clientID >= other.clientID(); //*this > other || *this == other;
}

bool clientConsumptions::operator==(clientConsumptions const& other) const {
    return m_clientID == other.clientID();
}

bool clientConsumptions::operator!=(clientConsumptions const& other) const {
    return m_clientID != other.clientID(); //!(*this == other);
}
