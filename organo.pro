#-------------------------------------------------
#
# Project created by QtCreator 2014-10-30T16:28:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = organo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connectiondialog.cpp \
    optionsdialog.cpp

HEADERS  += mainwindow.h \
    connectiondialog.h \
    optionsdialog.h

FORMS    += mainwindow.ui \
    connectiondialog.ui \
    optionsdialog.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS += languages/english.ts \
    languages/spanish.ts \
    languages/italian.ts

win32-g++: QMAKE_CXXFLAGS += -std=c++11
