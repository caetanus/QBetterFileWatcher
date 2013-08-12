#pragma once

#include <windows.h>
#include <QMap>
#include <QSocketNotifier>


#include <QObject>

#define SELF_WATCH FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE |\
                   FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |\
                   FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE |\
                   FILE_NOTIFY_CHANGE_SECURITY

static const int RESULT_SIZE = 8192;

struct MyOVERLAPPED {
    LPVOID Internal;
    LPVOID InternalHigh;
    DWORD Offset;
    DWORD OffsetHigh;
    LPVOID Pointer;
    HANDLE hEvent;
};

class WindowsWatcher : public QObject
{
    enum RecursiveWatch {
        WatchNotRecursive = false,
        WatchRecursive = true
    };

    enum FSObjectType{
        File,
        Directory
    };
    MyOVERLAPPED m_overlapped;
    char m_result[RESULT_SIZE];
    QSocketNotifier* m_notifier;
    HANDLE m_fd;

    void asyncQueryDirectoryChanges();


    Q_OBJECT
public:
    explicit WindowsWatcher(QObject *parent = 0);
    bool watchDirectory(QString path);
    void start();
    
signals:

private slots:
    void eventCallback();
    
public slots:
    
};

#endif // WINDOWSWATCHER_H
