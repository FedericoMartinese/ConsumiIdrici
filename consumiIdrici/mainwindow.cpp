#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <consumptionswindow.h>
#include <QFileDialog>

#include <QMessageBox>
MainWindow::MainWindow(std::vector<record> *data, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_data = data;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openFileDialog_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open consumptions file", QDir::current().absolutePath(), "CSV Files (*.csv)"); //seleziona un file .csv da cui leggere i dati

    if (!fileName.isEmpty()) { //se il file è stato selezionato
        *m_data = readFile(fileName);


        //debug test
        /*QString a1 = m_data->at(4).date.toString("yyyy-MM-dd HH:mm:ss");
        QMessageBox m1(QMessageBox::Critical, "date", a1, QMessageBox::Ok);
        m1.exec();

        QString a2 = QString::number(m_data->at(4).value);
        QMessageBox m2(QMessageBox::Critical, "value", a2, QMessageBox::Ok);
        m2.exec();

        QString a3 = QString::fromStdString(m_data->at(4).clientID);
        QMessageBox m3(QMessageBox::Critical, "clientID", a3, QMessageBox::Ok);
        m3.exec();*/


        ConsumptionsWindow w(m_data);
        w.exec();

    }

}
