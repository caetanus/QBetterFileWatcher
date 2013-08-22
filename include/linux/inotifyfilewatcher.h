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
#include <QStack>
#include <QQueue>
#include <QObject>
#include <QList>
#include <QMap>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTimer>

#include "AbstractWatcher.h"

#define CHILD_WATCH IN_ATTRIB | IN_CLOSE_WRITE | IN_CREATE | IN_DELETE |\
                    IN_MODIFY | IN_MOVE

#define SELF_WATCH IN_DELETE_SELF | IN_MOVE_SELF | IN_DELETE_SELF

#define CHILD_DIRECTORY_WATCH CHILD_WATCH | IN_ONLYDIR
#define ROOT_DIRECTORY_WATCH CHILD_DIRECTORY_WATCH | SELF_WATCH


#define EVENT_SIZE (sizeof (struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

class EventUtilsInotify{
    static bool m_setup;
    static QMap<int, QString> m_evs;
    static inline void setup()
    {
        if (m_setup)
            return;
        m_evs[IN_ACCESS] = "IN_ACCESS";
        m_evs[IN_ATTRIB] = "IN_ATTRIB";
        m_evs[IN_CLOSE_WRITE] = "IN_CLOSE_WRITE";
        m_evs[IN_CLOSE_NOWRITE] = "IN_CLOSE_NOWRITE";
        m_evs[IN_CREATE] = "IN_CREATE";
        m_evs[IN_DELETE] = "IN_DELETE";
        m_evs[IN_DELETE_SELF] = "IN_DELETE_SELF";
        m_evs[IN_MODIFY] = "IN_MODIFY";
        m_evs[IN_MOVE_SELF] = "IN_MOVE_SELF";
        m_evs[IN_MOVED_FROM] = "IN_MOVED_FROM";
        m_evs[IN_MOVED_TO] = "IN_MOVED_TO";
        m_evs[IN_OPEN] = "IN_OPEN";
        m_evs[IN_CLOEXEC] = "IN_CLOEXEC";
        m_evs[IN_CLOSE] = "IN_CLOSE";
        m_evs[IN_DONT_FOLLOW] = "IN_DONT_FOLLOW";
        m_evs[IN_EXCL_UNLINK] = "IN_EXCL_UNLINK";
        m_evs[IN_IGNORED] = "IN_IGNORED";
        m_evs[IN_ISDIR] = "IN_ISDIR";
        m_evs[IN_MASK_ADD] = "IN_MASK_ADD";
        m_evs[IN_UNMOUNT] = "IN_UNMOUNT";
        m_evs[IN_Q_OVERFLOW] = "IN_Q_OVERFLOW";
        m_setup = true;
    }

public:
    static inline QStringList translateEvent(inotify_event* ev)
    {
        QStringList ret;
        setup();
        for (int i = 0; i < 64; i++)
        {
            int evid = 1 << i;
            if (ev->mask & evid && m_evs.contains(evid))
                ret << m_evs[evid];
            else if(ev->mask & evid)
                ret << "Unknown Event";
        }
        return ret;
    }

};




class INotifyFileWatcher : public AbstractWatcher
{
    enum FSObjectType{
        File,
        Directory
    };
    Q_OBJECT
    char m_buffer[EVENT_BUF_LEN];
    int m_fd;
    bool m_started;
    QQueue< QPair<FSObjectType,QString> > m_moveEventQueue;
    QQueue<QTimer*> m_moveEventDequeuer;

    QSocketNotifier *m_notifier;
    QMap<int, QString> m_handlesDirectory;
    QMap<QString, int> m_directoryHandles;
    void fetchSubDirectories(QString path);
    void onMovedFromEvent();
    bool deQueueMovedTimer();
    inline QString getEventFileName(inotify_event* e)
    {
        return m_handlesDirectory[e->wd] + QDir::separator() + QString(e->name);
    }
    bool watchDirectory(QString path, bool child);
public:
    explicit INotifyFileWatcher();
    QList<QString> directoriesWatching();
    bool isWatchingDirectory(QString path);
    ~INotifyFileWatcher();

public slots:
    void start();
    void stop();
    bool watchDirectory(QString path);
    bool unwatchDirectory(QString path);

private slots:
    void eventCallback();
    void unStackerMoves();
};

