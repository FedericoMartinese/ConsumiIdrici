#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QProgressDialog>

#include <QMessageBox>
#define SYEAR "Annuale"
#define SMONTH_D "Mensile (giorni)"
#define SMONTH_W "Mensile (settimane)"
#define SDAY "Giornaliero"
#define ND "n.d."
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    leaksModel(parent),
    avgModel(parent)
{
    ui->setupUi(this);
    ui->tabWidget->setEnabled(false); //disabled fino a quando un file viene caricato correttamente

    ui->histogramModeCombo->addItem(SYEAR);
    ui->histogramModeCombo->addItem(SMONTH_D);
    ui->histogramModeCombo->addItem(SMONTH_W);
    ui->histogramModeCombo->addItem(SDAY);

    try {
        plot = new Plot(ui->customPlot);
    } catch (...) {
        plot = nullptr;
    }

    ui->firstDate->setMinimumDate(minDate);
    ui->firstDate->setMaximumDate(maxDate);
    ui->lastDate->setMinimumDate(minDate);
    ui->lastDate->setMaximumDate(maxDate);
    ui->firstDate->setDate(minDate);
    ui->lastDate->setDate(maxDate);

    ui->histogramDate->setMinimumDate(minDate);
    ui->histogramDate->setMaximumDate(maxDate);

    ui->tabWidget->setCurrentIndex(0);

    ui->leaksTable->verticalHeader()->hide();
    ui->leaksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);   //larghezza colonne non modificabile
    ui->leaksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //larghezza colonne streched

    ui->avgTable->verticalHeader()->hide();
    ui->avgTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);   //larghezza colonne non modificabile
    ui->avgTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //larghezza colonne streched

    ui->thresholdSpinbox->setSingleStep(0.05);
    ui->thresholdSpinbox->setMinimum(0);
    ui->thresholdSpinbox->setMaximum(99.99);
    ui->thresholdSpinbox->setValue(0.2);
}

MainWindow::~MainWindow()
{
    delete plot; //se nullptr non crea problemi
    delete ui;
}


void MainWindow::on_openFileDialog_clicked()
{
    this->ui->tabWidget->setEnabled(false); //disabilita l'interfaccia finché il caricamento non è completo
    QString tmpFileLoaded = ui->loadedFileName->text();
    ui->loadedFileName->setText("Apertura file in corso...");


    QString fileName = QFileDialog::getOpenFileName(this, "Open consumptions file", QDir::current().absolutePath(), "CSV Files (*.csv)"); //seleziona un file .csv da cui leggere i dati

    if (!fileName.isEmpty()) { //se il file è stato selezionato
        m_data = readFile(fileName, this);

        //debug test
        /*
        for (int i=0; i<6;++i) {
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

        QString a3 = m_data->at(4).clientID;
        QMessageBox m3(QMessageBox::Critical, "clientID", a3, QMessageBox::Ok);
        m3.exec();*/

        if (!m_data.empty()) {
            this->ui->loadedFileName->setText(QFileInfo(fileName).fileName());
            hasReadFile = true;
        } else {
            this->ui->loadedFileName->setText("Apertura fallita");
        }
    } else {
        this->ui->loadedFileName->setText(tmpFileLoaded); //apertura annullata, torna com'era prima
    }

    this->ui->tabWidget->setEnabled(!m_data.empty());

    ui->leaksTable->setModel(NULL); //pulisce la tabella perdite per prepararla al nuovo file
    ui->leaksClient->clear();
    clientMap.clear();
    ui->avgTable->setModel(NULL);
    ui->clientID_query->clear();
    ui->clientID_view->clear();
    updateViewTab();
    updateQueryTab();
    if (ui->tabWidget->currentIndex() == 2)
        updateAnalysisTab();


}

