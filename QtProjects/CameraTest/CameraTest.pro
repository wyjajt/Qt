#-------------------------------------------------
#
# Project created by QtCreator 2018-06-29T15:00:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = CameraTest
TEMPLATE = app

CONFIG += console

INCLUDEPATH += $$PWD/include

LIBS += $$PWD/MVCAMSDK_X64.lib

SOURCES  += \
    main.cpp\
    mainwindow.cpp \
    capturethread.cpp

HEADERS  += \
    mainwindow.h \
    capturethread.h

FORMS    += \
    mainwindow.ui \
