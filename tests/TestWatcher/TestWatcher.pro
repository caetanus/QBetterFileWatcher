#-------------------------------------------------
#
# Project created by QtCreator 2013-08-04T12:07:26
#
#-------------------------------------------------

include(../../QBetterFileWatcher.prf)

QT       += core
#QT       -= gui

TARGET = $$PWD/../../builds/TestWatcher
#CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    testdeletefiles.cpp \
    testbase.cpp \
    testcreatefile.cpp \
    testcreatedirectory.cpp\
    testdeletedirectory.cpp\
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
    testmovefile.h\
    testmoveintodirectory.h\
    #testmovefileoutside.h\
    #testmovefileinse.h\
    #testmovedirectory.h\
    #testmovedirectoryoutside.h\
    #testmovedirectoryinside.h\
    testbase.h
