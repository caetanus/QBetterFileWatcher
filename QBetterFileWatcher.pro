#-------------------------------------------------
#
# Project created by QtCreator 2013-07-27T17:25:38
#
#-------------------------------------------------

QT       -= gui

TARGET = QBetterFileWatcher
TEMPLATE = lib
CONFIG += staticlib

SOURCES += QBetterFileWatcher.cpp \
    LinuxFileWatcher.cpp

HEADERS += QBetterFileWatcher.h \
    LinuxFileWatcher.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
