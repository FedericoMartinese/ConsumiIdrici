#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <datautility.h>
#include <plot.h>
#include <leakstablemodel.h>
#include <avgtablemodel.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_openFileDialog_clicked();

    void on_histogramModeCombo_currentIndexChanged(int index);

    void on_clientID_view_editingFinished();

    void on_clientID_query_editingFinished();

    void on_firstDate_dateChanged(const QDate &date);

    void on_lastDate_dateChanged(const QDate &date);

    void on_histogramDate_dateChanged(const QDate &date);

    void on_tabWidget_currentChanged(int index);

    void on_leaksClient_currentIndexChanged(int index);

    void on_thresholdSpinbox_editingFinished();

private:
    Ui::MainWindow *ui;
    std::map<QString, consumptionSet> m_data;
    leaksTableModel leaksModel;
    avgTableModel avgModel;
    std::vector<int> clientMap;
    Plot* plot;

    void updateViewTab();
    void updateQueryTab();
    void updateAnalysisTab();
    double avgDaysInMonth(int firstM, int lastM);
    bool hasReadFile = false;
    void updatePlotValues(bool visible);

};

#endif // MAINWINDOW_H
