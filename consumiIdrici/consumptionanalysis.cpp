#include "consumptionanalysis.h"
#include "ui_consumptionanalysis.h"

consumptionAnalysis::consumptionAnalysis(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::consumptionAnalysis)
{
    ui->setupUi(this);
}

consumptionAnalysis::~consumptionAnalysis()
{
    delete ui;
}
