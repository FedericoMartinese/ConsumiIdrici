#ifndef DATES_H
#define DATES_H
#include <QDateTime>

QDate getLastSunday(int month, int year);
const QDate minDate(2015,01,01);
const QDate maxDate(2015,12,31);
const QDateTime dstStartDay(QDate(2015,3,29), QTime(1,0), Qt::UTC); //il passaggio avviene alle 01:00 UTC ultima domenica di marzo e ottobre //  dstStartDay(getLastSunday(3, 2015), QTime(1,0));
const QDateTime dstEndDay(QDate(2015,10,25), QTime(1,0), Qt::UTC);   //  dstEndDay(getLastSunday(10, 2015), QTime(1,0));

/* cercare di utilizzarla per dichiarare le const nel .h
QDate getLastSunday(int month, int year) {
    QDate sunday(year, month, QDate(year,month,1).daysInMonth());
    while (sunday.dayOfWeek() != 7) {
        sunday.addDays(-1);
    }
    return sunday;
}*/

#endif // DATES_H
