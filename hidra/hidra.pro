#-------------------------------------------------
#
# Project created by QtCreator 2014-07-18T17:26:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hidra
TEMPLATE = app


SOURCES += main.cpp\
        hidragui.cpp \
    machine.cpp \
    bit.cpp \
    register.cpp \
    byte.cpp \
    instruction.cpp \
    ramsesmachine.cpp \
    neandermachine.cpp \
    ahmesmachine.cpp

HEADERS  += hidragui.h \
    machine.h \
    bit.h \
    register.h \
    byte.h \
    instruction.h \
    ramsesmachine.h \
    neandermachine.h \
    ahmesmachine.h

FORMS    += hidragui.ui
