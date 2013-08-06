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
    OBJECTS_DIR = $$PWD/../../shadow_build
    MOC_DIR = $$PWD/../../shadow_build
    RCC_DIR = $$PWD/../../shadow_build
    UI_DIR = $$PWD/../../shadow_build

}

TARGET = $$PWD/../../SmokeTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp
