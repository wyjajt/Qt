QT  += core gui

CONFIG += c++11
#CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = Test_VCI_3.0
TEMPLATE = app


include(./plc_adi/plc_adi.pri)
include(./camera_mv/camera_mv.pri)

HEADERS += \
    userinterface.h \
    include/defines.h \
    struct/readdata.h \
    struct/imgbuffer.h


SOURCES += \
    main.cpp \
    userinterface.cpp \
    struct/readdata.cpp \
    struct/imgbuffer.cpp

FORMS += \
    userinterface.ui


#include
LogPATH = $${PWD}/include/log4cxx

#log4cxx
INCLUDEPATH += $${LogPATH}/include
CONFIG(debug, debug|release) {
#LIBS += $${LogPATH}/Debug/VS2013/log4cxx.lib
LIBS += $${LogPATH}/Debug/VS2015/log4cxx.lib
} else {
#LIBS += $${LogPATH}/Release/VS2013/log4cxx.lib
LIBS += $${LogPATH}/Release/VS2015/log4cxx.lib
}
