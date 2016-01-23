#include "plot.h"

Plot::Plot(QCustomPlot* plot): m_plot(plot)
{
    m_plot->xAxis->setVisible(false);
    m_plot->yAxis->setVisible(false);
}

void Plot::draw(plotMode mode, std::vector<double> data) {
    clear();

    if (m_plot == nullptr || data.empty())
        return;

    m_plot->xAxis->setVisible(true);
    m_plot->yAxis->setVisible(true);

    // create empty bar chart objects:
    QCPBars *consumptions = new QCPBars(m_plot->xAxis, m_plot->yAxis);

    m_plot->addPlottable(consumptions);
    // set names and colors:
    QPen pen;
    pen.setWidthF(1.2);

    switch (mode) {
    case YEAR:  consumptions->setName("Consumo annuale"); break;
    case MONTH_BY_DAYS: consumptions->setName("Consumo mensile"); break;
    case MONTH_BY_WEEKS:  consumptions->setName("Consumo settimanale"); break;
    case DAY:   consumptions->setName("Consumo orario"); break;
    }

    pen.setColor(QColor(255, 131, 0));
    consumptions->setPen(pen);
    consumptions->setBrush(QColor(255, 131, 0, 50));

    // prepare x axis with country labels:
    QVector<double> ticks;
    QVector<QString> labels;

    int xNum = data.size(), i;
    double minValue = data[0], medValue = data[0], maxValue = data[0];
    for (i = 1; i<=xNum; ++i) {
        ticks << i;
        switch (mode) {
        case YEAR: labels << QDate(2015,i,1).toString("MMMM"); break;
        case MONTH_BY_DAYS:
        case MONTH_BY_WEEKS: labels << QString::number(i); break;
        case DAY: labels << QString::number(i-1).rightJustified(2, '0'); break; //-1 perché 00-23
        }

        if (i<xNum) {
            if (data[i] < minValue)
                minValue = data[i];
            else if (data[i] > maxValue)
                maxValue = data[i];
            medValue += data[i];
        }
    }
    medValue/=i;

    m_plot->xAxis->setAutoTicks(false);
    m_plot->xAxis->setAutoTickLabels(false);
    m_plot->xAxis->setTickVector(ticks);
    m_plot->xAxis->setTickVectorLabels(labels);

    //m_plot->xAxis->setSubTickCount(0);
    //m_plot->xAxis->setTickLength(0, 4);
    m_plot->xAxis->grid()->setVisible(true);
    m_plot->xAxis->setRange(0, xNum + 1); //+1 spazio a dx

    // prepare y axis:
    m_plot->yAxis->setRange(0, maxValue);
    switch (mode) {
    case YEAR:  m_plot->yAxis->setLabel("Consumo annuale"); break;
    case MONTH_BY_DAYS: m_plot->yAxis->setLabel("Consumo mensile"); break;
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

    // Add data:
    consumptions->setData(ticks, QVector<double>::fromStdVector(data));

    // setup legend:
    m_plot->legend->setVisible(true);
    m_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    m_plot->legend->setBrush(QColor(255, 255, 255, 200));
    QPen legendPen;
    legendPen.setColor(QColor(130, 130, 130, 200));
    m_plot->legend->setBorderPen(legendPen);
    QFont legendFont;
    legendFont.setPointSize(10);
    m_plot->legend->setFont(legendFont);
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);


    // configure bottom axis to show date and time instead of number:
    m_plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    m_plot->xAxis->setDateTimeFormat("MMMM\nyyyy");
    // set a more compact font size for bottom and left axis tick labels:
    m_plot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    m_plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    // apply manual tick and tick label for left axis:
    m_plot->yAxis->setAutoTicks(false);
    m_plot->yAxis->setAutoTickLabels(false);
    m_plot->yAxis->setAutoSubTicks(false);

    const int k = 5;
    QVector<double> tickValues;
    QVector<QString> tickNames;
    bool min = false, med = false;
    double d = maxValue/k;
    int j = 1, digit;
    if (d>=5)
        digit = 0;
    else if (d>=2)
        digit = 1;
    else if (d>=0.8)
        digit = 2;
    else
        digit = 3;
    while (maxValue - d*j > 0) {
        double tickValue = d*j;
        if (minValue < tickValue && !min) {
            tickValues.push_back(minValue);
            min = true;
            tickNames.push_back("Min");
        }
        else if (medValue < tickValue && !med) {
            tickValues.push_back(medValue);
            med = true;
            tickNames.push_back("Medio");
        } else {
            tickValues.push_back(tickValue);
            tickNames.push_back(QString::number(tickValue, 'f', digit));
            ++j;
        }

    }
    tickValues.push_back(maxValue);
    tickNames.push_back("Max");

    m_plot->yAxis->setTickVector(tickValues);
    m_plot->yAxis->setTickVectorLabels(tickNames);
    m_plot->yAxis->setSubTickCount(0);


    m_plot->replot();

}

void Plot::clear() {
    m_plot->clearItems();
    m_plot->clearPlottables();
    m_plot->xAxis->setVisible(false);
    m_plot->yAxis->setVisible(false);
    m_plot->replot();
}

