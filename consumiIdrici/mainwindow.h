#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <inputfile.h>
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

    void on_userID_view_textChanged();

    void on_userID_query_textChanged();

    void on_firstDate_dateChanged(const QDate &date);

    void on_lastDate_dateChanged(const QDate &date);

    void on_histogramDate_dateChanged();

    void on_histogramModeCombo_currentIndexChanged(int index);

    void on_tabWidget_currentChanged(int index);

    void on_leaksUser_currentIndexChanged(int index);

    void on_analysisButton_clicked();

    void on_devUsers_startDate_dateChanged(const QDate &date);

    void on_devUsers_endDate_dateChanged(const QDate &date);

private:
    Ui::MainWindow *ui;
    std::map<QString, ConsumptionSet> m_data;
    typedef std::pair<const QString, ConsumptionSet> mapIterator;
    LeaksTableModel leaksModel;
    AvgTableModel avgModel;
    std::vector<int> usersMap; //traccia gli utenti nella tabella delle perdite
    Plot* plot;
    bool hasReadFile;

    void updateViewTab();
    void updateQueryTab();
    void updateAnalysisTab();
    void updatePlotValues(bool visible);
    void findDevUsers();

};

#endif // MAINWINDOW_H
