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
    register.cpp \
    byte.cpp \
    instruction.cpp \
    ramsesmachine.cpp \
    neandermachine.cpp \
    ahmesmachine.cpp \
    hidracli.cpp \
    hidrahighlighter.cpp \
    hidracodeeditor.cpp \
    flag.cpp \
    registerwidget.cpp \
    flagwidget.cpp

HEADERS  += hidragui.h \
    machine.h \
    register.h \
    byte.h \
    instruction.h \
    ramsesmachine.h \
    neandermachine.h \
    ahmesmachine.h \
    hidracli.h \
    hidrahighlighter.h \
    hidracodeeditor.h \
    flag.h \
    registerwidget.h \
    flagwidget.h

FORMS    += hidragui.ui \
    registerwidget.ui \
    flagwidget.ui

RESOURCES += \
    res.qrc

OTHER_FILES += \
    Changed.txt \
    Notes.txt
