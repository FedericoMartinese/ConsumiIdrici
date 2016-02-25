#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QProgressDialog>
#include <dates.h>
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

    updatePlotValues(false);
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
        m_data = InputFile(fileName).read(this);

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

    ui->leaksTable->setModel(nullptr); //pulisce la tabella perdite per prepararla al nuovo file
    ui->leaksUser->clear();
    usersMap.clear();
    ui->avgTable->setModel(nullptr);
    ui->userID_query->clear();
    ui->userID_view->clear();
    updateViewTab();
    updateQueryTab();
    if (ui->tabWidget->currentIndex() == 2)
        updateAnalysisTab();


}

void MainWindow::on_userID_view_editingFinished()
{
    ui->userID_query->setText(ui->userID_view->text());
    updateViewTab();
    updateQueryTab();
}

void MainWindow::on_userID_query_editingFinished()
{
    ui->userID_view->setText(ui->userID_query->text());
    updateViewTab();
    updateQueryTab();
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

void MainWindow::on_histogramDate_dateChanged()
{
    updateViewTab();
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

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 2)
        updateAnalysisTab();
}

void MainWindow::on_leaksUser_currentIndexChanged(int index)
{
    if (ui->leaksTable->model()!=nullptr) {
        for (int i=0; i<ui->leaksTable->model()->rowCount(); ++i) {
            if (i<usersMap[index] || i>=usersMap[index+1])
                ui->leaksTable->hideRow(i);
            else
                ui->leaksTable->showRow(i);
        }
    }

}

void MainWindow::on_thresholdSpinbox_editingFinished()
{
    ui->leaksTable->setModel(nullptr); //pulisce la tabella perdite per prepararla al nuovo file
    ui->leaksUser->clear();
    usersMap.clear();
    updateAnalysisTab();
}

void MainWindow::updateViewTab() {
    if (!hasReadFile || m_data.empty()) return;

    if (m_data.find(ui->userID_view->text()) == m_data.end()) { //non trovato quindi vuoto
        ui->totalConsumption->setText("n.d");
        ui->lastUpdated->setText("");
        if (plot != nullptr) plot->clear();
        updatePlotValues(false);
    } else {
        Consumption totalCons = m_data[ui->userID_view->text()].getLast();
        ui->totalConsumption->setText(QString::number(totalCons.value()) + " m^3");
        ui->lastUpdated->setText("aggiornato al " + totalCons.date().toString("dd/MM/yyyy hh:mm:ss"));


        Plot::plotMode mode = (Plot::plotMode)ui->histogramModeCombo->currentIndex();
        ConsumptionSet::histogramStep step;
        QDate first, last;
        switch (mode) {
        case Plot::YEAR:
            first = minDate;
            last = maxDate;
            step = ConsumptionSet::MONTH;
            break;
        case Plot::MONTH_BY_DAYS:
        case Plot::MONTH_BY_WEEKS:
            first.setDate(ui->histogramDate->date().year(), ui->histogramDate->date().month(), 1);
            last.setDate(ui->histogramDate->date().year(), ui->histogramDate->date().month(), ui->histogramDate->date().daysInMonth());
            step = ConsumptionSet::DAY;
            break;
        case Plot::DAY:
            first = ui->histogramDate->date();
            last = ui->histogramDate->date();
            step = ConsumptionSet::HOUR;
            break;
        default:
            if (plot != nullptr) plot->clear();
            return;
        }

        std::vector<double> hdata = m_data[ui->userID_view->text()].getHistogramData(QDateTime(first, QTime(0,0), Qt::TimeSpec::UTC), QDateTime(last, QTime(23,59), Qt::TimeSpec::UTC), step);

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
            if (hdata.empty()) {
                plot->clear();
                updatePlotValues(false);
            } else {
                plot->draw(mode, hdata);
                updatePlotValues(true);
            }
        }
    }

}

