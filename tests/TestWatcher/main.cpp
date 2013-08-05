#include <QCoreApplication>
#include "testbase.h"
#include "testcreatefile.h"
#include "testcreatedirectory.h"
#include "testdeletedirectory.h"
#include "testdeletefiles.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TestRunner testRunner;
    testRunner
              << new CreateFilesTestCase()
              << new CreateDirectoriesTestCase()
               << new DeleteDirectoriesTestCase()
                 ;
    testRunner.run();
    return a.exec();
}
