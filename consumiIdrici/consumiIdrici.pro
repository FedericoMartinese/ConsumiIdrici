#-------------------------------------------------
#
# Project created by QtCreator 2015-12-21T18:05:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = consumiIdrici
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    datautility.cpp \
    qcustomplot.cpp \
    consumptionset.cpp \
    consumption.cpp \
    leakstablemodel.cpp \
    plot.cpp \
    avgtablemodel.cpp

HEADERS  += mainwindow.h \
    datautility.h \
    qcustomplot.h \
    plotutility.h \
    consumptionset.h \
    consumption.h \
    leakstablemodel.h \
    plot.h \
    avgtablemodel.h

FORMS    += mainwindow.ui
CONFIG += C++11