void MainWindow::updateQueryTab() {
    if (!hasReadFile || m_data.empty()) return;

    QDateTime firstDate(ui->firstDate->date(), QTime(0,0), Qt::TimeSpec::UTC);
    QDateTime lastDate(ui->lastDate->date(), QTime(23,59,59), Qt::TimeSpec::UTC);


    double periodCons = 0;

    if (m_data.find(ui->userID_query->text()) != m_data.end()) {
        periodCons = m_data[ui->userID_query->text()].getPeriodConsumption(firstDate, lastDate);

        int daysDiff = lastDate.date().dayOfYear() - firstDate.date().dayOfYear() + 1;

        ui->periodTotalCons->setText(QString::number(periodCons));
        ui->hourConsumption->setText(QString::number(periodCons / (daysDiff * 24)));
        ui->dayConsumption->setText(QString::number(periodCons / daysDiff));

        //le settimane possono cadere anche solo parzialmente nel periodo indicato
        ui->weekConsumption->setText(daysDiff >= 7 ? QString::number(periodCons / (lastDate.date().weekNumber() - firstDate.date().weekNumber() + 1)) : ND);

        //lo stesso per i mesi
        int monthsDiff = lastDate.date().month() - firstDate.date().month() + 1;
        if (monthsDiff > 1 || //almeno un mese completo o
            (firstDate.date().day() == 1 && lastDate.date().day() == lastDate.date().daysInMonth())) { //un mese esatto (dall'1 all'ultimo giorno del mese)
            ui->monthConsumption->setText(QString::number(periodCons / monthsDiff));
        } else {
            ui->monthConsumption->setText(ND);
        }
    }

    if (m_data.find(ui->userID_query->text()) == m_data.end() || periodCons < 0) {
        ui->periodTotalCons->setText("Dati non trovati");
        ui->hourConsumption->setText(ND);
        ui->dayConsumption->setText(ND);
        ui->weekConsumption->setText(ND);
        ui->monthConsumption->setText(ND);
    }
}

void MainWindow::updateAnalysisTab() {
    QProgressDialog progress(this);
    progress.setLabelText("Analisi consumi...");
    progress.setWindowTitle("Consumi idrici");
    progress.setRange(0, m_data.size()*3);
    progress.setModal(true);
    progress.setCancelButton(0);
    progress.show();
    qApp->processEvents();

    size_t i = 0;

    //PERDITE NOTTURNE ~20 SECONDI PER FILE GRANDE. 1,1 SECONDI FILE PICCOLO (DEBUG)
    if (ui->leaksTable->model() == nullptr) {
        std::vector<Consumption> leaks;

        for (std::pair<const QString, ConsumptionSet> user : m_data) {
            std::vector<Consumption> nights = user.second.getNightLeaks(ui->thresholdSpinbox->value());
            if (!nights.empty()) {
                usersMap.push_back(leaks.size()); //inizio del nuovo utente, fine del precedente
                leaks.insert(leaks.end(), nights.begin(), nights.end()); //aggiunge in coda il vector
                ui->leaksUser->addItem(user.first); //aggiunge utente alla combobox
            }

            progress.setValue(++i);
        }
        usersMap.push_back(leaks.size()); //fine dell'ultimo utente

        //carica la tabella
        leaksModel.load(leaks);
        ui->leaksTable->setModel(&leaksModel);

        if (ui->leaksUser->count() > 0)
            ui->leaksUser->setCurrentIndex(0);

    }

    //UTENZE DEVIANTI ~0.8 SECONDI PER FILE GRANDE. IRRIVELANTE FILE PICCOLO
    if (ui->avgTable->model() == nullptr) {
        QDateTime min(minDate, QTime(0,0), Qt::TimeSpec::UTC), max(maxDate, QTime(23,59,59), Qt::TimeSpec::UTC);
        double avg = 0;
        int i = 0;
        for (std::pair<const QString, ConsumptionSet> user : m_data) {
            double c = user.second.getPeriodConsumption(min ,max);
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

        for (std::pair<const QString, ConsumptionSet> user : m_data) {
            double c = user.second.getPeriodConsumption(min, max);
            if (c >= (2*avg)) {
                devusers.push_back({user.first, QString::number(c/days, 'f', 3), QString::number(c/weeks, 'f', 3), QString::number(c/months, 'f', 3)});
            }

            progress.setValue(++i);
        }

        //carica tabella
        avgModel.load(devusers);
        ui->avgTable->setModel(&avgModel);

        ui->dailyAvg->setText(QString::number(avg/days, 'f', 3));
        ui->weeklyAvg->setText(QString::number(avg/weeks, 'f', 3));
        ui->monthlyAvg->setText(QString::number(avg/months, 'f', 3));

    }
}

void MainWindow::updatePlotValues(bool visible) {
    ui->minText_label->setVisible(visible);
    ui->minValue_label->setVisible(visible);
    ui->midText_label->setVisible(visible);
    ui->midValue_label->setVisible(visible);
    ui->maxTest_label->setVisible(visible);
    ui->maxValue_label->setVisible(visible);
    if (visible && plot != nullptr) {
        ui->minValue_label->setText(QString::number(plot->getMinValue()));
        ui->midValue_label->setText(QString::number(plot->getMidValue()));
        ui->maxValue_label->setText(QString::number(plot->getMaxValue()));
    }
}
