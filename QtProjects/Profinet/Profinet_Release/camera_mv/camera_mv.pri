#-------------------------------------------------
#
# Camera_MindVision module created by QtCreator 2018-06-29T15:00:58
#
#-------------------------------------------------
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../include/Camera/inc
LIBS += $$PWD/../include/Camera/lib/MVCAMSDK_X64.lib

SOURCES  += \
    $$PWD/mainwindow.cpp \
#    $$PWD/capturethread.cpp

HEADERS  += \
    $$PWD/mainwindow.h \
#    $$PWD/capturethread.h \

FORMS    += \
    $$PWD/mainwindow.ui \
#    $$PWD/camera_mv.ui
