#include "consumption.h"
#include <QMessageBox>
consumption::consumption() : m_date(), m_value(0)
{
}

consumption::consumption(QDateTime date, double value) : m_date(date), m_value(value) {
    if (!m_date.isValid())
        throw "Date is invalid";
    if (m_value < 0)
        throw "Consumption value is invalid";
}

QDateTime consumption::date() const {
    return m_date;
}

double consumption::value() const {
    return m_value;
}

bool consumption::isValid() const {
    return m_date.isValid() && m_value >= 0;
}

bool consumption::operator < (const consumption& other) const {
    if (m_date == other.date())
        return m_value < other.value();

    return m_date < other.date();
}

bool consumption::operator <= (const consumption& other) const {
    return !(other < *this);

    //return *this < other || *this == other;
}

bool consumption::operator > (const consumption& other) const {
    return other < *this;

    /*if (m_date == other.date())
        return m_value > other.value();

    return m_date > other.date();*/
}

bool consumption::operator >= (const consumption& other) const {
    return !(*this < other);

    //return *this > other || *this == other;
}

bool consumption::operator==(const consumption& other) const {
    return m_date == other.date() && m_value == other.value();
}

bool consumption::operator!=(const consumption& other) const {
    return !(*this == other);
}
