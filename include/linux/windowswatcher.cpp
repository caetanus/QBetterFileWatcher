#include "windowswatcher.h"

void WindowsWatcher::asyncQueryDirectoryChanges()
{
    m_overlapped.Internal = 0;
    m_overlapped.InternalHigh = 0;
    m_overlapped.Offset = 0;
    m_overlapped.OffsetHigh = 0;
    m_overlapped.Pointer = 0;
    m_overlapped.hEvent = 0;
    ReadDirectoryChangesW(m_fd, (void*)m_result, RESULT_SIZE, WatchRecursive,
                          SELF_WATCH, null, m_overlapped, null);
}

WindowsWatcher::WindowsWatcher(QObject *parent) :
    QObject(parent)
{
}


bool WindowsWatcher::watchDirectory(QString path)
{
    m_fd = CreateFileW(path.toStdWString().c_str(), FILE_LIST_DIRECTORY,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read);
    m_notifier->setEnabled(false);
    connect(m_notifier, SIGNAL(activated(int)), this, SLOT(eventCallback()), Qt::QueuedConnection);
    FindFirstChangeNotificationW(path.toStdWString().c_str(), WatchRecursive, SELF_WATCH);
    asyncQueryDirectoryChanges();
}

void WindowsWatcher::start()
{
    m_notifier->setEnabled(true);
}

void WindowsWatcher::eventCallback()
{


}
