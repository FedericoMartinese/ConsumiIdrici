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
    //inizializzazione finestra

    ui->setupUi(this);
    ui->tabWidget->setEnabled(false); //disabled fino a quando un file viene caricato correttamente

    //carica scritte nella combobox
    ui->histogramModeCombo->addItem(SYEAR);
    ui->histogramModeCombo->addItem(SMONTH_D);
    ui->histogramModeCombo->addItem(SMONTH_W);
    ui->histogramModeCombo->addItem(SDAY);

    //il costruttore di Plot ha bisogno di un puntatore all'oggetto QCustomPlot dell'interfaccia
    //quindi non è possibile chiamarlo nella lista di inizializzazione
    try {
        plot = new Plot(ui->customPlot);
    } catch (...) {
        plot = nullptr;
    }

    //impostazione date
    ui->firstDate->setMinimumDate(minDate);
    ui->firstDate->setMaximumDate(maxDate);
    ui->lastDate->setMinimumDate(minDate);
    ui->lastDate->setMaximumDate(maxDate);
    ui->firstDate->setDate(minDate);
    ui->lastDate->setDate(maxDate);

    ui->histogramDate->setMinimumDate(minDate);
    ui->histogramDate->setMaximumDate(maxDate);

    //prima tab selezionata
    ui->tabWidget->setCurrentIndex(0);

    //impostazioni tabelle della tab di analisi
    ui->leaksTable->verticalHeader()->hide();
    ui->leaksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);   //larghezza colonne non modificabile
    ui->leaksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //larghezza colonne streched

    ui->avgTable->verticalHeader()->hide();
    ui->avgTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);   //larghezza colonne non modificabile
    ui->avgTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //larghezza colonne streched

    //inizializzazione threshold
    ui->thresholdSpinbox->setSingleStep(0.05);
    ui->thresholdSpinbox->setMinimum(0);
    ui->thresholdSpinbox->setMaximum(99.99);
    ui->thresholdSpinbox->setValue(0.2);

    //nasconde i valori del grafico
    updatePlotValues(false);
}

MainWindow::~MainWindow()
{
    delete plot; //se nullptr non crea problemi
    delete ui;
}


void MainWindow::on_openFileDialog_clicked()
{
    //apertura file .csv

    this->ui->tabWidget->setEnabled(false); //disabilita l'interfaccia finché il caricamento non è completo
    QString tmpFileLoaded = ui->loadedFileName->text(); //memorizza la scritta attuale
    ui->loadedFileName->setText("Apertura file in corso...");

    //seleziona un file .csv da cui leggere i dati
    QString fileName = QFileDialog::getOpenFileName(this, "Open consumptions file", QDir::current().absolutePath(), "CSV Files (*.csv)");

    if (!fileName.isEmpty()) { //se il file è stato selezionato
        m_data = InputFile(fileName).read(this); //legge il file

        if (!m_data.empty()) {
            this->ui->loadedFileName->setText(QFileInfo(fileName).fileName());
            hasReadFile = true;
        } else {
            this->ui->loadedFileName->setText("Apertura fallita");
        }        

        //ripristina tutta l'interfaccia per prepararla al nuovo file
        ui->leaksTable->setModel(nullptr);
        ui->leaksUser->clear();
        usersMap.clear();
        ui->avgTable->setModel(nullptr);
        ui->userID_query->clear();
        ui->userID_view->clear();
        plot->clear();
        updatePlotValues(false);
        updateViewTab();
        updateQueryTab();

        //dato che è un'operazione lunga avvia l'analisi dei consumi solo se la tab selezionata è quella di analisi
        if (ui->tabWidget->currentIndex() == 2)
            updateAnalysisTab();
    } else {
        this->ui->loadedFileName->setText(tmpFileLoaded); //apertura annullata, torna com'era prima
    }

    this->ui->tabWidget->setEnabled(!m_data.empty()); //se il file è stato letto con successo abilita l'interfaccia grafica

}

void MainWindow::on_userID_view_textChanged()
{
    //ricerca utente tab visualizzazione
    updateViewTab();
}

