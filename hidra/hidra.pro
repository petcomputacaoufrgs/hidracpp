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
    neandermachine.cpp

HEADERS  += hidragui.h \
    machine.h \
    bit.h \
    register.h \
    byte.h \
    instruction.h \
    neandermachine.h

FORMS    += hidragui.ui
