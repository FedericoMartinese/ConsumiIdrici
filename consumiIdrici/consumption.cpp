#include "consumption.h"
#include "dates.h"
Consumption::Consumption() : m_date(minDate, midnightAM, Qt::TimeSpec::UTC), m_value(0) //valori di default
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

bool Consumption::operator < (const Consumption& other) const {
    //ordinamento per data, poi per valore
    if (m_date == other.date())
        return m_value < other.value();

    return m_date < other.date();
}

bool Consumption::operator <= (const Consumption& other) const {
    return !(other < *this);
}

bool Consumption::operator > (const Consumption& other) const {
    return other < *this;
}

bool Consumption::operator >= (const Consumption& other) const {
    return !(*this < other);
}

bool Consumption::operator==(const Consumption& other) const {
    return m_date == other.date() && m_value == other.value();
}

bool Consumption::operator!=(const Consumption& other) const {
    return !(*this == other);
}
