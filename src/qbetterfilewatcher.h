#pragma once

#ifdef __gnu_linux__
#   include "linux/inotifyfilewatcher.h"
#   define IMPLEMENTATION InotifyFileWatcher
#endif

class QBetterFileWatcher : public IMPLEMENTATION
{
    
public:
    QBetterFileWatcher();
};

