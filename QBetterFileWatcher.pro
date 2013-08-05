#-------------------------------------------------
#
# Project created by QtCreator 2013-07-27T17:25:38
#
#-------------------------------------------------

CONFIG += shadow_build

QT       -= gui

shadow_build {
    OBJECTS_DIR = $$PWD/shadow_build
    MOC_DIR = $$PWD/shadow_build
    UIC_DIR = $$PWD/shadow_build
    RCC_DIR = $$PWD/shadow_build
    UI_DIR = $$PWD/shadow_build

}

!win32:!macx{
 UNAME = $$system(uname -a)
 contains(UNAME, Linux): CONFIG += linux
}

TARGET = lib/QBetterFileWatcher
TEMPLATE = lib
CONFIG += staticlib

DEPENDPATH += src include
INCLUDEPATH += include


linux {
    INCLUDEPATH += include/linux
    DEPENDPATH += src/linux include/linux
    SOURCES += src/linux/inotifyfilewatcher.cpp
    HEADERS += include/linux/inotifyfilewatcher.h
}

SOURCES += src/qbetterfilewatcher.cpp


HEADERS += include/qbetterfilewatcher.h

INCLUDEPATH += src

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    QBetterFileWatcher.prf
