#pragma once

#include <linux/inotify.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

extern "C" {
extern int inotify_init (void);
/* Create and initialize inotify instance.  */
extern int inotify_init1 (int __flags);
/* Add watch of object NAME to inotify instance FD.  Notify about
   events specified by MASK.  */
extern int inotify_add_watch (int __fd, const char *__name, uint32_t __mask);
/* Remove the watch specified by WD from the inotify instance FD.  */
extern int inotify_rm_watch (int __fd, int __wd);
}

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

#define CHILD_WATCH IN_ATTRIB | IN_CLOSE_WRITE | IN_CREATE | IN_DELETE |\
                    IN_MODIFY | IN_MOVE

#define SELF_WATCH IN_DELETE_SELF | IN_MOVE_SELF | IN_DELETE_SELF

#define CHILD_DIRECTORY_WATCH CHILD_WATCH | IN_ONLYDIR
#define ROOT_DIRECTORY_WATCH CHILD_DIRECTORY_WATCH | SELF_WATCH


#define EVENT_SIZE (sizeof (struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

typedef struct {
    int mask;
    char* name;
} InotifyEvents_t;




class InotifyFileWatcher : public QObject
{
    Q_OBJECT
    char m_buffer[EVENT_BUF_LEN];
    int m_fd;
    QSocketNotifier* m_notifier;
    QMap<QString, int> m_directoryHandles;
    QMap<QString, int> m_fileHandles;
    void fetchSubDirectories(QString path);
public:
    explicit InotifyFileWatcher();
    bool start();
    bool stop();
    bool watchDirectory(QString path, bool child=false);
    bool unwatchDirectory(QString path);
    QList<QString> directoriesWatching();
    bool isWatchingDirectory(QString path);
    bool watchFile(QString path);
    bool unwatchFile(QString path);
    QList<QString> filesWatching();
    bool isWatchingFile(QString path);
    int getHandle();

    ~InotifyFileWatcher();
    
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

