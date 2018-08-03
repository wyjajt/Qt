#-------------------------------------------------
#
# Project created by QtCreator 2018-06-27T11:29:04
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++11
CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = qqCheckOnline
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    dialog.cpp \
    gsoap/soapC.cpp \
    gsoap/soapqqOnlineWebServiceSoapProxy.cpp \
    gsoap/stdsoap2.cpp

HEADERS += \
    dialog.h \
#    gsoap/qqOnlineWebServiceSoap.nsmap \
    gsoap/soap12.h \
    gsoap/soapH.h \
    gsoap/soapqqOnlineWebServiceSoapProxy.h \
    gsoap/soapStub.h \
    gsoap/stdsoap2.h

FORMS += \
    dialog.ui
