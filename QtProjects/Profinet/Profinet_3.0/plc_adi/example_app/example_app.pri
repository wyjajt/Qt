############################################################

INCLUDEPATH += $${PWD}
INCLUDEPATH += $${PWD}/../abcc_obj/nw_obj

HEADERS += \
    $${PWD}/abcc_setup.h \
    $${PWD}/ad_map.h \
    $${PWD}/appl_abcc_handler.h \
    $${PWD}/appl_adi_config.h \
    $${PWD}/appl_adimap.h \
    $${PWD}/appl_comm.h \
    $${PWD}/appl_exception.h \
    $${PWD}/appl_state.h \
    $${PWD}/appl_userinit.h

SOURCES += \
    $${PWD}/abcc_setup.c \
    $${PWD}/ad_map.c \
    $${PWD}/appl_abcc_handler.c \
    $${PWD}/appl_adimap_alltypes.c \
    $${PWD}/appl_adimap_separate16.c \
    $${PWD}/appl_adimap_simple16.c \
    $${PWD}/appl_adimap_speed.c \
    $${PWD}/appl_comm.c \
    $${PWD}/appl_exception.c \
    $${PWD}/appl_state.c \
    $${PWD}/appl_userinit.c