void MainWindow::on_userID_query_textChanged()
{
    //ricerca utente tab interrogazione
    updateQueryTab();
}

void MainWindow::on_firstDate_dateChanged(const QDate &date)
{
    //interrogazione per data
    ui->lastDate->setMinimumDate(date); //lastDate non può avere una data precedente a firstDate
    updateQueryTab();
}

void MainWindow::on_lastDate_dateChanged(const QDate &date)
{
    //interrogazione per data
    ui->firstDate->setMaximumDate(date); //firstDate non può avere una data successiva a lastDate
    updateQueryTab();
}

void MainWindow::on_histogramDate_dateChanged()
{
    //ricerca per data nella tab di visualizzazione
    updateViewTab();
}

void MainWindow::on_histogramModeCombo_currentIndexChanged(int index)
{
    //cambia modalità di visualizzazione del grafico

    //aggiorna formato data
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
    ui->histogramDate->setEnabled(index != Plot::plotMode::YEAR); //se la modalità è annuale non è possibile cambiare data (solo 2015)

    //aggiorna la tab visualizzazione
    updateViewTab();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    //l'analisi dei dati viene effettuata solo quando viene selezionata la tab corrispondente
    if (index == 2)
        updateAnalysisTab();
}

void MainWindow::on_leaksUser_currentIndexChanged(int index)
{
    //mostra nella tabella perdite solo le perdite relative all'utente selezionato
    if (ui->leaksTable->model()!=nullptr) {
        for (int i=0; i<ui->leaksTable->model()->rowCount(); ++i) {
            if (i<usersMap[index] || i>=usersMap[index+1])
                ui->leaksTable->hideRow(i); //nasconde le perdite non relative all'utente
            else
                ui->leaksTable->showRow(i); //mostra le perdite relative all'utente
        }
    }

}

void MainWindow::on_analysisButton_clicked()
{
    //svuota la tabella perdite
    ui->leaksTable->setModel(nullptr);
    ui->leaksUser->clear();
    usersMap.clear();
    //effettua la nuova analisi
    updateAnalysisTab();
}

void MainWindow::updateViewTab() {
    if (!hasReadFile || m_data.empty()) return;

    if (m_data.find(ui->userID_view->text()) != m_data.end()) {
        //il consumo totale è l'ulimo in ordine temporale
        Consumption totalCons = m_data[ui->userID_view->text()].getLast();
        ui->totalConsumption->setText(QString::number(totalCons.value()) + " m^3");
        ui->lastUpdated->setText("aggiornato al " + totalCons.date().toString("dd/MM/yyyy hh:mm:ss"));

        //range di date in base alla modalità del grafico
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
            //il grafico funziona con step costanti. la visualizzazione per settimane non ha step costanti, dato che all'interno di un singolo mese possono
            //ricadere solo parzialmente. viene gestita come quella del mese per giorni ed i valori dei giorni vengono poi accorpati da lunedì a domenica
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

        //calcolo consumi per ogni step
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

        //disegna il grafico se i dati sono validi, altrimenti lo pulisce e nasconde i valori
        if (plot != nullptr) {
            if (hdata.empty()) {
                plot->clear();
                updatePlotValues(false);
            } else {
                plot->draw(mode, hdata);
                updatePlotValues(true);
            }
        }
    } else {
        //utente non trovato
        ui->totalConsumption->setText("n.d");
        ui->lastUpdated->setText("");
        if (plot != nullptr) plot->clear(); //pulisce grafico
        updatePlotValues(false); //nasconde i valori del grafico

    }

}

