#include "consumption.h"
#include "dates.h"
Consumption::Consumption() : m_date(minDate, QTime(0,0), Qt::TimeSpec::UTC), m_value(0)
{
}

Consumption::Consumption(QDateTime date, double value) : m_date(date), m_value(value) {
    if (!m_date.isValid())
        throw "Date is invalid";
    if (m_value < 0)
        throw "Consumption value is invalid";
}

QDateTime Consumption::date() const {
    return m_date;
}

double Consumption::value() const {
    return m_value;
}

bool Consumption::isValid() const {
    return m_date.isValid() && m_value >= 0;
}

bool Consumption::operator < (const Consumption& other) const {
    if (m_date == other.date())
        return m_value < other.value();

    return m_date < other.date();
}

bool Consumption::operator <= (const Consumption& other) const {
    return !(other < *this);

    //return *this < other || *this == other;
}

bool Consumption::operator > (const Consumption& other) const {
    return other < *this;

    /*if (m_date == other.date())
        return m_value > other.value();

    return m_date > other.date();*/
}

bool Consumption::operator >= (const Consumption& other) const {
    return !(*this < other);

    //return *this > other || *this == other;
}

bool Consumption::operator==(const Consumption& other) const {
    return m_date == other.date() && m_value == other.value();
}

bool Consumption::operator!=(const Consumption& other) const {
    return !(*this == other);
}
