QT  += core gui

CONFIG += c++11

INCLUDEPATH += $${PWD}
INCLUDEPATH += ./abcc_obj/nw_obj

include($$PWD/abcc_obj/abcc_obj.pri)
include($$PWD/abcc_drv/abcc_drv.pri)
include($$PWD/abcc_adapt/abcc_adapt.pri)
include($$PWD/abcc_abp/abcc_abp.pri)
include($$PWD/example_app/example_app.pri)

HEADERS += \
    $$PWD/example_app/plc_adi.h

SOURCES += \
    $$PWD/example_app/plc_adi.cpp

