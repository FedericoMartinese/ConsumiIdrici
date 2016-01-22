#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <datautility.h>
#include <qcustomplot.h>
#include <set>
#include <tablemodel.h>
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

    void on_leaksClient_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    std::set<clientConsumptions, clientConsCompare> m_data;

    struct nightLeaks {
        QString clientID;
        std::vector<consumption> nights;
        bool operator == (nightLeaks other) {
            return clientID == other.clientID;
        }
    };
    std::vector<nightLeaks> m_leaks;
    tableModel model;

    void updateViewTab();
    void updateQueryTab();
    double avgDaysInMonth(int firstM, int lastM);
    bool hasReadFile = false;

};

#endif // MAINWINDOW_H
