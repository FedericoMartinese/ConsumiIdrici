#ifndef CONSUMPTIONANALYSIS_H
#define CONSUMPTIONANALYSIS_H

#include <QDialog>

namespace Ui {
class consumptionAnalysis;
}

class consumptionAnalysis : public QDialog
{
    Q_OBJECT

public:
    explicit consumptionAnalysis(QWidget *parent = 0);
    ~consumptionAnalysis();

private:
    Ui::consumptionAnalysis *ui;
};

#endif // CONSUMPTIONANALYSIS_H
