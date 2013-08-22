#-------------------------------------------------
#
# Project created by QtCreator 2013-08-21T16:13:39
#
#-------------------------------------------------

QT       += core gui

include (../../QBetterFileWatcher.prf)

include(../../vendor/PythonQt/build/PythonQt.prf)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ScriptTest
TEMPLATE = app


SOURCES += main.cpp\
        pythoncontext.cpp\
        PythonQtScriptingConsole.cpp


HEADERS  += pythoncontext.h\
            PythonQtScriptingConsole.h

OTHER_FILES += \
    TestBase.py \
    test_file_watcher.py

RESOURCES += \
    script.qrc
