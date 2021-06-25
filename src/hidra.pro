#-------------------------------------------------
#
# Project created by QtCreator 2014-07-18T17:26:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hidra
TEMPLATE = app

CONFIG  += \
    c++11

SOURCES += \
    core/baseconversor.cpp \
    core/invalidconversorinput.cpp \
    core/pointconversor.cpp \
    gui/baseconversordialog.cpp \
    gui/flagwidget.cpp \
    gui/hidracodeeditor.cpp \
    gui/hidragui.cpp \
    gui/hidrahighlighter.cpp \
    gui/pointconversordialog.cpp \
    gui/registerwidget.cpp \
    core/byte.cpp \
    core/flag.cpp \
    core/instruction.cpp \
    core/machine.cpp \
    core/main.cpp \
    core/register.cpp \
    machines/ahmesmachine.cpp \
    machines/neandermachine.cpp \
    machines/ramsesmachine.cpp \
    core/addressingmode.cpp \
    machines/cromagmachine.cpp \
    machines/queopsmachine.cpp \
    machines/pitagorasmachine.cpp \
    machines/voltamachine.cpp \
    machines/regmachine.cpp \
    machines/periclesmachine.cpp \
    gui/about.cpp

HEADERS  += \
    core/baseconversor.h \
    core/invalidconversorinput.h \
    core/pointconversor.h \
    gui/baseconversordialog.h \
    gui/flagwidget.h \
    gui/hidracodeeditor.h \
    gui/hidragui.h \
    gui/hidrahighlighter.h \
    gui/pointconversordialog.h \
    gui/registerwidget.h \
    core/byte.h \
    core/flag.h \
    core/instruction.h \
    core/machine.h \
    core/register.h \
    machines/ahmesmachine.h \
    machines/neandermachine.h \
    machines/ramsesmachine.h \
    core/addressingmode.h \
    machines/cromagmachine.h \
    machines/queopsmachine.h \
    machines/pitagorasmachine.h \
    machines/voltamachine.h \
    machines/regmachine.h \
    machines/periclesmachine.h \
    gui/about.h

FORMS    += \
    gui/baseconversordialog.ui \
    gui/flagwidget.ui \
    gui/hidragui.ui \
    gui/pointconversordialog.ui \
    gui/registerwidget.ui \
    gui/about.ui

RESOURCES +=

OTHER_FILES += \
    to-do.txt
