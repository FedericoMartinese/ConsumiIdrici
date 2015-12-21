#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <consumptionanalysis.h>
#include <consumptionquery.h>
#include <consumptionview.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_viewButton_clicked()
{
    ConsumptionView v(this);
    v.exec(); //show solo se sono modali
}

void MainWindow::on_queryButton_clicked()
{
    ConsumptionQuery q(this);
    q.exec();
}

void MainWindow::on_analysisButton_clicked()
{
    consumptionAnalysis a(this);
    a.exec();
}
