#include <QCoreApplication>
#include "testbase.h"
#include "testcreatefile.h"
#include "testcreatedirectory.h"
#include "testdeletedirectory.h"
#include "testdeletefiles.h"
#include "testmovefile.h"
#include "testmoveintodirectory.h"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TestRunner testRunner;
    testRunner
              << new CreateFilesTestCase()
              << new DeleteFilesTestCase()
              << new CreateDirectoriesTestCase()
              << new DeleteDirectoriesTestCase()
              << new MoveFileTestCase()
              << new MoveFileIntoDirectoryTestCase()
                 ;
    qDebug() <<  a.thread();
    testRunner.run();
    return a.exec();
}
