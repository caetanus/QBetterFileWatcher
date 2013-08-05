#include <QCoreApplication>
#include "testbase.h"
#include "testcreatefile.h"
#include "testcreatedirectory.h"
#include "testdeletedirectory.h"
#include "testdeletefiles.h"
#include "testcreatesubdirfile.h"
#include "testdeletesubdirfiles.h"
#include "testcreatesubdirs.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TestRunner testRunner;
    testRunner
              << new CreateFilesTestCase()
              << new CreateSubdirFileTestCase()
              << new DeleteFilesTestCase()
              << new DeleteSubdirFilesTestCase()
              << new CreateDirectoriesTestCase()
              << new CreateSubDirectoriesTestCase()
              << new DeleteDirectoriesTestCase()
                 ;
    testRunner.run();
    return a.exec();
}
