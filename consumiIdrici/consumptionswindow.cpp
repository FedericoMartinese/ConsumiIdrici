#include "consumptionswindow.h"
#include "ui_consumptionswindow.h"

ConsumptionsWindow::ConsumptionsWindow(std::vector<record> *data, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConsumptionsWindow)
{
    ui->setupUi(this);
}

ConsumptionsWindow::~ConsumptionsWindow()
{
    delete ui;
}
