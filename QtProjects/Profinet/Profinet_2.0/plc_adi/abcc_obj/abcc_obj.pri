############################################################

INCLUDEPATH += $${PWD}
INCLUDEPATH += $${PWD}/../abcc_drv/inc
INCLUDEPATH += $${PWD}/../example_app

SOURCES += \
    $${PWD}/ad_obj.c \
    $${PWD}/app_obj.c \
    $${PWD}/etn_obj.c \
    $${PWD}/sync_obj.c \
    $${PWD}/nw_obj/cop.c \
    $${PWD}/nw_obj/dev.c \
    $${PWD}/nw_obj/dpv1.c \
    $${PWD}/nw_obj/ect.c \
    $${PWD}/nw_obj/eip.c \
    $${PWD}/nw_obj/epl.c \
    $${PWD}/nw_obj/mod.c \
    $${PWD}/nw_obj/prt.c

HEADERS += \
    $${PWD}/ad_obj.h \
    $${PWD}/app_obj.h \
    $${PWD}/etn_obj.h \
    $${PWD}/sync_obj.h \
    $${PWD}/nw_obj/cop.h \
    $${PWD}/nw_obj/dev.h \
    $${PWD}/nw_obj/dpv1.h \
    $${PWD}/nw_obj/ect.h \
    $${PWD}/nw_obj/eip.h \
    $${PWD}/nw_obj/epl.h \
    $${PWD}/nw_obj/mod.h \
    $${PWD}/nw_obj/prt.h

