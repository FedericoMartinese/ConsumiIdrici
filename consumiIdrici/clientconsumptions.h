#ifndef CLIENTCONSUMPTIONS_H
#define CLIENTCONSUMPTIONS_H

#include <vector>
#include <consumption.h>
#include <set>

class clientConsumptions
{
private:
    QString m_clientID;
    mutable std::set<consumption, consCompare> m_cons;

public:
    clientConsumptions();
    clientConsumptions(QString clientID);

    void addCons(consumption cons) const;

    QString clientID() const;
    std::set<consumption, consCompare> cons() const;

    bool isValid() const;

    enum histogramStep {
        HOUR,
        DAY,
        MONTH
    };

    consumption getLast() const;
    double getPeriodConsumption(QDateTime firstDate, QDateTime lastDate) const;
    double getConsAtDate(QDateTime date) const;
    std::vector<double> getHistogramData(QDateTime begin, QDateTime end, histogramStep step) const;


    //get valore medio per ...

    bool operator == (clientConsumptions const& other) const; // usato???
    bool operator != (clientConsumptions const& other) const; // non necessario ma messo per rispettare la logica (se esiste == deve esistere !=)

    bool operator <  (clientConsumptions const& other) const; //operatore < per ordinamento
    bool operator <= (clientConsumptions const& other) const;  //non necessari ma messi per rispettare la logica (se esistono < e == devono esistere >, <= e >=)
    bool operator >  (clientConsumptions const& other) const;
    bool operator >= (clientConsumptions const& other) const;


};

struct clientConsCompare {
  bool operator() (const clientConsumptions& lhs, const clientConsumptions& rhs) const
  {return lhs<rhs;}
};
#endif // CLIENTCONSUMPTIONS_H
