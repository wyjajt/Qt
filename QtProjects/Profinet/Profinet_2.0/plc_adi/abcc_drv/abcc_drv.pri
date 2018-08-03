############################################################

INCLUDEPATH += $${PWD}

INCLUDEPATH += $${PWD}/../../include/PLC/inc
LIBS        += $${PWD}/../../include/PLC/lib/x64/vciIDLc.lib

SOURCES += \
    $${PWD}/src/abcc_driver.c \
    $${PWD}/src/abcc_handler.c

HEADERS += \
    $${PWD}/inc/abcc.h \
    $${PWD}/inc/abcc_ad_if.h \
    $${PWD}/inc/abcc_port.h \
    $${PWD}/inc/abcc_sys_adapt.h \
    $${PWD}/inc/vciIdType.h