void MainWindow::on_histogramModeCombo_currentIndexChanged(int index)
{
    switch (index) {
    case Plot::YEAR: ui->histogramDate->setDisplayFormat("yyyy"); break;
    case Plot::MONTH_BY_DAYS:
    case Plot::MONTH_BY_WEEKS:
        ui->histogramDate->setDisplayFormat("MMMM yyyy"); break;
    case Plot::DAY: ui->histogramDate->setDisplayFormat("dd/MM/yyyy"); break;
    default:
        ui->histogramDate->setEnabled(false);
        return;
    }

    ui->histogramDate->setDate(minDate);
    ui->histogramDate->setEnabled(index != Plot::plotMode::YEAR);

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
    if (!hasReadFile || m_data.empty()) return;

    if (m_data.find(ui->clientID_view->text()) == m_data.end()) { //non trovato quindi vuoto
        ui->totalConsumption->setText("n.d");
        ui->lastUpdated->setText("");
        if (plot != nullptr) plot->clear();
    } else {
        consumption totalCons = m_data[ui->clientID_view->text()].getLast();
        ui->totalConsumption->setText(QString::number(totalCons.value()) + " m^3");
        ui->lastUpdated->setText("aggiornato al " + totalCons.date().toString("dd/MM/yyyy hh:mm:ss"));


        Plot::plotMode mode = (Plot::plotMode)ui->histogramModeCombo->currentIndex();
        consumptionSet::histogramStep step;
        QDate first, last;
        switch (mode) {
        case Plot::YEAR:
            first = minDate;
            last = maxDate;
            step = consumptionSet::MONTH;
            break;
        case Plot::MONTH_BY_DAYS:
        case Plot::MONTH_BY_WEEKS:
            first.setDate(ui->histogramDate->date().year(), ui->histogramDate->date().month(), 1);
            last.setDate(ui->histogramDate->date().year(), ui->histogramDate->date().month(), ui->histogramDate->date().daysInMonth());
            step = consumptionSet::DAY;
            break;
        case Plot::DAY:
            first = ui->histogramDate->date();
            last = ui->histogramDate->date();
            step = consumptionSet::HOUR;
            break;
        default:
            if (plot != nullptr) plot->clear();
            return;
        }

        std::vector<double> hdata = m_data[ui->clientID_view->text()].getHistogramData(QDateTime(first, QTime(0,0), Qt::TimeSpec::UTC), QDateTime(last, QTime(23,59), Qt::TimeSpec::UTC), step);

        // somma i consumi da lunedì a domenica per la visualizzazione a settimane
        if (mode == Plot::MONTH_BY_WEEKS) {
            std::vector<double> temp;
            double t = 0;
            for (std::size_t i = 0; i < hdata.size(); ++i) {
                t += hdata[i];
                if (first.addDays(i).dayOfWeek() == 7 || i == hdata.size() - 1) { //alla domenica e alla fine del mese aggiunge
                    temp.push_back(t);
                    t = 0;
                }
            }
            hdata = temp;
        }

        if (plot != nullptr) {
            if (hdata.empty())
                plot->clear();
            else
                plot->draw(mode, hdata);
        }
    }

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
    if (!hasReadFile || m_data.empty()) return;

    QDate firstDate = ui->firstDate->date(), lastDate = ui->lastDate->date();


    double periodCons = 0;

    if (m_data.find(ui->clientID_query->text()) != m_data.end()) {
        periodCons = m_data[ui->clientID_query->text()].getPeriodConsumption(QDateTime(firstDate, QTime(0,0), Qt::TimeSpec::UTC), QDateTime(lastDate, QTime(23,59), Qt::TimeSpec::UTC));
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
    }

    if (m_data.find(ui->clientID_query->text()) == m_data.end() || periodCons < 0) {
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

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 2)
        updateAnalysisTab();
}

