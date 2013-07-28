#pragma once

#include <linux/inotify.h>
#include <sys/types.h>
#include <errno.h>
#include <QDebug>
#include <QSocketNotifier>

#include <QObject>
#include <QList>
#include <QMap>
#include <QDir>
#include <QFileInfo>
#include <QFile>

#define GENERIC_FILE_WATCH  IN_ATTRIB | IN_CLOSE_WRITE | IN_CREATE | IN_DELETE |
                            IN_DELETE_SELF | IN_MODIFY | IN_MOVE_SELF |
                            IN_MOVED_FROM | IN_MOVED_TO

#define GENERIC_DIRECTORY_WATCH IN_ONLYDIR | GENERIC_FILE_WATCH


#define EVENT_SIZE (sizeof (struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))




class LinuxFileWatcher : public QObject
{
    Q_OBJECT
    char m_buffer[EVENT_BUF_LEN];
    int m_fd;
    QSocketNotifier* m_notifier;
    QMap<QString, int> m_directoryHandles;
    QMap<QString, int> m_fileHandles;
public:
    explicit LinuxFileWatcher(QObject *parent = 0);
    bool start();
    bool stop();
    bool watchDirectory(QString path);
    bool unwatchDirectory(QString path);
    QList<QString> directoriesWatching();
    bool isWatchingDirectory(QString path);
    bool watchFile(QString path);
    bool unwatchFile(QString path);
    QList<QString> filesWatching();
    bool isWatchingFile(QString path);
    int getHandle();

    ~LinuxFileWatcher();
    
signals:
    void directoryCreated(QString path);
    void directoryDeleted(QString path);
    void directoryChanged(QString path);
    void directoryMoved(QString oldPath, QString newPath);
    void fileCreated(QString path);
    void fileDeleted(QString path);
    void fileMoved(QString oldPath, QString newPath);
    void fileUpdated(QString path);
private slots:
    void eventCallback();
};

