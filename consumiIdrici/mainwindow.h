#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <datautility.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(std::vector<record> *data, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_openFileDialog_clicked();

private:
    Ui::MainWindow *ui;
    std::vector<record> *m_data;
};

#endif // MAINWINDOW_H