void MainWindow::updateAnalysisTab() {
    qint64 temp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QProgressDialog progress(this);
    progress.setLabelText("Analisi consumi...");
    progress.setRange(0, m_data.size()*3); //quando i due cicli verranno uniti va sistemato il range
    progress.setModal(true);
    progress.setCancelButton(0);
    progress.show();
    qApp->processEvents();

    size_t i = 0;

    //PERDITE NOTTURNE ~20 SECONDI PER FILE GRANDE. 1,1 SECONDI FILE PICCOLO (DEBUG)
    if (ui->leaksTable->model() == NULL) {
        std::vector<consumption> leaks;

        //SPOSTARE COME FUNZIONE MEMBRO DI CLIENTCONSUMPTIONS CHE RESTITUISCE I DUE VECTOR DI CONSUMPTION E CLIENTI ?
        for (std::pair<const QString, consumptionSet> client : m_data) {
            std::vector<consumption> nights = client.second.getNightLeaks(ui->thresholdSpinbox->value());
            if (!nights.empty()) {
                clientMap.push_back(leaks.size()); //inizio del nuovo cliente, fine del precedente
                leaks.insert(leaks.end(), nights.begin(), nights.end());
                ui->leaksClient->addItem(client.first);

                //qDebug() << client.first << " - " << nights.size();
            }

            progress.setValue(++i);
        }
        clientMap.push_back(leaks.size()); //fine dell'ultimo cliente

        leaksModel.load(leaks);
        ui->leaksTable->setModel(&leaksModel);

        if (ui->leaksClient->count() > 0)
            ui->leaksClient->setCurrentIndex(0);

        qDebug() << "Perdite notturne: " << (QDateTime::currentDateTime().toMSecsSinceEpoch() - temp);
    }

    //UTENZE DEVIANTI ~0.8 SECONDI PER FILE GRANDE. IRRIVELANTE FILE PICCOLO
    if (ui->avgTable->model() == NULL) {


        //almeno un ciclo si può spostare all'interno di quello sopra
        temp = QDateTime::currentDateTime().toMSecsSinceEpoch();

        QDateTime min(minDate, QTime(0,0), Qt::TimeSpec::UTC), max(maxDate, QTime(23,59,59), Qt::TimeSpec::UTC);
        double avg = 0;
        int i = 0;
        for (std::pair<const QString, consumptionSet> client : m_data) {
            double c = client.second.getPeriodConsumption(min ,max);
            if (c>=0) {
                avg += c;
                ++i;
            }

            progress.setValue(++i);
        }
        avg /= i; //non m_data.size() perché solo utenti con consumo reale (>=0)

        std::vector<std::vector<QString>> devusers;
        int days = min.daysTo(max) + 1;
        int weeks = max.date().weekNumber() - min.date().weekNumber() + 1;
        int months = max.date().month() - min.date().month() + 1;

        for (std::pair<const QString, consumptionSet> client : m_data) {
            double c = client.second.getPeriodConsumption(min, max);
            if (c >= (2*avg)) {
                devusers.push_back({client.first, QString::number(c/days, 'f', 3), QString::number(c/weeks, 'f', 3), QString::number(c/months, 'f', 3)});
            }

            progress.setValue(++i);
        }

        avgModel.load(devusers);
        ui->avgTable->setModel(&avgModel);

        ui->dailyAvg->setText(QString::number(avg/days, 'f', 3));
        ui->weeklyAvg->setText(QString::number(avg/weeks, 'f', 3));
        ui->monthlyAvg->setText(QString::number(avg/months, 'f', 3));

        qDebug() << "Utenze devianti: " << devusers.size() << " - " << (QDateTime::currentDateTime().toMSecsSinceEpoch() - temp);
    }
}

void MainWindow::on_leaksClient_currentIndexChanged(int index)
{
    if (ui->leaksTable->model()!=NULL) {
        for (int i=0; i<ui->leaksTable->model()->rowCount(); ++i) {
            if (i<clientMap[index] || i>=clientMap[index+1])
                ui->leaksTable->hideRow(i);
            else
                ui->leaksTable->showRow(i);
        }
    }

}

void MainWindow::on_thresholdSpinbox_editingFinished()
{
    ui->leaksTable->setModel(NULL); //pulisce la tabella perdite per prepararla al nuovo file
    ui->leaksClient->clear();
    clientMap.clear();
    updateAnalysisTab();
}
