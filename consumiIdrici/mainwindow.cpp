#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <consumptionswindow.h>
#include <QFileDialog>

#include <QMessageBox>
#define YEAR "Annuale"
#define MONTH "Mensile"
#define DAY "Giornaliero"

MainWindow::MainWindow(std::vector<record> *data, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setEnabled(false); //disabled fino a quando un file viene caricato correttamente

    ui->histogramModeCombo->addItem(YEAR);
    ui->histogramModeCombo->addItem(MONTH);
    ui->histogramModeCombo->addItem(DAY);

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
        /*for (int i=0; i<6;++i) {
        QString a1 = m_data->at(i).date.toString("yyyy-MM-dd HH:mm:ss");
        QMessageBox m1(QMessageBox::Critical, "date", a1, QMessageBox::Ok);
        m1.exec();
        }

        for (int i=12364; i<12364+6;++i) {
        QString a1 = m_data->at(i).date.toString("yyyy-MM-dd HH:mm:ss");
        QString a2 = QString::number(m_data->at(i).value);
        QMessageBox m1(QMessageBox::Critical, "date", a1, QMessageBox::Ok);
        QMessageBox m2(QMessageBox::Critical, "value", a2, QMessageBox::Ok);
        m1.exec();
        m2.exec();
        }

        for (int i=29879; i<29879+6;++i) {
        QString a1 = m_data->at(i).date.toString("yyyy-MM-dd HH:mm:ss");
        QString a2 = QString::number(m_data->at(i).value);
        QMessageBox m1(QMessageBox::Critical, "date", a1, QMessageBox::Ok);
        QMessageBox m2(QMessageBox::Critical, "value", a2, QMessageBox::Ok);
        m1.exec();
        m2.exec();
        }

        QString a2 = QString::number(m_data->at(4).value);
        QMessageBox m2(QMessageBox::Critical, "value", a2, QMessageBox::Ok);
        m2.exec();

        QString a3 = QString::fromStdString(m_data->at(4).clientID);
        QMessageBox m3(QMessageBox::Critical, "clientID", a3, QMessageBox::Ok);
        m3.exec();*/


        QFileInfo fileInfo(fileName);
        this->ui->loadedFileName->setText(fileInfo.fileName());

    } else {
        this->ui->loadedFileName->setText("Apertura fallita");
    }

    this->ui->tabWidget->setEnabled(!fileName.isEmpty());


}
