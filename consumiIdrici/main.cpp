#include "mainwindow.h"
#include <QApplication>
#include "datautility.h"

#include <QDir>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDir::setCurrent(QCoreApplication::applicationDirPath()); //per il percorso relativo per trovare il file csv
    std::vector<record> data = readFile();


    /*test
    QString a1 = data.at(4).date.toString("yyyy-MM-dd HH:mm:ss");
    QMessageBox m1(QMessageBox::Critical, "date", a1, QMessageBox::Ok);
    m1.exec();

    QString a2 = QString::number(data.at(4).value);
    QMessageBox m2(QMessageBox::Critical, "value", a2, QMessageBox::Ok);
    m2.exec();

    QString a3 = QString::fromStdString(data.at(4).clientID);
    QMessageBox m3(QMessageBox::Critical, "clientID", a3, QMessageBox::Ok);
    m3.exec();
    */

    MainWindow w;
    w.show();

    return a.exec();
}


