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
    qcustomplot.cpp \
    consumptionset.cpp \
    consumption.cpp \
    leakstablemodel.cpp \
    plot.cpp \
    avgtablemodel.cpp \
    inputfile.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    consumptionset.h \
    consumption.h \
    leakstablemodel.h \
    plot.h \
    avgtablemodel.h \
    inputfile.h \
    dates.h

FORMS    += mainwindow.ui
CONFIG += C++11

win32:RC_ICONS += icon.ico
