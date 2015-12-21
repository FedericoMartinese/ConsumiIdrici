#include "consumptionview.h"
#include "ui_consumptionview.h"

ConsumptionView::ConsumptionView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConsumptionView)
{
    ui->setupUi(this);
}

ConsumptionView::~ConsumptionView()
{
    delete ui;
}
