#include "inotifyfilewatcher.h"
//this is just for debuging. we'll remove this as soon as possible.

QMap<int, QString> EventUtilsInotify::m_evs = QMap<int, QString>();
bool EventUtilsInotify::m_setup = false;

INotifyFileWatcher::INotifyFileWatcher() :
    m_started(false), m_notifier(NULL)
{
    m_fd = inotify_init();
    if (m_fd < 0)
    {
        perror("inotify_init");
        abort();
    }
    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read);
    m_notifier->setEnabled(false);

}

void INotifyFileWatcher::fetchSubDirectories(QString path)
{
    QDir dpath(QDir(path).absolutePath());

    foreach(QString dir, dpath.entryList(QDir::Dirs | QDir::Hidden |
                                         QDir::System | QDir::NoDotAndDotDot))
    {
#ifdef DEBUG_INFORMATION
        emit debugInformation(QStringList() << "watching subdirectories" << path + QDir::separator() + dir);
#endif
        watchDirectory(path + QDir::separator() + dir, true);
    }
}

bool INotifyFileWatcher::watchDirectory(QString path)
{
    return watchDirectory(path, false);
}

bool INotifyFileWatcher::watchDirectory(QString path, bool child)
{
    path = QDir(path).absolutePath();
    int res;
    if(!QFileInfo(path).isDir())
    {
        return false;
    }
    if (child)
        res = inotify_add_watch(m_fd, path.toStdString().c_str(), CHILD_DIRECTORY_WATCH);
    else
        res = inotify_add_watch(m_fd, path.toStdString().c_str(), IN_ALL_EVENTS);

    if (res == -1){
        perror("watch directory error");
        qDebug() << errno << strerror(errno);
        return false;
    }

    m_handlesDirectory[res] = path;
    m_directoryHandles[path] = res;
    fetchSubDirectories(path);
    return true;
}

bool INotifyFileWatcher::isWatchingDirectory(QString path)
{
    path = QDir(path).absolutePath();
    return m_directoryHandles.contains(path);
}

QList<QString> INotifyFileWatcher::directoriesWatching()
{
    return m_directoryHandles.keys();
}

bool INotifyFileWatcher::unwatchDirectory(QString path)
{
    path = QDir(path).absolutePath();
    if (!m_directoryHandles.contains(path))
    {
        return false;
    }
    inotify_rm_watch(m_fd, m_directoryHandles[path]);
    m_handlesDirectory.remove(m_directoryHandles[path]);
    m_directoryHandles.remove(path);
    return true;
}

void INotifyFileWatcher::start()
{
    m_started = true;
    connect(m_notifier, SIGNAL(activated(int)), this, SLOT(eventCallback()),  Qt::QueuedConnection);
    m_notifier->setEnabled(true);
}

void INotifyFileWatcher::stop()
{
    m_started = false;
    m_notifier->setEnabled(false);
    disconnect(this, SLOT(eventCallback()));
}



