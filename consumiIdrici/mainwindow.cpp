#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "plotutility.h"

#include <QMessageBox>
#define SYEAR "Annuale"
#define SMONTH_D "Mensile (giorni)"
#define SMONTH_W "Mensile (settimane)"
#define SDAY "Giornaliero"
#define ND "n.d."
MainWindow::MainWindow(std::vector<record> *data, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setEnabled(false); //disabled fino a quando un file viene caricato correttamente

    ui->histogramModeCombo->addItem(SYEAR);
    ui->histogramModeCombo->addItem(SMONTH_D);
    ui->histogramModeCombo->addItem(SMONTH_W);
    ui->histogramModeCombo->addItem(SDAY);

    ui->firstDate->setMinimumDate(minDate);
    ui->firstDate->setMaximumDate(maxDate);
    ui->lastDate->setMinimumDate(minDate);
    ui->lastDate->setMaximumDate(maxDate);
    ui->firstDate->setDate(minDate);
    ui->lastDate->setDate(maxDate);

    ui->customPlot->xAxis->setVisible(false);
    ui->customPlot->yAxis->setVisible(false);

    m_data = data;

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openFileDialog_clicked()
{
    this->ui->tabWidget->setEnabled(false); //disabilita l'interfaccia finché il caricamento non è completo
    QString tmpFileLoaded = ui->loadedFileName->text();
    ui->loadedFileName->setText("Apertura file in corso...");


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

        if (!m_data->empty()) {
            QFileInfo fileInfo(fileName);
            this->ui->loadedFileName->setText(fileInfo.fileName());
            hasReadFile = true;
        } else {
            this->ui->loadedFileName->setText("Apertura fallita");
        }
    } else {
        this->ui->loadedFileName->setText(tmpFileLoaded); //apertura annullata, torna com'era prima
    }

    this->ui->tabWidget->setEnabled(!m_data->empty());


}

void MainWindow::on_histogramModeCombo_currentIndexChanged(int index)
{
    switch (index) {
    case YEAR: ui->histogramDate->setDisplayFormat("yyyy"); break;
    case MONTH_BY_DAYS:
    case MONTH_BY_WEEKS:
        ui->histogramDate->setDisplayFormat("MMMM yyyy"); break;
    case DAY: ui->histogramDate->setDisplayFormat("dd/MM/yyyy"); break;
    default:
        ui->histogramDate->setEnabled(false);
        return;
    }

    ui->histogramDate->setDate(minDate);
    ui->histogramDate->setMinimumDate(minDate);
    ui->histogramDate->setMaximumDate(maxDate);
    ui->histogramDate->setEnabled(index != YEAR);

    updateViewTab();
}


void MainWindow::on_clientID_view_editingFinished()
{
    ui->clientID_query->setText(ui->clientID_view->text());
    updateViewTab();
    updateQueryTab();
}

void MainWindow::on_clientID_query_editingFinished()
{
    ui->clientID_view->setText(ui->clientID_query->text());
    updateViewTab();
    updateQueryTab();
}

void MainWindow::updateViewTab() {
    if (!hasReadFile || m_data == nullptr || m_data->empty()) return;

    record totalCons = getLastRecord(ui->clientID_view->text(), m_data);

    if (totalCons.value<0) {
        clearPlot(ui->customPlot);
        ui->totalConsumption->setText("n.d");
        ui->lastUpdated->setText("");
    } else {
        ui->totalConsumption->setText(QString::number(totalCons.value) + " m^3");
        ui->lastUpdated->setText("(aggiornato al " + totalCons.date.toString("dd/MM/yyyy hh:mm:ss") + ")");        
    }

    plotMode mode = (plotMode)ui->histogramModeCombo->currentIndex();
    QDate first, last;
    switch (mode) {
    case YEAR:
        first = minDate;
        last = maxDate;
        break;
    case MONTH_BY_DAYS:
    case MONTH_BY_WEEKS:
        first.setDate(ui->histogramDate->date().year(), ui->histogramDate->date().month(), 1);
        last.setDate(ui->histogramDate->date().year(), ui->histogramDate->date().month(), ui->histogramDate->date().daysInMonth());
        break;
    case DAY:
        first = ui->histogramDate->date();
        last = ui->histogramDate->date();
        break;
    default:
        clearPlot(ui->customPlot);
        return;
    }
    std::vector<double> hdata = getHistogramData(ui->clientID_view->text(), m_data, QDateTime(first, QTime(0,0)), QDateTime(last, QTime(23,59)), mode);
    if (hdata.empty())
        clearPlot(ui->customPlot);
    else
        drawPlot(ui->customPlot, mode, /*first, last, */ hdata);

    ///TEST
    /*QDateTime first, last;
    switch (mode) {
    case YEAR:
        first = QDateTime(minDate.addMonths(1), QTime(0,0)); //la colonna di gennaio riporta il consumo alle 00:00 del 1° febbraio
        last = QDateTime(minDate.addMonths(12), QTime(0,0));
        break;
    case MONTH_BY_DAYS:
    case MONTH_BY_WEEKS:
        first = QDateTime(QDate(ui->histogramDate->date().year(), ui->histogramDate->date().month(), 1), QTime(0,0));
        last = QDateTime(QDate(ui->histogramDate->date().year(), ui->histogramDate->date().month(), 1),QTime(0,0));
        first.addDays(1); //il conumo di un giorno si conclude alle 00:00 del giorno dopo
        last.addDays(1);
        break;
    case DAY:
        first = QDateTime(ui->histogramDate->date(),QTime(1,0));
        last = QDateTime(ui->histogramDate->date().addDays(1),QTime(0,0));
        break;
    default:
        clearPlot(ui->customPlot);
        return;
    }
    std::vector<double> hdata = getHistogramData(ui->clientID_view->text(), m_data, first, last, mode);
    if (hdata.empty())
        clearPlot(ui->customPlot);
    else
        drawPlot(ui->customPlot, mode, hdata);
    */
}

