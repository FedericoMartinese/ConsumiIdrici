#ifndef CONSUMPTION_H
#define CONSUMPTION_H

#include <QDateTime>

class Consumption
{
private:
    QDateTime m_date;
    double m_value;
public:
    Consumption();
    Consumption(QDateTime date, double value);

    QDateTime date() const;
    double value() const;
    bool isValid() const;

    //operatore assegnamento usato ma va bene quello di default

    bool operator == (const Consumption& other) const;
    bool operator != (const Consumption& other) const;

    bool operator <  (const Consumption& other) const; //operatore < per ordinamento
    bool operator <= (const Consumption& other) const; //non necessari ma messi per rispettare la logica (se esistono < e == devono esistere >, <= e >=)
    bool operator >  (const Consumption& other) const;
    bool operator >= (const Consumption& other) const;
};

#endif // CONSUMPTION_H
