#-------------------------------------------------
#
# Project created by QtCreator 2015-12-21T18:05:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = consumiIdrici
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    consumptionview.cpp \
    consumptionanalysis.cpp \
    consumptionquery.cpp \
    datautility.cpp

HEADERS  += mainwindow.h \
    consumptionview.h \
    consumptionanalysis.h \
    consumptionquery.h \
    datautility.h

FORMS    += mainwindow.ui \
    consumptionview.ui \
    consumptionanalysis.ui \
    consumptionquery.ui
