#ifndef CONSUMPTIONSWINDOW_H
#define CONSUMPTIONSWINDOW_H

#include <QDialog>
#include <datautility.h>

namespace Ui {
class ConsumptionsWindow;
}

class ConsumptionsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConsumptionsWindow(std::vector<record> *data, QWidget *parent = 0);
    ~ConsumptionsWindow();

private:
    Ui::ConsumptionsWindow *ui;
};

#endif // CONSUMPTIONSWINDOW_H
