#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);   

    std::vector<record> data;

    MainWindow w(&data);
    w.show();

    return a.exec();
}


