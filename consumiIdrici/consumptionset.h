#ifndef CLIENTCONSUMPTIONS_H
#define CLIENTCONSUMPTIONS_H

#include <consumption.h>
#include <set>

//std::set wrapper with extended functions

class ConsumptionSet
{
private:
    std::set<Consumption> m_cons;

public:
    ConsumptionSet();

    bool insert(Consumption cons);

    //std::set<consumption> cons() const;
    //bool isEmpty() const;

    enum histogramStep {
        HOUR,
        DAY,
        MONTH
    };

    Consumption getLast() const;
    double getPeriodConsumption(QDateTime firstDate, QDateTime lastDate) const;
    double getConsAtDate(QDateTime date) const;
    std::vector<double> getHistogramData(QDateTime begin, QDateTime end, histogramStep step) const;
    std::vector<Consumption> getNightLeaks(double threshold) const;
};

#endif // CLIENTCONSUMPTIONS_H
