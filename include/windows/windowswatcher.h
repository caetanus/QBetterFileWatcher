#pragma once

#include <windows.h>
#include <QMap>
#include <QSocketNotifier>
#include <private/qwineventnotifier_p.h>


#include <QObject>
#include <QDebug>
#include <QSet>
#include <QDir>
#include <QMap>
#include <QQueue>

#include "AbstractWatcher.h"

#define SELF_WATCH FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |\
                   FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |\
                   FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY


static const int RESULT_SIZE = 8192;


class WindowsWatcher : public AbstractWatcher
{
    Q_OBJECT

    enum RecursiveWatch {
        WatchNotRecursive = false,
        WatchRecursive = true
    };

    enum FSObjectType{
        File,
        Directory
    };
    QMap<HANDLE, OVERLAPPED*> m_overlappeds;
    QMap<HANDLE, char*> m_results;
    QMap<HANDLE, QWinEventNotifier*> m_notifiers;
    QMap<QString, HANDLE> m_watchingPaths;
    QMap<HANDLE, QString> m_watchingPathsHandles;
    QMap<QString, QSet<QString> > m_subdirs;
    QMap<HANDLE, QQueue<QString> > m_moveQueue;
    QMap<HANDLE, HANDLE> m_fileHandle;

    void asyncQueryDirectoryChanges(HANDLE fd);
    void enumSubDirectories(QString path, QString basePath);
    void enumSubDirectories(QString path);


public:
    explicit WindowsWatcher();
    ~WindowsWatcher();

public slots:
    void start();
    void stop();
    bool watchDirectory(QString path);
    bool unwatchDirectory(QString path);

private slots:
    void eventCallback(HANDLE fd);

public slots:

};

