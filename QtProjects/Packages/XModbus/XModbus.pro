
include(../../global.pri)

QT += core gui widgets
QT += serialport

CONFIG += c++11
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


#include
INCLUDEPATH += $${ProPATH}/project_3.1/include

#log4cxx
INCLUDEPATH += $${ProPATH}/3rdparty/log4cxx/include
CONFIG(release, debug|release) {
LIBS += $${ProPATH}/3rdparty/log4cxx/Release/log4cxx.lib
} else {
LIBS += $${ProPATH}/3rdparty/log4cxx/Debug/log4cxx.lib
}

#modbus
INCLUDEPATH += $${ProPATH}/3rdparty/libmodbus/include
LIBS += $${ProPATH}/3rdparty/libmodbus/VS2015/lib/Release/modbus.lib



HEADERS += \
    xmodbus.h \
    managertool.h

SOURCES += \
    main.cpp \
    xmodbus.cpp \
    managertool.cpp

FORMS += \
    managertool.ui
