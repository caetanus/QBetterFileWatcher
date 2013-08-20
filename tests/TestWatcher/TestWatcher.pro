#-------------------------------------------------
#
# Project created by QtCreator 2013-08-04T12:07:26
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


TARGET = $$PWD/../../builds/TestWatcher
win32: TARGET = ./../../../TestWatcher
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    testdeletefiles.cpp \
    testbase.cpp \
    testcreatefile.cpp \
    testcreatedirectory.cpp\
    testdeletedirectory.cpp\
    testcreatesubdirfile.cpp\
    testdeletesubdirfiles.cpp\
    testcreatesubdirs.cpp\
    testmovefile.cpp\
    testmoveintodirectory.cpp
    #testmovefileoutside.cpp\
    #testmovefileinse.cpp\
    #testmovedirectory.cpp\
    #testmovedirectoryoutside.cpp\
    #testmovedirectoryinside.cpp

HEADERS += \
    testdeletefiles.h \
    testcreatefile.h \
    testcreatedirectory.h\
    testdeletedirectory.h\
    testcreatesubdirfile.h\
    testdeletesubdirfiles.h\
    testcreatesubdirs.h\
    testmovefile.h\
    testmoveintodirectory.h\
    #testmovefileoutside.h\
    #testmovefileinse.h\
    #testmovedirectory.h\
    #testmovedirectoryoutside.h\
    #testmovedirectoryinside.h\
    testbase.h
