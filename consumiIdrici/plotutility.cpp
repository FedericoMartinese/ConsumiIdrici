#include "plotutility.h"

void drawPlot(QCustomPlot *customPlot, plotMode mode, std::vector<double> data) {
    clearPlot(customPlot);

    customPlot->xAxis->setVisible(true);
    customPlot->yAxis->setVisible(true);

    if (customPlot == nullptr || data.empty())
        return;

    // create empty bar chart objects:
    QCPBars *consumptions = new QCPBars(customPlot->xAxis, customPlot->yAxis);

    customPlot->addPlottable(consumptions);
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

    int xNum = data.size(), i = 1;
    double minValue = data[0], medValue = data[0], maxValue = data[0];
    for (i; i<=xNum; ++i) {
        ticks << i;
        switch (mode) {
        case YEAR: labels << QDate(2015,i,1).toString("MMMM"); break;
        case MONTH_BY_DAYS:
        case MONTH_BY_WEEKS: labels << QString::number(i); break;
        case DAY: labels << QString::number(i).rightJustified(2, '0'); break;
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

    customPlot->xAxis->setAutoTicks(false);
    customPlot->xAxis->setAutoTickLabels(false);
    customPlot->xAxis->setTickVector(ticks);
    customPlot->xAxis->setTickVectorLabels(labels);

    //customPlot->xAxis->setSubTickCount(0);
    //customPlot->xAxis->setTickLength(0, 4);
    customPlot->xAxis->grid()->setVisible(true);
    customPlot->xAxis->setRange(0, xNum + 1); //+1 spazio a dx

    // prepare y axis:
    customPlot->yAxis->setRange(0, maxValue);
    customPlot->yAxis->setLabel("Consumi");
    customPlot->yAxis->grid()->setSubGridVisible(true);
    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    customPlot->yAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    customPlot->yAxis->grid()->setSubGridPen(gridPen);

    // Add data:
    consumptions->setData(ticks, QVector<double>::fromStdVector(data));

    // setup legend:
    customPlot->legend->setVisible(true);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    customPlot->legend->setBrush(QColor(255, 255, 255, 200));
    QPen legendPen;
    legendPen.setColor(QColor(130, 130, 130, 200));
    customPlot->legend->setBorderPen(legendPen);
    QFont legendFont;
    legendFont.setPointSize(10);
    customPlot->legend->setFont(legendFont);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);


    // configure bottom axis to show date and time instead of number:
    customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot->xAxis->setDateTimeFormat("MMMM\nyyyy");
    // set a more compact font size for bottom and left axis tick labels:
    customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    // apply manual tick and tick label for left axis:
    customPlot->yAxis->setAutoTicks(false);
    customPlot->yAxis->setAutoTickLabels(false);
    customPlot->yAxis->setAutoSubTicks(false);

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

    customPlot->yAxis->setTickVector(tickValues);
    customPlot->yAxis->setTickVectorLabels(tickNames);
    customPlot->yAxis->setSubTickCount(0);


    customPlot->replot();

}

void clearPlot(QCustomPlot *customPlot) {
    customPlot->clearItems();
    customPlot->clearPlottables();
}

