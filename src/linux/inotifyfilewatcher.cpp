#include "inotifyfilewatcher.h"
//this is just for debuging. we'll remove this as soon as possible.
InotifyEvents_t strEvents[] =
 {
   {IN_ACCESS         , "IN_ACCESS        "},
   {IN_ATTRIB         , "IN_ATTRIB        "},
   {IN_CLOSE_WRITE    , "IN_CLOSE_WRITE   "},
   {IN_CLOSE_NOWRITE  , "IN_CLOSE_NOWRITE "},
   {IN_CREATE         , "IN_CREATE        "},
   {IN_DELETE         , "IN_DELETE        "},
   {IN_DELETE_SELF    , "IN_DELETE_SELF   "},
   {IN_MODIFY         , "IN_MODIFY        "},
   {IN_MOVE_SELF      , "IN_MOVE_SELF     "},
   {IN_MOVED_FROM     , "IN_MOVED_FROM    "},
   {IN_MOVED_TO       , "IN_MOVED_TO      "},
   {IN_OPEN           , "IN_OPEN          "},
 };

InotifyFileWatcher::InotifyFileWatcher() :
    m_notifier(NULL)
{
    inotify_init();
    if (m_fd < 0)
    {
        perror("inotify_init");
        abort();
    }
}

void InotifyFileWatcher::fetchSubDirectories(QString path)
{
    QDir dpath(QDir(path).absolutePath());

    foreach(QString dir, dpath.entryList(QDir::Dirs | QDir::Hidden |
                                         QDir::System | QDir::NoDotAndDotDot))
    {
        watchDirectory(path + QDir::separator() + dir, true);
    }
}

bool InotifyFileWatcher::watchDirectory(QString path, bool child)
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
        res = inotify_add_watch(m_fd, path.toStdString().c_str(), ROOT_DIRECTORY_WATCH);

    if (res == -1){
        perror("watch directory error.");
        qDebug() << errno << strerror(errno);
        return false;
    }

    m_directoryHandles[path] = res;
    fetchSubDirectories(path);
    return true;
}

bool InotifyFileWatcher::isWatchingDirectory(QString path)
{
    path = QDir(path).absolutePath();
    return m_directoryHandles.contains(path);
}

QList<QString> InotifyFileWatcher::directoriesWatching()
{
    return m_directoryHandles.keys();
}

bool InotifyFileWatcher::unwatchDirectory(QString path)
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


bool InotifyFileWatcher::watchFile(QString path)
{
    path = QDir(path).absolutePath();
    if(!QFileInfo(path).isFile()){
        return false;
    }

    int res =  inotify_add_watch(m_fd,
                                 path.toStdString().c_str(),
                                 CHILD_WATCH | SELF_WATCH);
    if (res == -1){
        perror("watch file error.");
        qDebug() << errno << strerror(errno);
        return false;
    }

    m_fileHandles[path] = res;
    return true;
}


bool InotifyFileWatcher::unwatchFile(QString path)
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

bool InotifyFileWatcher::isWatchingFile(QString path)
{
    path = QDir(path).absolutePath();
    return m_fileHandles.contains(path);
}

QList<QString> InotifyFileWatcher::filesWatching()
{
    return m_fileHandles.keys();
}


bool InotifyFileWatcher::start()
{
    if (m_notifier)
        return false;
    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read);
    connect(m_notifier, SIGNAL(activated(int)), this, SLOT(eventCallback()));
    return true;
}

bool InotifyFileWatcher::stop()
{
    if (!m_notifier)
        return false;
    disconnect(m_notifier, SIGNAL(activated(int)), this, SLOT(eventCallback()));
    delete m_notifier;
    m_notifier = 0;
    return true;
}


int InotifyFileWatcher::getHandle()
{
    return m_fd;
}
void InotifyFileWatcher::eventCallback()
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
                    watchDirectory(event->name, true);
                    emit directoryCreated(QString(event->name));
                } else {
                    emit fileCreated(QString(event->name));
                }

            }
            else if ((event->mask & IN_MODIFY) || (event->mask & IN_MOVE_SELF) || (event->mask & IN_ATTRIB))
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
                    if (m_directoryHandles.contains(QString(event->name)))
                        unwatchDirectory(event->name);
                    emit directoryDeleted(QString(event->name));
                } else {
                    emit fileDeleted(QString(event->name));
                }


            }
            for (unsigned int i = 0; i < sizeof(strEvents); i++)
            {
                if (strEvents[i].mask & event->mask)
                {
                    qDebug() << strEvents[i].name << event->name << event->wd;
                }
            }
        }
        i += EVENT_SIZE + event->len;
    }

}
InotifyFileWatcher::~InotifyFileWatcher()
{
    stop();
    close(m_fd);
}