void MainWindow::updateQueryTab() {
    if (!hasReadFile || m_data.empty()) return;

    QDateTime firstDate(ui->firstDate->date(), QTime(0,0), Qt::TimeSpec::UTC);
    QDateTime lastDate(ui->lastDate->date(), QTime(23,59,59), Qt::TimeSpec::UTC);

    double periodCons = 0;

    if (m_data.find(ui->userID_query->text()) != m_data.end()) {
        //consumo totale nel periodo
        periodCons = m_data[ui->userID_query->text()].getPeriodConsumption(firstDate, lastDate);
        ui->periodTotalCons->setText(QString::number(periodCons));

        //calcolo consumo orario/giornaliero/settimanle/mensile

        //consumo orario e giornaliero sempre calcolabili (la differenza minima tra le due date è di 1 giorno
        int daysDiff = lastDate.date().dayOfYear() - firstDate.date().dayOfYear() + 1;
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
    } else {
        //utente non trovato
        ui->periodTotalCons->setText("Dati non trovati");
        ui->hourConsumption->setText(ND);
        ui->dayConsumption->setText(ND);
        ui->weekConsumption->setText(ND);
        ui->monthConsumption->setText(ND);
    }
}

void MainWindow::updateAnalysisTab() {
    //analisi dei consumi

    //PERDITE
    if (ui->leaksTable->model() == nullptr) { //l'analisi va fatta solo se la tabella è vuota
        //maschera di avanzamento dell'analisi
        QProgressDialog progress(this, Qt::WindowTitleHint);
        progress.setLabelText("Analisi consumi...");
        progress.setWindowTitle("Consumi idrici");
        progress.setRange(0, m_data.size());
        progress.setModal(true);
        progress.setCancelButton(0);
        progress.show();
        qApp->processEvents();

        std::size_t i = 0;
        std::vector<Consumption> leaks;

        for (mapIterator user : m_data) {
            //per ogni utente trova tutte le notti in cui ci sono state perdite notturne
            std::vector<Consumption> nights = user.second.getNightLeaks(ui->thresholdSpinbox->value());
            //se ce ne sono, le aggiunge al modello della tabella
            if (!nights.empty()) {
                usersMap.push_back(leaks.size()); //posizione di inizio del nuovo utente, fine del precedente
                leaks.insert(leaks.end(), nights.begin(), nights.end()); //aggiunge in coda il vector
                ui->leaksUser->addItem(user.first); //aggiunge utente alla combobox
            }

            progress.setValue(++i);
        }
        usersMap.push_back(leaks.size()); //fine dell'ultimo utente

        //carica nella tabella le perdite di tutti gli utenti
        leaksModel.load(leaks);
        ui->leaksTable->setModel(&leaksModel);

        //mostra il primo utente
        if (ui->leaksUser->count() > 0)
            ui->leaksUser->setCurrentIndex(0);

    }

    //UTENZE DEVIANTI
    if (ui->avgTable->model() == nullptr) { //l'analisi va fatta solo se la tabella è vuota
        QDateTime min(minDate, QTime(0,0), Qt::TimeSpec::UTC), max(maxDate, QTime(23,59,59), Qt::TimeSpec::UTC);

        //calcolo consumo medio
        double avg = 0;
        std::size_t i = 0;
        for (mapIterator user : m_data) {
            double c = user.second.getPeriodConsumption(min ,max);
            if (c>=0) {
                avg += c;
                ++i;
            }
        }
        avg /= i;

        //ricerca utenti con consumo superiore al doppio della media
        std::vector<std::vector<QString>> devusers;
        int days = min.daysTo(max) + 1;
        int weeks = max.date().weekNumber() - min.date().weekNumber() + 1;
        int months = max.date().month() - min.date().month() + 1;

        for (mapIterator user : m_data) {
            double c = user.second.getPeriodConsumption(min, max);
            if (c >= (2*avg)) {
                //memorizza consumo giornaliero, settimanel e mensile dell'utenza deviante
                devusers.push_back({user.first, QString::number(c/days, 'f', 3), QString::number(c/weeks, 'f', 3), QString::number(c/months, 'f', 3)});
            }
        }

        //carica tabella
        avgModel.load(devusers);
        ui->avgTable->setModel(&avgModel);

        //stampa consumi medi
        ui->dailyAvg->setText(QString::number(avg/days, 'f', 3));
        ui->weeklyAvg->setText(QString::number(avg/weeks, 'f', 3));
        ui->monthlyAvg->setText(QString::number(avg/months, 'f', 3));

    }

}

void MainWindow::updatePlotValues(bool visible) {
    //aggiorna o nasconde i valori del grafico
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
