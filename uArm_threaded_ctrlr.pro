QT       += widgets serialport

TARGET = uArm_threaded_ctrlr
TEMPLATE = app

HEADERS += \
    uarm_ifx.h \
    uarm_controller.h

SOURCES += \
    main.cpp \
    uarm_ifx.cpp \
    uarm_controller.cpp

FORMS += \
    uarm_controller.ui
