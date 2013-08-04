#include <QCoreApplication>
#include "TestCreateFile.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CreateFileTestCase t;
    t.setUp();
    t.runTests();
    return a.exec();
}

