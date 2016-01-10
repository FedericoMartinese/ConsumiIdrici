#ifndef DATA_UTILITY_H
#define DATA_UTILITY_H

#include <QDateTime>
#include <vector>

struct record {
    QString clientID;   // identificativo univoco cliente
    QDateTime date;     // data e ora della lettura
    double value;       // valore della lettura
};

const QDate minDate(2015,01,01);
const QDate maxDate(2015,12,31);

std::vector<record> readFile(QString fileName);
record getLastRecord(QString clientID, const std::vector<record> *data);
bool getPeriodConsumption(QString clientID, const std::vector<record> *data, QDateTime firstDate, QDateTime lastDate, double &periodConsumption);
double getConsAtDate(QString clientID, QDateTime date, const std::vector<record> *data);


#endif
