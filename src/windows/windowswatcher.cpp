#include "windowswatcher.h"



void WindowsWatcher::asyncQueryDirectoryChanges()
{
    m_overlapped.Internal = 0;
    m_overlapped.InternalHigh = 0;
    m_overlapped.hEvent = 0;
    ::ReadDirectoryChangesW(m_fd, (void*)m_result, RESULT_SIZE, WatchRecursive,
                          SELF_WATCH, NULL, &m_overlapped, NULL);
}

void WindowsWatcher::enumSubDirectories(QString path)
{
    foreach(QString subDir, QDir(path).entryList(QDir::AllDirs |
                                                 QDir::NoDotAndDotDot |
                                                 QDir::Hidden))
    {
        QString fullPath = QDir::toNativeSeparators(path + QDir::separator() + subDir);
        m_subdirs.insert(fullPath);
        enumSubDirectories(fullPath);
    }
}


WindowsWatcher::WindowsWatcher()
{
    m_fd = INVALID_HANDLE_VALUE;
    m_notifier = NULL;
}

bool WindowsWatcher::watchDirectory(QString path)
{
    if (m_fd != INVALID_HANDLE_VALUE)
        CloseHandle(m_fd);
    if (!QDir(path).exists())
    {
        m_watchingPath = "";
        return false;
    }
    path = QDir::toNativeSeparators(QDir(path).absolutePath());
    m_watchingPath = path;
    m_fd = ::CreateFileW(path.toStdWString().c_str(), FILE_LIST_DIRECTORY,
                         FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                         FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
    if (m_fd == INVALID_HANDLE_VALUE)
    {
        qDebug() << "Unable to watch directory:" << path << ".";
        return false;
    }
    else {
        qDebug() << "watching directory: " << path;
    }
    if (m_notifier)
    {
        disconnect(this, SLOT(eventCallback()));
        delete m_notifier;
    }
    m_notifier = new QWinEventNotifier(m_fd);
    m_notifier->setEnabled(false);
    enumSubDirectories(path);
    connect(m_notifier, SIGNAL(activated(HANDLE)), this, SLOT(eventCallback()));
    return true;
}

bool WindowsWatcher::unwatchDirectory(QString path)
{
    if (m_watchingPath != path)
        return false;
    stop();
    m_watchingPath = "";
    return true;
}

int WindowsWatcher::getHandle()
{
    return (int)m_fd;
}


void WindowsWatcher::start()
{
    if(!m_fd)
    {
        if (m_watchingPath == "")
            return;
        m_subdirs.clear();
        watchDirectory(m_watchingPath);
    }
    m_notifier->setEnabled(true);
    ::FindFirstChangeNotificationW(m_watchingPath.toStdWString().c_str(), WatchRecursive, SELF_WATCH);
    asyncQueryDirectoryChanges();
}

void WindowsWatcher::stop()
{
    if (m_notifier)
    {
        m_notifier->setEnabled(false);
        disconnect(this, SLOT(eventCallback()));
        delete m_notifier;
    }
    if(m_fd != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_fd);
        m_fd = INVALID_HANDLE_VALUE;
    }

}

void WindowsWatcher::eventCallback()
{
    FindNextChangeNotification(m_fd);
    int nextEntry = 1;
    int pos = 0;
    wchar_t fileName[4096] = {0};
    OVERLAPPED oldOverlapped = m_overlapped;

    while (nextEntry)
    {
        FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)(m_result + pos);
        nextEntry = event->NextEntryOffset;
        pos += nextEntry;
        memset(fileName, 0, sizeof(fileName));
        memcpy(event->FileName, fileName, event->FileNameLength);
        QString eventName = QString::fromWCharArray(fileName);
        switch (event->Action)
        {
        case FILE_ACTION_ADDED:
        case FILE_ACTION_ADDED_STREAM:
            if (QDir(eventName).exists())
            {
                m_subdirs.insert(eventName);
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
        case FILE_ACTION_MODIFIED_STREAM:
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
        case FILE_ACTION_REMOVED_BY_DELETE:
        case FILE_ACTION_REMOVED_STREAM:
            if (m_subdirs.contains(eventName))
            {
                m_subdirs.remove(eventName);

                debug(QStringList() << "DirectoryDeleted" << eventName);
                emit directoryDeleted(eventName);
            }
            else
            {
                debug(QStringList() << "DirectoryFile" << eventName);
                emit fileDeleted(eventName);
            }
            break;
        case FILE_ACTION_RENAMED_OLD_NAME:
            m_moveQueue.append(eventName);
            break;
        case FILE_ACTION_RENAMED_NEW_NAME:
            {
                QString oldName = m_moveQueue.dequeue();
                if (m_subdirs.contains(oldName))
                {
                    m_subdirs.insert(eventName);
                    m_subdirs.remove(oldName);
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
        bool retValue = GetOverlappedResult(m_fd, &oldOverlapped, (DWORD*)m_result, true);

        if (!retValue)
        {
            qCritical() << "WindowsFileWatcher Error:" << GetLastError();
        }
        asyncQueryDirectoryChanges();
    }


}

WindowsWatcher::~WindowsWatcher()
{
    unwatchDirectory(m_watchingPath);
    stop();
}
