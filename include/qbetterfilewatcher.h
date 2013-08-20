#pragma once

#ifdef __gnu_linux__
#   include "linux/inotifyfilewatcher.h"
#   define IMPLEMENTATION INotifyFileWatcher
#elif WIN32
#   include "windows/windowswatcher.h"
#   define IMPLEMENTATION WindowsWatcher
#endif

class QBetterFileWatcher : public IMPLEMENTATION
{

public:
    QBetterFileWatcher();
};

