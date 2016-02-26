#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setFixedSize(w.size()); //dimensione finestra non modificabile
    w.show();

    return a.exec();
}
