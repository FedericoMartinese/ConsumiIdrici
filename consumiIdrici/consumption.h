#ifndef CONSUMPTION_H
#define CONSUMPTION_H

#include <QDateTime>

class consumption
{
private:
    QDateTime m_date;
    double m_value;
public:
    consumption();
    consumption(QDateTime date, double value);

    QDateTime date() const;
    double value() const;


    bool isValid() const;

    //operatore = (assegnamento) usato ma va bene quello di default

    bool operator == (const consumption& other) const; // usato!
    bool operator != (const consumption& other) const; // non necessario ma messo per rispettare la logica (se esiste == deve esistere !=)

    bool operator <  (const consumption& other) const; //operatore < per ordinamento
    bool operator <= (const consumption& other) const;  //non necessari ma messi per rispettare la logica (se esistono < e == devono esistere >, <= e >=)
    bool operator >  (const consumption& other) const;
    bool operator >= (const consumption& other) const;
};

#endif // CONSUMPTION_H