void INotifyFileWatcher::eventCallback()
{
    int lenght = read(m_fd, m_buffer, EVENT_BUF_LEN);
    if ( lenght < 0)
    {
        perror("read");
    }
    int i  = 0;
    if (!m_started)
        return;
    while (i < lenght)
    {
        struct inotify_event *event = ( struct inotify_event *)&m_buffer[i];
        if (event->len){
            //create event
            if (event->mask & IN_CREATE)
            {
                if (event->mask & IN_ISDIR)
                {
                    debug(QStringList(QStringList() << "watchDirectory" << getEventFileName(event)));
                    watchDirectory(getEventFileName(event), true);
                    debug(QStringList(QStringList() << "directoryCreated" << getEventFileName(event)));
                    emit directoryCreated(getEventFileName(event));
                } else {
                    debug(QStringList(QStringList() << "fileCreated" << getEventFileName(event)));
                    emit fileCreated(getEventFileName(event));
                }

            }
            else if ((event->mask & IN_MODIFY) ||
                     (event->mask & IN_CLOSE_WRITE) ||
                     (event->mask & IN_ATTRIB))

            {
                if (event->mask & IN_ISDIR)
                {
                    debug(QStringList(QStringList() << "directoryChanged" << getEventFileName(event)));
                    emit directoryChanged(getEventFileName(event));
                } else {
                    debug(QStringList(QStringList() << "fileChanged" << getEventFileName(event)));
                    emit fileUpdated(getEventFileName(event));
                }

            }
            else if (event->mask & IN_MOVED_FROM)
            {
                QPair<FSObjectType, QString> moveQueueItem;
                if (event->mask & IN_ISDIR)
                {
                    moveQueueItem.first = Directory;
                }
                else moveQueueItem.first = File;
                moveQueueItem.second = getEventFileName(event);
                m_moveEventQueue.append(moveQueueItem);
                onMovedFromEvent(); // starts a timer to checks if this was moved out of directory
            }
            else if (event->mask & IN_MOVED_TO)
            {
                //TODO: fix the queue to get a way to check if it is a dir or not.
                //nowadays we just unstack
                if (!m_moveEventQueue.length())
                {
                    //There is not in the queue, so we'll emit a create event.
                    if (event->mask & IN_ISDIR)
                    {
                        QString eventName = getEventFileName(event);
                        watchDirectory(eventName);
                        debug(QStringList(QStringList() << "directoryCreated(byMove)" << getEventFileName(event)));
                        emit directoryCreated(eventName);
                    }
                    else
                    {
                        debug(QStringList(QStringList() << "fileCreated(byMove)" << getEventFileName(event)));
                        emit fileCreated(getEventFileName(event));
                    }

                }
                else
                {
                    //lets cancel the last move timer.
                    deQueueMovedTimer();
                    QPair<FSObjectType, QString> moveQueueItem = m_moveEventQueue.takeFirst();
                    if (event->mask & IN_ISDIR)
                    {
                        debug(QStringList(QStringList() << "directoryMoved" << moveQueueItem.second << getEventFileName(event)));
                        int handle = m_directoryHandles[moveQueueItem.second];
                        m_directoryHandles.remove(moveQueueItem.second);
                        m_directoryHandles[getEventFileName(event)] = handle;
                        m_handlesDirectory[handle] = getEventFileName(event);
                        emit directoryMoved(moveQueueItem.second, getEventFileName(event));
                    }
                    else
                    {
                        debug(QStringList(QStringList() << "fileMoved" << moveQueueItem.second << getEventFileName(event)));
                        emit fileMoved(moveQueueItem.second, getEventFileName(event));
                    }
                }
            }
            else if ((event->mask & IN_DELETE) || (event->mask & IN_DELETE_SELF))
            {
                if (event->mask & IN_ISDIR)
                {
                    if (m_directoryHandles.contains(getEventFileName(event)))
                        unwatchDirectory(getEventFileName(event));
                    debug(QStringList(QStringList() << "directoryDeleted" << getEventFileName(event)));
                    emit directoryDeleted(getEventFileName(event));
                } else {
                    debug(QStringList(QStringList() << "fileDeleted" << getEventFileName(event)));
                    emit fileDeleted(getEventFileName(event));
                }


            }
#ifdef DEBUG_INFORMATION
            emit debugInformation(EventUtilsInotify::translateEvent(event) << getEventFileName(event));
#endif
        }
        i += EVENT_SIZE + event->len;
    }

}

void INotifyFileWatcher::onMovedFromEvent()
{
    QTimer* timeout = new QTimer();
    connect(timeout, SIGNAL(timeout()), this, SLOT(unStackerMoves()));
    timeout->setInterval(5); //checks event on 5ms if has no moved to event in this time, acts as a delete.
    timeout->setSingleShot(true);
    m_moveEventDequeuer.append(timeout);
    timeout->start();
}

bool INotifyFileWatcher::deQueueMovedTimer()
{
    if (!m_moveEventDequeuer.length())
        return false;
    QTimer* timeout = m_moveEventDequeuer.takeFirst();
    timeout->stop();
    disconnect(this, SLOT(unStackerMoves()));
    delete timeout;
    return true;
}

void INotifyFileWatcher::unStackerMoves()
{
    if (m_moveEventQueue.length())
    {
        QPair<FSObjectType, QString> event = m_moveEventQueue.takeFirst();
        if(event.first == File)
        {
            debug(QStringList(QStringList() << "fileDeleted(by move)" << event.second));
            emit fileDeleted(event.second);
        }
        else
        {
            debug(QStringList(QStringList() << "directoryDeleted(by move)" << event.second));
            unwatchDirectory(event.second);
            emit directoryDeleted(event.second);
        }
    }
    deQueueMovedTimer();
}

INotifyFileWatcher::~INotifyFileWatcher()
{
    while(deQueueMovedTimer());

    m_directoryHandles.clear();
    m_handlesDirectory.clear();
    stop();
    close(m_fd);
    //delete m_notifier;
}
