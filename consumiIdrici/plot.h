#ifndef PLOT_H
#define PLOT_H

#include <qcustomplot.h>

class Plot
{
private:
    QCustomPlot* m_plot;
    double minValue;
    double midValue;
    double maxValue;
public:
    Plot(QCustomPlot* plot);

    enum plotMode {
        YEAR = 0,
        MONTH_BY_DAYS = 1,
        MONTH_BY_WEEKS = 2,
        DAY = 3
    };

    void draw(plotMode plotMode, std::vector<double> data, bool showLegend = false, bool setInteractions = false);
    void clear();
    double getMinValue() const;
    double getMidValue() const;
    double getMaxValue() const;
};

#endif // PLOT_H