double MainWindow::avgDaysInMonth(int firstM, int lastM) {
    double avg = 0;
    for (int i = firstM; i<=lastM; ++i)
        switch (i) {
        case 2: avg += 28; break; //si considera solo il 2015
        case 11:
        case 4:
        case 6:
        case 9: avg += 30; break;
        default: avg += 31;
        }
    return avg / (lastM - firstM + 1) ;
}



void MainWindow::updateQueryTab() {
    if (!hasReadFile || m_data == nullptr || m_data->empty()) return;

    QString clientID = ui->clientID_query->text();
    double periodCons;
    QDate firstDate = ui->firstDate->date(), lastDate = ui->lastDate->date();
    if (getPeriodConsumption(clientID, m_data, QDateTime(firstDate, QTime(0,0)), QDateTime(lastDate, QTime(23,59)), periodCons)) {
        double msecDiff = ui->lastDate->dateTime().toMSecsSinceEpoch() - ui->firstDate->dateTime().toMSecsSinceEpoch();
        ui->periodTotalCons->setText(QString::number(periodCons));
        ui->hourConsumption->setText(QString::number(periodCons / msecDiff *1000 * 60 * 60));
        ui->dayConsumption->setText(QString::number(periodCons / msecDiff * 1000 * 60 * 60 * 24));

        //oppure si calcola il consumo giornaliero con la funzione getPeriodConsumption per ogni giorno e si mette in un vector
        //poi si somma da lunedì a domenica in un altro vector e se ne fa la media
        //stessa cosa sullo stesso vector dall'1 al 28/30/31 (QDate::dayofmonth() )

        if (lastDate.dayOfYear() - firstDate.dayOfYear() >= 6) //dalle 00:00 di lunedì alle 23:59 di domenica sono 6 giorni interi, ma ha senso calcolare il consumo settimanale
            ui->weekConsumption->setText(QString::number(periodCons / msecDiff * 1000 * 60 * 60 * 24 * 7)); //settimane comprese parzialmente non deviano la media ma contribuiscono in base a quanti giorni sono considerati
        else
            ui->weekConsumption->setText(ND);
        if (lastDate.month() - firstDate.month() > 1 || (firstDate.day() == 1 && lastDate.day() == lastDate.daysInMonth()))
            ui->monthConsumption->setText(QString::number(periodCons / msecDiff * 1000 * 60 * 60 * 24 * avgDaysInMonth(firstDate.month(), lastDate.month())));
        else
            ui->monthConsumption->setText(ND);
    } else {
        ui->periodTotalCons->setText("Dati non trovati");
        ui->hourConsumption->setText(ND);
        ui->dayConsumption->setText(ND);
        ui->weekConsumption->setText(ND);
        ui->monthConsumption->setText(ND);
    }
}


void MainWindow::on_firstDate_dateChanged(const QDate &date)
{
    ui->lastDate->setMinimumDate(date);
    updateQueryTab();
}
void MainWindow::on_lastDate_dateChanged(const QDate &date)
{
    ui->firstDate->setMaximumDate(date);
    updateQueryTab();
}

void MainWindow::on_histogramDate_dateChanged(const QDate &date)
{
    updateViewTab();
}
