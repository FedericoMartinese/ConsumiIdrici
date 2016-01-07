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
    datautility.cpp \
    consumptionswindow.cpp

HEADERS  += mainwindow.h \
    datautility.h \
    consumptionswindow.h

FORMS    += mainwindow.ui \
    consumptionswindow.ui
CONFIG += C++11
