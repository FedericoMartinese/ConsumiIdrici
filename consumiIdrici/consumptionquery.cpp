#include "consumptionquery.h"
#include "ui_consumptionquery.h"

ConsumptionQuery::ConsumptionQuery(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConsumptionQuery)
{
    ui->setupUi(this);
}

ConsumptionQuery::~ConsumptionQuery()
{
    delete ui;
}
