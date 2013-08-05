#-------------------------------------------------
#
# Project created by QtCreator 2013-08-04T12:07:26
#
#-------------------------------------------------

include(../../QBetterFileWatcher.prf)

QT       += core
QT       -= gui

TARGET = $$PWD/../../builds/TestWatcher
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    testdeletefiles.cpp \
    testbase.cpp \
    testcreatefile.cpp \
    testcreatedirectory.cpp\
    testdeletedirectory.cpp

HEADERS += \
    testdeletefiles.h \
    testcreatefile.h \
    testcreatedirectory.h\
    testdeletedirectory.h\
    testbase.h
