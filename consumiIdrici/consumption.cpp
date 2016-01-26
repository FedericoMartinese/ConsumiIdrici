#include "consumption.h"

consumption::consumption() : m_date(), m_value(0)
{
}

consumption::consumption(QDateTime date, double value) : m_date(date), m_value(value) {}

QDateTime consumption::date() const {
    return m_date;
}

double consumption::value() const {
    return m_value;
}


bool consumption::isValid() const {
    return m_date.isValid() && m_value >= 0;
}

bool consumption::operator < (consumption const& other) const {
    if (m_date == other.date())
        return m_value < other.value();

    return m_date < other.date();
}

bool consumption::operator <= (consumption const& other) const {
    return *this < other || *this == other;
}

bool consumption::operator > (consumption const& other) const {
    if (m_date == other.date())
        return m_value > other.value();

    return m_date > other.date();
}

bool consumption::operator >= (consumption const& other) const {
    return *this > other || *this == other;
}

bool consumption::operator==(consumption const& other) const {
    return m_date == other.date() && m_value == other.value();
}

bool consumption::operator!=(consumption const& other) const {
    return !(*this == other);
}
