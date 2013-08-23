#include "windowswatcher.h"

void WindowsWatcher::asyncQueryDirectoryChanges(HANDLE fd)
{
    OVERLAPPED* overlapped = new OVERLAPPED;
    overlapped->hEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    m_overlappeds[fd] = overlapped;
    memset(overlapped, 0, sizeof(OVERLAPPED));
    void* result = (void*)m_results[fd];
    HANDLE file_handle = m_fileHandle[fd];
    ::ReadDirectoryChangesW(file_handle, result, RESULT_SIZE, true,
                            SELF_WATCH, NULL, overlapped, NULL);
}

void WindowsWatcher::enumSubDirectories(QString path)
{
    enumSubDirectories(path, path);
}

void WindowsWatcher::enumSubDirectories(QString path, QString basePath)
{
    foreach(QString subDir, QDir(path).entryList(QDir::AllDirs |
                                                 QDir::NoDotAndDotDot |
                                                 QDir::Hidden))
    {
        QString fullPath = QDir::toNativeSeparators(path + QDir::separator() + subDir);
        m_subdirs[basePath].insert(fullPath);
        enumSubDirectories(fullPath, basePath);
    }
}


WindowsWatcher::WindowsWatcher()
{
    m_started = false;
}

bool WindowsWatcher::watchDirectory(QString path)
{
    HANDLE fd, file_fd;
    if (!QDir(path).exists())
    {
        return false;
    }
    path = QDir::toNativeSeparators(QDir(path).absolutePath());
    qDebug() << "watching path";
    foreach(QSet<QString> subdirs, m_subdirs.values())
    {
        if (subdirs.contains(path))
        {
            //this library always watchs out recursively. so there's no sense to watching a subdirectory.
            return true;
        }
    }
    if (m_watchingPaths.contains(path))
    {
        return true;
    }
    file_fd = ::CreateFileW(path.toStdWString().c_str(), FILE_LIST_DIRECTORY,
                         FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                         FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
    fd = ::FindFirstChangeNotificationW(path.toStdWString().c_str(),
                                        true, SELF_WATCH);

    m_fileHandle[fd] = file_fd;
    m_watchingPaths[path] = fd;
    m_watchingPathsHandles[fd] = path;
    m_results[fd] = new char[RESULT_SIZE];

    if (fd == INVALID_HANDLE_VALUE)
    {
        qCritical()  << "Unable to watch directory:" << path << ".";
        return false;
    }
    if (m_notifiers.contains(fd))
    {
        disconnect(m_notifiers[fd], SIGNAL(activated(HANDLE)),
                   this, SLOT(eventCallback(HANDLE)));
        delete m_notifiers[fd];
    }
    m_notifiers[fd] = new QWinEventNotifier(fd);
    m_notifiers[fd]->setEnabled(false);
    enumSubDirectories(path);
    connect(m_notifiers[fd], SIGNAL(activated(HANDLE)),
            this, SLOT(eventCallback(HANDLE)) );
    if (m_started)
    {
        asyncQueryDirectoryChanges(fd);
        m_notifiers[fd]->setEnabled(true);
    }
    return true;
}

bool WindowsWatcher::unwatchDirectory(QString path)
{
    if (!m_watchingPaths.contains(path))
        return false;
    HANDLE fd = m_watchingPaths[path];
    m_watchingPaths.remove(path);
    ::FindCloseChangeNotification(fd);
    HANDLE file_fd = m_fileHandle[fd];
    m_fileHandle.remove(fd);
    OVERLAPPED *over = m_overlappeds[fd];
    ::CloseHandle(over->hEvent);
    //delete over;
    //::CancelIo(fd);
    ::CloseHandle(file_fd);
    m_watchingPathsHandles.remove(fd);
    m_subdirs.remove(path);
    //delete m_results[fd];
    m_results.remove(fd);
    //delete m_overlappeds[fd];
    m_overlappeds.remove(fd);
    QWinEventNotifier* notifier = m_notifiers[fd];
    disconnect(notifier, SIGNAL(activated(HANDLE)),
               this, SLOT(eventCallback(HANDLE)));
    m_notifiers.remove(fd);
    return true;
}

void WindowsWatcher::start()
{
    if (m_started)
        return;
    foreach (HANDLE fd, m_notifiers.keys())
    {
        m_notifiers[fd]->setEnabled(true);
        asyncQueryDirectoryChanges(fd);
    }
    m_started = true;

}

void WindowsWatcher::stop()
{
    if (!m_started)
        return;
    foreach (QWinEventNotifier* notifier, m_notifiers.values())
    {
        notifier->setEnabled(false);
        disconnect(this, SLOT(eventCallback(HANDLE)));
    }
    m_started = false;

}

void WindowsWatcher::winEventCallback(HANDLE fd)
{
    //transfer the event to the main thread.
    metaObject()->invokeMethod(this, "eventCallback" , Qt::AutoConnection, Q_ARG(int, (int)fd));
}

void WindowsWatcher::eventCallback(HANDLE fd)
{

    //HANDLE fd = (HANDLE)winfd;
    FindNextChangeNotification(fd);
    int nextEntry = 1;
    int pos = 0;
    char* result = m_results[fd];
    OVERLAPPED* oldOverlapped = m_overlappeds[fd];
    QString path = m_watchingPathsHandles[fd];
    DWORD bytesRead;
    GetOverlappedResult(fd, oldOverlapped, &bytesRead, true);
    if (!bytesRead)
    {
        qDebug() << "no buffer!";
        ::CloseHandle(oldOverlapped->hEvent);
        delete oldOverlapped;
        QString directory = m_watchingPathsHandles[fd];
        unwatchDirectory(directory);
        emit failWatchingDirectory(directory);
        return;
    }

    while (nextEntry)
    {
        FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)(result + pos);
        nextEntry = event->NextEntryOffset;
        pos += nextEntry;

        QString eventName = path + QDir::separator() + QString::fromWCharArray(event->FileName, event->FileNameLength / sizeof(wchar_t));
        switch (event->Action)
        {
        case 0:
            if (QDir(eventName).exists())
            {
                m_subdirs[path].insert(eventName);
                debug(QStringList() << "UnknownEvent" << eventName);
                throw QString("UnknownEventError");
            }
            else
            {
                debug(QStringList() << "UnknownEvent" << eventName);
                throw QString("UnknownEventError");
            }
            break;
        case FILE_ACTION_ADDED:

            if (QDir(eventName).exists())
            {
                m_subdirs[path].insert(eventName);
                //debug(QStringList() << "DirectoryCreated" << eventName);
                emit directoryCreated(eventName);
            }
            else
            {
                //debug(QStringList() << "FileCreated" << eventName);
                emit fileCreated(eventName);
            }
            break;
        case FILE_ACTION_MODIFIED:
            if (m_subdirs[path].contains(eventName))
            {
                //debug(QStringList() << "DirectoryUpdated" << eventName);
                emit directoryChanged(eventName);
            }
            else
            {
                //debug(QStringList() << "FileUpdated" << eventName);
                emit fileUpdated(eventName);
            }
            break;
        case FILE_ACTION_REMOVED:
            if (m_subdirs[path].contains(eventName))
            {
                m_subdirs[path].remove(eventName);

                //debug(QStringList() << "DirectoryDeleted" << eventName);
                emit directoryDeleted(eventName);
            }
            else
            {
                //debug(QStringList() << "FileDeleted" << eventName);
                emit fileDeleted(eventName);
            }
            break;
        case FILE_ACTION_RENAMED_OLD_NAME:
            m_moveQueue[fd].append(eventName);
            break;
        case FILE_ACTION_RENAMED_NEW_NAME:
            {
                QString oldName = m_moveQueue[fd].dequeue();
                if (m_subdirs[path].contains(oldName))
                {
                    m_subdirs[path].insert(eventName);
                    m_subdirs[path].remove(oldName);
                    //debug(QStringList() << "DirectoryMoved" << oldName << eventName);
                    emit directoryMoved(oldName, eventName);
                }
                else
                {
                    //debug(QStringList() << "FileMoved" << oldName << eventName);
                    emit fileMoved(oldName, eventName);
                }

            }
        }
        asyncQueryDirectoryChanges(fd);
        CloseHandle(oldOverlapped->hEvent);
        delete oldOverlapped;
    }


}

WindowsWatcher::~WindowsWatcher()
{
    stop();
    return;
    foreach (QString path, m_watchingPaths.keys())
    {
        unwatchDirectory(path);
    }
}

