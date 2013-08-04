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
    TestCreateFile.cpp \
    TestBase.cpp

HEADERS += \
    TestCreateFile.h \
    TestBase.h
