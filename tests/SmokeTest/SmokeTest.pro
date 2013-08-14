#-------------------------------------------------
#
# Project created by QtCreator 2013-08-05T20:27:00
#
#-------------------------------------------------

include(../../QBetterFileWatcher.prf)

CONFIG += shadow_build

QT       += core

QT       -= gui

shadow_build {
    SHADOW_PATH = $$PWD/../../shadow_build/
    DEPENDPATH += $$SHADOW_PATH
    INCLUDEPATH += $$SHADOW_PATH
    OBJECTS_DIR = $$SHADOW_PATH
    MOC_DIR = $$SHADOW_PATH
    RCC_DIR = $$SHADOW_PATH
    UI_DIR = $$SHADOW_PATH

}

TARGET = $$PWD/../../SmokeTest
win32: TARGET = ./../../../SmokeTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp
