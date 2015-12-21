#ifndef CONSUMPTIONQUERY_H
#define CONSUMPTIONQUERY_H

#include <QDialog>

namespace Ui {
class ConsumptionQuery;
}

class ConsumptionQuery : public QDialog
{
    Q_OBJECT

public:
    explicit ConsumptionQuery(QWidget *parent = 0);
    ~ConsumptionQuery();

private:
    Ui::ConsumptionQuery *ui;
};

#endif // CONSUMPTIONQUERY_H
