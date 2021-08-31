QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../core

SOURCES +=  tst_pointconversortest.cpp ../../core/pointconversor.cpp ../../core/invalidconversorinput.cpp
