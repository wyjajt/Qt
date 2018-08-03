QT  += core gui

CONFIG += c++11
CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = Test_VCI_1.0
TEMPLATE = app


include(./plc_adi/plc_adi.pri)
include(./camera_mv/camera_mv.pri)

HEADERS += \
    userinterface.h \
    defines.h \
    readdata.h \
    imgbuffer.h


SOURCES += \
    main.cpp \
    userinterface.cpp \
    readdata.cpp \
    imgbuffer.cpp

FORMS += \
    userinterface.ui


#include
LogPATH = ../../../../svn/MachineVision_3.1/trunk

#log4cxx
INCLUDEPATH += $${LogPATH}/project_3.1/include
INCLUDEPATH += $${LogPATH}/3rdparty/log4cxx/include
CONFIG(debug, debug|release) {
#LIBS += $${LogPATH}/3rdparty/log4cxx/Debug/log4cxx.lib
LIBS += $${LogPATH}/3rdparty/log4cxx/VS2015(vc14)/Debug/log4cxx.lib
} else {
#LIBS += $${LogPATH}/3rdparty/log4cxx/Release/log4cxx.lib
LIBS += $${LogPATH}/3rdparty/log4cxx/VS2015(vc14)/Release/log4cxx.lib
}
