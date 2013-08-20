#include "AbstractWatcher.h"

void AbstractWatcher::debug(QStringList debugInfo)
{
#ifdef DEBUG_INFORMATION
    emit debugInformation(debugInfo);
#else
    Q_UNUSED(debugInfo)
#endif
}
