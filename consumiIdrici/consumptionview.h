#ifndef CONSUMPTIONVIEW_H
#define CONSUMPTIONVIEW_H

#include <QDialog>

namespace Ui {
class ConsumptionView;
}

class ConsumptionView : public QDialog
{
    Q_OBJECT

public:
    explicit ConsumptionView(QWidget *parent = 0);
    ~ConsumptionView();

private:
    Ui::ConsumptionView *ui;
};

#endif // CONSUMPTIONVIEW_H
