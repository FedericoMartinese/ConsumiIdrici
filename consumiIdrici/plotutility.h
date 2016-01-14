#ifndef PLOTUTILITY_H
#define PLOTUTILITY_H

#include <qcustomplot.h>

enum plotMode {
    YEAR = 0,
    MONTH_BY_DAYS = 1,
    MONTH_BY_WEEKS = 2,
    DAY = 3
};

void drawPlot(QCustomPlot* customPlot, plotMode plotMode, std::vector<double> data);
void clearPlot(QCustomPlot *customPlot);

#endif // PLOTUTILITY_H



