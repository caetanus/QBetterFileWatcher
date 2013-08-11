#-------------------------------------------------
#
# Project created by QtCreator 2013-08-04T12:07:26
#
#-------------------------------------------------

include(../../QBetterFileWatcher.prf)

CONFIG += shadow_build

shadow_build {
    OBJECTS_DIR = $$PWD/../../shadow_build
    MOC_DIR = $$PWD/../../shadow_build
    UIC_DIR = $$PWD/../../shadow_build
    RCC_DIR = $$PWD/../../shadow_build
    UI_DIR = $$PWD/../../shadow_build

}

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
