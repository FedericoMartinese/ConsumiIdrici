#include "plot.h"

Plot::Plot(QCustomPlot* plot): m_plot(plot), minValue(0), midValue(0), maxValue(0)
{
    if (plot == nullptr)
        throw "Null pointer";

    //alla costruzione non è visibile
    m_plot->xAxis->setVisible(false);
    m_plot->yAxis->setVisible(false);
}

void Plot::draw(plotMode mode, std::vector<double> data, bool showLegend, bool setInteractions) {
    clear();

    if (m_plot == nullptr || data.empty())
        return;

    m_plot->xAxis->setVisible(true);
    m_plot->yAxis->setVisible(true);

    //crea bar vuota e la aggiunge al grafico
    QCPBars *consumptions = new QCPBars(m_plot->xAxis, m_plot->yAxis);
    m_plot->addPlottable(consumptions);

    //imposta nome
    switch (mode) {
    case YEAR:  consumptions->setName("Consumo annuale"); break;
    case MONTH_BY_DAYS: consumptions->setName("Consumo mensile"); break;
    case MONTH_BY_WEEKS:  consumptions->setName("Consumo settimanale"); break;
    case DAY:   consumptions->setName("Consumo orario"); break;
    }
    //imposta colori
    QPen pen;
    pen.setWidthF(1.2);
    pen.setColor(QColor(255, 0, 0));
    consumptions->setPen(pen);
    consumptions->setBrush(QColor(0, 240, 255, 150));

    //prepara ticks e label asse x
    QVector<double> ticks;
    QVector<QString> labels;

    int xNum = data.size(), i;
    minValue = data[0], midValue = data[0], maxValue = data[0];
    for (i = 1; i<=xNum; ++i) {
        ticks << i;
        switch (mode) {
        case YEAR: labels << QDate(2015,i,1).toString("MMMM"); break; //nome del mese
        case MONTH_BY_DAYS:
        case MONTH_BY_WEEKS: labels << QString::number(i); break; //giorno del mese
        case DAY: labels << QString::number(i-1).rightJustified(2, '0'); break; //ora a due cifre (-1 perché 00-23)
        }

        //ricerca minimo e massimo
        if (i<xNum) {
            if (data[i] < minValue)
                minValue = data[i];
            else if (data[i] > maxValue)
                maxValue = data[i];
            midValue += data[i];
        }
    }
    //calcolo valore medio
    midValue/=i;

    //impostazione non automatica ticks e labels asse x
    m_plot->xAxis->setAutoTicks(false);
    m_plot->xAxis->setAutoTickLabels(false);
    m_plot->xAxis->setTickVector(ticks);
    m_plot->xAxis->setTickVectorLabels(labels);

    m_plot->xAxis->grid()->setVisible(true);
    m_plot->xAxis->setRange(0, xNum + 1); //+1 spazio a dx

    //Asse y
    m_plot->yAxis->setRange(0, maxValue);
    switch (mode) {
    case YEAR:  m_plot->yAxis->setLabel("Consumo mensile"); break;
    case MONTH_BY_DAYS: m_plot->yAxis->setLabel("Consumo giornaliero"); break;
    case MONTH_BY_WEEKS:  m_plot->yAxis->setLabel("Consumo settimanale"); break;
    case DAY:   m_plot->yAxis->setLabel("Consumo orario"); break;
    }
    m_plot->yAxis->grid()->setSubGridVisible(true);
    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    m_plot->yAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    m_plot->yAxis->grid()->setSubGridPen(gridPen);

    //Aggiunta dati
    consumptions->setData(ticks, QVector<double>::fromStdVector(data));

    //Legenda
    m_plot->legend->setVisible(showLegend);
    if (showLegend) {
        m_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
        m_plot->legend->setBrush(QColor(255, 255, 255, 200));
        QPen legendPen;
        legendPen.setColor(QColor(130, 130, 130, 200));
        m_plot->legend->setBorderPen(legendPen);
        QFont legendFont;
        legendFont.setPointSize(10);
        m_plot->legend->setFont(legendFont);

    }

    //Interazioni utente (zoom e spostamento del grafico)
    if (setInteractions) {
        m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    }

    m_plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    m_plot->xAxis->setDateTimeFormat("MMMM\nyyyy");

    m_plot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    m_plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    //Impostazione tick e label asse y
    m_plot->yAxis->setAutoTicks(false);
    m_plot->yAxis->setAutoTickLabels(false);
    m_plot->yAxis->setAutoSubTicks(false);

    const int k = 5; //numero tick da mostrare
    QVector<double> tickValues;
    QVector<QString> tickNames;
    bool min = false, med = false;
    double d = maxValue/k; //differenza tra ogni tick
    int digit;
    if (d>=5) //in base al valore imposta la precisione da visualizzare
        digit = 0;
    else if (d>=2)
        digit = 1;
    else if (d>=0.8)
        digit = 2;
    else
        digit = 3;

    //Aggiunge ai vector i tick calcolati e quelli di minimo, medio e massimo in ordine
    double tickValue = d;
    while ((maxValue - tickValue) > 0) {
        if (minValue < tickValue && !min) {
            tickValues.push_back(minValue);
            min = true;
            tickNames.push_back("Min");
        }
        else if (midValue < tickValue && !med) {
            tickValues.push_back(midValue);
            med = true;
            tickNames.push_back("Medio");
        } else {
            tickValues.push_back(tickValue);
            tickNames.push_back(QString::number(tickValue, 'f', digit));
            tickValue += d;
        }

    }
    tickValues.push_back(maxValue);
    tickNames.push_back("Max");

    //Aggiunge i tick al grafico
    m_plot->yAxis->setTickVector(tickValues);
    m_plot->yAxis->setTickVectorLabels(tickNames);
    m_plot->yAxis->setSubTickCount(0);

    //Disegna grafico
    m_plot->replot();
}

void Plot::clear() {
    //Pulisce e nasconde il grafico
    m_plot->clearItems();
    m_plot->clearPlottables();
    m_plot->xAxis->setVisible(false);
    m_plot->yAxis->setVisible(false);
    m_plot->replot();
}

double Plot::getMinValue() const {
    return minValue;
}

double Plot::getMidValue() const {
    return midValue;
}

double Plot::getMaxValue() const {
    return maxValue;
}


