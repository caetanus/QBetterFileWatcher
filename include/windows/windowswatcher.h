#pragma once

#include <windows.h>
#include <QMap>
#include <QSocketNotifier>
#include <private/qwineventnotifier_p.h>


#include <QObject>
#include <QDebug>
#include <QSet>
#include <QDir>
#include <QQueue>

#include "AbstractWatcher.h"

#define SELF_WATCH FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE |\
                   FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |\
                   FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE |\
                   FILE_NOTIFY_CHANGE_SECURITY

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
    OVERLAPPED m_overlapped;
    char m_result[RESULT_SIZE];
    QWinEventNotifier* m_notifier;
    HANDLE m_fd;
    QString m_watchingPath;
    QSet<QString> m_subdirs;
    QQueue<QString> m_moveQueue;

    void asyncQueryDirectoryChanges();
    void enumSubDirectories(QString path);


public:
    explicit WindowsWatcher();
    bool watchDirectory(QString path);
    bool unwatchDirectory(QString path);
    int getHandle();

    void start();
    void stop();
    ~WindowsWatcher();

private slots:
    void eventCallback();

public slots:

};

