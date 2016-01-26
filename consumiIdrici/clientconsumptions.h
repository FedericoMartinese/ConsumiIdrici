#ifndef CLIENTCONSUMPTIONS_H
#define CLIENTCONSUMPTIONS_H

#include <vector>
#include <consumption.h>
#include <set>

//std::set wrapper with extended functions

class clientConsumptions
{
private:
    std::set<consumption> m_cons;

public:
    clientConsumptions();

    void insert(consumption cons);

    //std::set<consumption> cons() const;
    //bool isEmpty() const;

    enum histogramStep {
        HOUR,
        DAY,
        MONTH
    };

    consumption getLast() const;
    double getPeriodConsumption(QDateTime firstDate, QDateTime lastDate) const;
    double getConsAtDate(QDateTime date) const;
    std::vector<double> getHistogramData(QDateTime begin, QDateTime end, histogramStep step) const;
    std::vector<consumption> getNightLeaks(double threshold) const;

    //get valore medio per ...

};

#endif // CLIENTCONSUMPTIONS_H
