#include "windowswatcher.h"
#include <windows.h>

void getLastError(LPWSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();
    if (dw == ERROR_SUCCESS)
        return;

    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process
    qDebug() << (wchar_t*)lpszFunction << (wchar_t*)lpMsgBuf;

    LocalFree(lpMsgBuf);
}



void WindowsWatcher::asyncQueryDirectoryChanges(HANDLE fd)
{
    OVERLAPPED* overlapped = new OVERLAPPED;
    m_overlappeds[fd] = overlapped;
    memset(overlapped, 0, sizeof(OVERLAPPED));
    void* result = (void*)m_results[fd];
    //memset(result, 0, RESULT_SIZE);
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
}

bool WindowsWatcher::watchDirectory(QString path)
{
    HANDLE fd, file_fd;
    if (!QDir(path).exists())
    {
        return false;
    }
    path = QDir::toNativeSeparators(QDir(path).absolutePath());
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
        qDebug() << "Unable to watch directory:" << path << ".";
        return false;
    }
    else {
        qDebug() << "watching directory: " << path;
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
    return true;
}

bool WindowsWatcher::unwatchDirectory(QString path)
{
    if (!m_watchingPaths.contains(path))
        return false;
    HANDLE fd = m_watchingPaths[path];
    HANDLE file_fd = m_fileHandle[fd];
    m_fileHandle.remove(fd);
    ::CancelIo(fd);
    ::CloseHandle(file_fd);
    ::FindCloseChangeNotification(fd);
    m_watchingPaths.remove(path);
    m_watchingPathsHandles.remove(fd);
    m_subdirs.remove(path);
    delete m_results[fd];
    m_results.remove(fd);
    delete m_overlappeds[fd];
    m_overlappeds.remove(fd);
    QWinEventNotifier* notifier = m_notifiers[fd];
    disconnect(notifier, SIGNAL(activated(HANDLE)),
               this, SLOT(eventCallback(HANDLE)));
    m_notifiers.remove(fd);
    return true;
}

void WindowsWatcher::start()
{
    foreach (HANDLE fd, m_notifiers.keys())
    {
        QString path = m_watchingPathsHandles[fd];
        m_notifiers[fd]->setEnabled(true);
        asyncQueryDirectoryChanges(fd);
    }

}

void WindowsWatcher::stop()
{
    foreach (QWinEventNotifier* notifier, m_notifiers.values())
    {
        notifier->setEnabled(false);
    }

}

void WindowsWatcher::eventCallback(HANDLE fd)
{
    FindNextChangeNotification(fd);
    getLastError((LPWSTR)L"FindNextChange eventCallback");
    int nextEntry = 1;
    int pos = 0;
    char* result = m_results[fd];
    OVERLAPPED* oldOverlapped = m_overlappeds[fd];
    QString path = m_watchingPathsHandles[fd];
    DWORD bytesRead;
    GetOverlappedResult(fd, oldOverlapped, &bytesRead, true);
    if (!bytesRead)
        return;

    qDebug() << "event file descriptor" << fd << path;

    while (nextEntry)
    {
        FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)(result + pos);
        nextEntry = event->NextEntryOffset;
        pos += nextEntry;
        qDebug() << pos << nextEntry;

        QString eventName = path + QDir::separator() + QString::fromWCharArray(event->FileName, event->FileNameLength / sizeof(wchar_t));
        qDebug() << event->Action;
        switch (event->Action)
        {
        case 0:
            if (QDir(eventName).exists())
            {
                m_subdirs[path].insert(eventName);
                debug(QStringList() << "UnknownEvent" << eventName);
                emit directoryCreated(eventName);
            }
            else
            {
                debug(QStringList() << "UnknownEvent" << eventName);
                emit fileCreated(eventName);
            }
            break;
        case FILE_ACTION_ADDED:
            qDebug() << eventName << QDir(eventName).exists();

            if (QDir(eventName).exists())
            {
                m_subdirs[path].insert(eventName);
                debug(QStringList() << "DirectoryCreated" << eventName);
                emit directoryCreated(eventName);
            }
            else
            {
                debug(QStringList() << "FileCreated" << eventName);
                emit fileCreated(eventName);
            }
            break;
        case FILE_ACTION_MODIFIED:
            if (m_subdirs.contains(eventName))
            {
                debug(QStringList() << "DirectoryUpdated" << eventName);
                emit directoryChanged(eventName);
            }
            else
            {
                debug(QStringList() << "FileUpdated" << eventName);
                emit fileUpdated(eventName);
            }
            break;
        case FILE_ACTION_REMOVED:
            qDebug() << "Deleted and don't know yet what is it." << eventName;
            if (m_subdirs[path].contains(eventName))
            {
                m_subdirs[path].remove(eventName);

                debug(QStringList() << "DirectoryDeleted" << eventName);
                emit directoryDeleted(eventName);
            }
            else
            {
                debug(QStringList() << "FileDeleted" << eventName);
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
                    debug(QStringList() << "DirectoryMoved" << oldName << eventName);
                    emit directoryMoved(oldName, eventName);
                }
                else
                {
                    debug(QStringList() << "FileMoved" << oldName << eventName);
                    emit fileMoved(oldName, eventName);
                }

            }
        }
        bool retValue = GetOverlappedResult(fd, oldOverlapped, &bytesRead, true);
        qDebug() << "bytes read" << bytesRead;
        getLastError((LPWSTR)L"eventCallback");
        delete oldOverlapped;

        /*if (!retValue)
        {
            qCritical() << "WindowsFileWatcher Error:" << GetLastError();
        }*/
        asyncQueryDirectoryChanges(fd);
    }


}

WindowsWatcher::~WindowsWatcher()
{
    foreach (QString path, m_watchingPaths.keys())
    {
        unwatchDirectory(path);
    }
    stop();
}
