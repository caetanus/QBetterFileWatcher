#include "LinuxFileWatcher.h"

LinuxFileWatcher::LinuxFileWatcher(QObject *parent) :
    QObject(parent), m_notifier(NULL)
{
    inotify_init();
    if (m_fd < 0)
    {
        perror("inotify_init");
        abort();
    }
}

bool LinuxFileWatcher::watchDirectory(QString path)
{
    path = QDir(path).absolutePath();
    if(!QFileInfo(QDir(path)).isDir())
    {
        return false;
    }
    int res = inotify_add_watch(m_fd,
                                path.toStdString().c_str(), GENERIC_DIRECTORY_WATCH);
    if (res == -1){
        perror("watch directory error.");
        qDebug() << errno << strerror(errno);
        return false;
    }

    m_directoryHandles[path] = res;
    return true;
}

bool LinuxFileWatcher::isWatchingDirectory(QString path)
{
    path = QDir(path).absolutePath();
    return m_directoryHandles.contains(path);
}

QList<QString> LinuxFileWatcher::directoriesWatching()
{
    return m_directoryHandles.keys();
}

bool LinuxFileWatcher::unwatchDirectory(QString path)
{
    path = QDir(path).absolutePath();
    if (!m_directoryHandles.contains(path))
    {
        return false;
    }
    inotify_rm_watch(m_fd, m_directoryHandles[path]);
    m_directoryHandles.remove(path);
    return true;
}


bool LinuxFileWatcher::watchFile(QString path)
{
    path = QDir(path).absolutePath();
    if(!QFileInfo(QDir(path)).isFile()){
        return false;
    }

    int res =  inotify_add_watch(m_fd,
                                 path.toStdString().c_str(),
                                 GENERIC_FILE_WATCH);
    if (res == -1){
        perror("watch file error.");
        qDebug() << errno << strerror(errno);
        return false;
    }

    m_fileHandles[path] = res;
    return true;
}


bool LinuxFileWatcher::unwatchFile(QString path)
{
    path = QDir(path).absolutePath();
    if (!m_fileHandles.contains(path))
    {
        return false;
    }
    inotify_rm_watch(m_fd, m_fileHandles[path]);
    m_fileHandles.remove(path);
    return true;
}

bool LinuxFileWatcher::isWatchingFile(QString path)
{
    path = QDir(path).absolutePath();
    return m_fileHandles.contains(path);
}

QList<QString> LinuxFileWatcher::filesWatching()
{
    return m_fileHandles.keys();
}


bool LinuxFileWatcher::start()
{
    if (m_notifier)
        return false;
    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read);
    connect(m_notifier, SIGNAL(activated(int)), this, SLOT(eventCallback()));
}

bool LinuxFileWatcher::stop()
{
    if (!m_notifier)
        return false;
    disconnect(m_notifier, SIGNAL(activated(int)), this, SLOT(eventCallback()));
    delete m_notifier;
    m_notifier = 0;
}


int LinuxFileWatcher::getHandle()
{
    return m_fd;
}
void LinuxFileWatcher::eventCallback()
{
    int lenght = read(m_fd, m_buffer, EVENT_BUF_LEN);
    if ( lenght < 0)
    {
        perror("read");
    }
    int i  = 0;
    while (i < lenght)
    {
        struct inotify_event *event = ( struct inotify_event *)&m_buffer[i];
        if (event->len){
            //create event
            if (event->mask & IN_CREATE)
            {
                if (event->mask & IN_ISDIR)
                {
                    emit directoryCreated(QString(event->name));
                } else {
                    emit fileCreated(QString(event->name));
                }

            }
            else if ((event->mask & IN_MODIFY) || (event->mask & IN_ATTRIB))
            {
                if (event->mask & IN_ISDIR)
                {
                    emit directoryChanged(QString(event->name));
                } else {
                    emit fileUpdated(QString(event->name));
                }

            }
            else if ((event->mask & IN_DELETE) || (event->mask && IN_DELETE_SELF))
            {
                if (event->mask & IN_ISDIR)
                {
                    emit directoryDeleted(QString(event->name));
                } else {
                    emit fileDeleted(QString(event->name));
                }


            }
        }
        i += EVENT_SIZE + event->len;
    }

}
LinuxFileWatcher::~LinuxFileWatcher()
{
    stop();
    close(m_fd);
}
