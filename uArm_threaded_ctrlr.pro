QT       += widgets serialport

TARGET = uArm_threaded_ctrlr
TEMPLATE = app
CONFIG += c++11

HEADERS += \
    uarm_ifx.h \
    uarm_controller.h \
    servo.h \
    fixedendian.h \
    var_st_xport.h \
    cmd_types.h


SOURCES += \
    main.cpp \
    uarm_ifx.cpp \
    servo.cpp \
    uarm_controller.cpp

FORMS += \
    uarm_controller.ui
