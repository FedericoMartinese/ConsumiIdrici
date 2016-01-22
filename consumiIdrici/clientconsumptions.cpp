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
    if (m_cons.empty())
        return consumption();

    return *--m_cons.end();
}


double clientConsumptions::getPeriodConsumption(QDateTime firstDate, QDateTime lastDate) const {
    if (firstDate > lastDate) return -1;
    if (m_cons.empty()) return 0;

    return getConsAtDate(lastDate) - getConsAtDate(firstDate);
}

double clientConsumptions::getConsAtDate(QDateTime date) const{
    if (m_cons.empty()) return 0;

    std::set<consumption>::iterator cons = std::lower_bound(m_cons.begin(), m_cons.end(), consumption(date,0));

    if (cons == m_cons.end()) //non ci sono registrazioni successive. si suppone che dall'ultima registrazione non ci siano stati consumi ulteriori
        return (--m_cons.end())->value(); //il consumo a quella data equivale a quello dell'ultima registrazione

    if (cons == m_cons.begin()) //non ci sono registrazioni precedenti. si suppone che fino alla prima registrazione non ci siano stati consumi
        return cons->value();  //il consimo a quella data equivale a quello della prima registrazione


    std::set<consumption>::iterator prev = std::prev(cons);

    // se vengono trovate una registrazione precedene e una successiva alla data il consumo previsto al momento richiesto è
    // consumo a una data = (differenza di tempo tra le registrazioni più vicine) * (differenza di consumi tra le registrazioni più vicine) /
    // (differenza di tempo tra la data e la registrazione precedente più vicina) + (consumo alla registrazione precedente più vicina)
    // se una delle due non viene trovata, la registrazione precedente e quella successiva coincidono, si presume che non ci siano stati consumi successivi o precedenti e restituisce il valore delle registrazioni
    // si suppone consumo costante tra le due data
    return (date.toMSecsSinceEpoch() - prev->date().toMSecsSinceEpoch()) * (cons->value() - prev->value()) / (cons->date().toMSecsSinceEpoch() - prev->date().toMSecsSinceEpoch()) + prev->value();

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

std::vector<consumption> clientConsumptions::getNightLeaks(double threshold) const {
    std::vector<consumption> nights;
   if (m_cons.empty()) return nights;
   QTime start(0,0), end(5,0);

   QDate date(m_cons.begin()->date().date());
   while (date <= (--(m_cons.end()))->date().date()) {
       double p = getPeriodConsumption(QDateTime(date,start), QDateTime(date,end));
       if (p >= threshold) {
           nights.push_back(consumption(QDateTime(date, QTime(0,0)), p));
       }

       date = date.addDays(1);
   }
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
