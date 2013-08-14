#-------------------------------------------------
#
# Project created by QtCreator 2013-07-27T17:25:38
#
#-------------------------------------------------

CONFIG += shadow_build

develop {
    DEFINES += DEBUG_INFORMATION
}
QT       -= gui

shadow_build {
    SHADOW_PATH = $$PWD/shadow_build/
    DEPENDPATH += $$SHADOW_PATH
    INCLUDEPATH += $$SHADOW_PATH
    OBJECTS_DIR = $$SHADOW_PATH
    MOC_DIR = $$SHADOW_PATH
    RCC_DIR = $$SHADOW_PATH
    UI_DIR = $$SHADOW_PATH

}

!win32:!macx{
 UNAME = $$system(uname -a)
 contains(UNAME, Linux): CONFIG += linux
}

TARGET = ./lib/QBetterFileWatcher
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

win32 {
    INCLUDEPATH += include/windows
    SOURCES += src/windows/windowswatcher.cpp
    HEADERS += include/windows/windowswatcher.h
}


SOURCES += src/qbetterfilewatcher.cpp \
    src/AbstractWatcher.cpp
HEADERS += include/qbetterfilewatcher.h \
    include/AbstractWatcher.h


INCLUDEPATH += src

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/local/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    QBetterFileWatcher.prf
