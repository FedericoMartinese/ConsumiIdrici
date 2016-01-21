#ifndef CLIENTCONSUMPTIONS_H
#define CLIENTCONSUMPTIONS_H

#include <vector>
#include <consumption.h>

class clientConsumptions
{
private:
    QString m_clientID;
    std::vector<consumption> m_cons;
    bool m_sorted = false;

    void sort();

public:
    clientConsumptions();
    clientConsumptions(QString clientID);

    void addCons(consumption cons);

    QString clientID() const;
    std::vector<consumption> cons() const;

    bool isValid() const;

    enum histogramStep {
        HOUR,
        DAY,
        MONTH
    };

    consumption getLast();
    double getPeriodConsumption(QDateTime firstDate, QDateTime lastDate);
    double getConsAtDate(QDateTime date);
    std::vector<double> getHistogramData(QDateTime begin, QDateTime end, histogramStep step);


    //get valore medio per ...

    bool operator == (clientConsumptions const& other) const; // usato???
    bool operator != (clientConsumptions const& other) const; // non necessario ma messo per rispettare la logica (se esiste == deve esistere !=)

    bool operator <  (clientConsumptions const& other) const; //operatore < per ordinamento
    bool operator <= (clientConsumptions const& other) const;  //non necessari ma messi per rispettare la logica (se esistono < e == devono esistere >, <= e >=)
    bool operator >  (clientConsumptions const& other) const;
    bool operator >= (clientConsumptions const& other) const;


};

#endif // CLIENTCONSUMPTIONS_H
