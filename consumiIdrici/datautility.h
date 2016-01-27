#ifndef DATA_UTILITY_H
#define DATA_UTILITY_H

#include <QDateTime>
#include <set>
#include <consumptionset.h>
#include <consumption.h>

const QDate minDate(2015,01,01);
const QDate maxDate(2015,12,31);
const QDateTime dstStartDay(QDate(2015,3,29), QTime(1,0), Qt::UTC); //il passaggio avviene alle 01:00 UTC ultima domenica di marzo e ottobre //  dstStartDay(getLastSunday(3, 2015), QTime(1,0));
const QDateTime dstEndDay(QDate(2015,10,25), QTime(1,0), Qt::UTC);   //  dstEndDay(getLastSunday(10, 2015), QTime(1,0));

std::map<QString, consumptionSet> readFile(QString fileName);

QDateTime UTCtoDayLightSavTime(QDateTime date, int UTC_offset = 1);
QDate getLastSunday(int month, int year);


#endif
