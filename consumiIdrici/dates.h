#ifndef DATES_H
#define DATES_H
#include <QDateTime>

//da specifica solo il 2015
const QDate minDate(2015,01,01);
const QDate maxDate(2015,12,31);
const QDateTime dstStartDay(QDate(2015,3,29), QTime(1,0), Qt::UTC); //il passaggio avviene alle 01:00 UTC ultima domenica di marzo e ottobre
const QDateTime dstEndDay(QDate(2015,10,25), QTime(1,0), Qt::UTC);
const QTime midnightAM(0,0);
const QTime midnightPM(23,59,59);

#endif // DATES_H
