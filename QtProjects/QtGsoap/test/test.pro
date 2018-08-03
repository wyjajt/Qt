QT += gui widgets

CONFIG += c++11
#CONFIG +=  console

CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    soapC.cpp \
    soapitoaProxy.cpp \
    stdsoap2.cpp \
    dialog.cpp

HEADERS += \
#    itoa.nsmap \
    soapitoaProxy.h \
    stdsoap2.h \
    soapH.h \
    soapStub.h \
    dialog.h

FORMS += \
    dialog.ui
