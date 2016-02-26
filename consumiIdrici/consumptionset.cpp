#include "consumptionset.h"
#define MINSECSPRECISION 60 //1 minuto di precisione minima

ConsumptionSet::ConsumptionSet() : m_cons()
{
}

bool ConsumptionSet::insert(Consumption cons) {
    //inserisce una lettura del contatore nel set dell'utente solo se questa è coerente con i dati già presenti, ossia se non crea periodi di consumo negativo
    //restituisce true se è stata inserita

    if (!m_cons.empty()) {
        //cerca i consumi precedenti e successivi a quello di inserire
        //controlla anche i casi in cui con vada inserito per primo o per ultimo

        std::set<Consumption>::iterator next = m_cons.lower_bound(cons);
        double prev = next == m_cons.begin() ? 0 : std::prev(next)->value(); //se next è il primo consumo memorizzato prev viene settato a 0
        
        if (prev > cons.value() || (next != m_cons.end() && next->value() > cons.value()))
            return false; //se i consumi non sono crescenti (prev <= cons <= next) non viene inserito
    }
    
    m_cons.insert(cons);
    return true;
}

/*std::set<consumption> consumptionSet::cons() const {
    return m_cons;
}
bool consumptionSet::isEmpty() const {
    return m_cons.empty();
}*/

Consumption ConsumptionSet::getLast() const {
    //restituisce ultimo consumo in ordine temporale, che è anche quello con value maggiore
    if (m_cons.empty())
        return Consumption();
    
    return *m_cons.rbegin();
}


double ConsumptionSet::getPeriodConsumption(QDateTime firstDate, QDateTime lastDate) const {
    if (m_cons.empty()) return 0;
    //if (firstDate > lastDate) std::swap(firstDate, lastDate);
    
    return getConsAtDate(lastDate) - getConsAtDate(firstDate);
}

double ConsumptionSet::getConsAtDate(QDateTime date) const{
    if (m_cons.empty()) return 0;

    //ricerca primo consumo successivo
    std::set<Consumption>::iterator cons = std::lower_bound(m_cons.begin(), m_cons.end(), Consumption(date,0));
    
    if (cons == m_cons.end()) //non ci sono registrazioni successive. si suppone che dall'ultima registrazione non ci siano stati consumi ulteriori
        return m_cons.rbegin()->value(); //il consumo alla data richiesta equivale a quello dell'ultima registrazione
    
    if (cons == m_cons.begin()) //non ci sono registrazioni precedenti. si suppone che fino alla prima registrazione non ci siano stati consumi
        return cons->value();  //il consimo alla data cercata equivale a quello della prima registrazione
    
    //ultimo consumo precedente
    std::set<Consumption>::iterator prev = std::prev(cons);
    
    // se vengono trovate una registrazione precedene e una successiva alla data il consumo previsto al momento richiesto è una proporzione
    // consumo a una data = (differenza di tempo tra le registrazioni più vicine) * (differenza di consumi tra le registrazioni più vicine) /
    // (differenza di tempo tra la data e la registrazione precedente più vicina) + (consumo alla registrazione precedente più vicina)
    // si suppone quindi consumo costante tra le due date
    return (date.toMSecsSinceEpoch() - prev->date().toMSecsSinceEpoch()) * (cons->value() - prev->value()) / (cons->date().toMSecsSinceEpoch() - prev->date().toMSecsSinceEpoch()) + prev->value();
    
}

std::vector<double> ConsumptionSet::getHistogramData(QDateTime begin, QDateTime end, histogramStep step) const {
    //restituisce un elenco di consumi tra due date con step costante
    std::vector<double> hdata;
    
    if (!begin.isValid() || !end.isValid() || begin >= end)
        return hdata;
    
    double consBefore = getConsAtDate(begin.addSecs(-1)); //consumo precedente al periodo richiesto
    
    while (begin <= end) {
        switch (step) {
        case HOUR: begin = begin.addSecs(3600); break;
        case DAY: begin = begin.addDays(1); break;
        case MONTH: begin = begin.addMonths(1); break;
        }
        
        double totCons = getConsAtDate(begin);
        hdata.push_back(totCons - consBefore); //consumo tra i due step
        consBefore = totCons;
    }
    
    return hdata;
}

std::vector<Consumption> ConsumptionSet::getNightLeaks(double threshold) const {
    //restituisce tutte le notti in cui ci sono stati consumi superiori alla threshold
    std::vector<Consumption> nights;
    if (m_cons.empty()) return nights;
    QTime start(0,0), end(5,0); //da specifica

    //per ogni giorno
    QDate date(m_cons.begin()->date().date());
    while (date <= (--(m_cons.end()))->date().date()) {
        //il consumo notturno è il consumo tra le ore 00 e 05 di un dato giorno
        double nightCons = getPeriodConsumption(QDateTime(date,start, Qt::TimeSpec::UTC), QDateTime(date,end, Qt::TimeSpec::UTC));
        if (nightCons >= threshold) {
            nights.push_back(Consumption(QDateTime(date, QTime(0,0), Qt::TimeSpec::UTC), nightCons)); //se superiore alla soglia viene aggiunto alle notti con perdite
        }
        
        date = date.addDays(1);
    }
    return nights;
}
