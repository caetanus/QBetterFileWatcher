#-------------------------------------------------
#
# Project created by QtCreator 2013-07-27T17:25:38
#
#-------------------------------------------------

QT       -= gui

UNAME = $$system(uname -a)
!win32:!macx{
 contains(UNAME, Linux): CONFIG += linux
}

TARGET = QBetterFileWatcher
TEMPLATE = lib
CONFIG += staticlib

DEPENDPATH += src

linux {
    DEPENDPATH += src/linux
    SOURCES += src/linux/inotifyfilewatcher.cpp
    HEADERS += src/linux/inotifyfilewatcher.h
}

SOURCES += src/qbetterfilewatcher.cpp


HEADERS += src/qbetterfilewatcher.h

INCLUDEPATH += src

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
