#ifndef DATA_UTILITY_H
#define DATA_UTILITY_H

#include <QDateTime>
#include <vector>
#include <plotutility.h>

struct record {
    QString clientID;   // identificativo univoco cliente
    QDateTime date;     // data e ora della lettura
    double value;       // valore della lettura
};

const QDate minDate(2015,01,01);
const QDate maxDate(2015,12,31);
const QDateTime dstStartDay(QDate(2015,3,29), QTime(1,0)); //il passaggio avviene alle 01:00 UTC ultima domenica di marzo e ottobre //  dstStartDay(getLastSunday(3, 2015), QTime(1,0));
const QDateTime dstEndDay(QDate(2015,10,25), QTime(1,0));   //  dstEndDay(getLastSunday(10, 2015), QTime(1,0));

std::vector<record> readFile(QString fileName);
record getLastRecord(QString clientID, const std::vector<record> *data);
bool getPeriodConsumption(QString clientID, const std::vector<record> *data, QDateTime firstDate, QDateTime lastDate, double &periodConsumption);
double getConsAtDate(QString clientID, QDateTime date, const std::vector<record> *data);
std::vector<double> getHistogramData(QString clientID, const std::vector<record> *data, QDateTime firstDate, QDateTime lastDate, plotMode mode);
QDateTime UTCtoDayLightSavTime(QDateTime date, int UTC_offset = 1);
QDate getLastSunday(int month, int year);

#endif
