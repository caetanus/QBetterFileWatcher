#include "TestBase.h"
#include <QDebug>

void TestBase::stopTest()
{
    emit testStop();
}
