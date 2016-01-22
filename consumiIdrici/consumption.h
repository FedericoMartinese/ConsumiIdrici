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

    bool operator == (consumption const& other) const; // usato!
    bool operator != (consumption const& other) const; // non necessario ma messo per rispettare la logica (se esiste == deve esistere !=)

    bool operator <  (consumption const& other) const; //operatore < per ordinamento
    bool operator <= (consumption const& other) const;  //non necessari ma messi per rispettare la logica (se esistono < e == devono esistere >, <= e >=)
    bool operator >  (consumption const& other) const;
    bool operator >= (consumption const& other) const;
};

struct consCompare {
  bool operator() (const consumption& lhs, const consumption& rhs) const
  {return lhs<rhs;}
};
#endif // CONSUMPTION_H
