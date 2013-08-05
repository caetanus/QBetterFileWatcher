#include "testdeletedirectory.h"
static const int numberOfDirectories = 128;

DeleteDirectoriesTestCase::DeleteDirectoriesTestCase()
{
    m_fwatcher = NULL;
}

void DeleteDirectoriesTestCase::setUp()
{
    delEvents = 0;
    selfCreateEvents.clear();
    createTemporaryDirectory();
    m_fwatcher = new QBetterFileWatcher();
    connect(m_fwatcher, SIGNAL(directoryDeleted(QString)), this, SLOT(onDirDeleted(QString)));

    qDebug() << "creating " << numberOfDirectories << "files.";
    for (int i = 0; i < numberOfDirectories; i++)
    {
        QString path(m_tempPath + QDir::separator() + QUuid::createUuid().toByteArray());
        QDir().mkpath(path);
        selfCreateEvents[path] = QDateTime::currentMSecsSinceEpoch();
    }

    m_fwatcher->watchDirectory(m_tempPath);
    m_fwatcher->start();

}


void DeleteDirectoriesTestCase::tearDown()
{
    disconnect(this, SLOT(onDirDeleted(QString)));
    m_fwatcher->unwatchDirectory(m_tempPath);
    m_fwatcher->stop();
    rmTree(m_tempPath);
    //delete m_fwatcher;
    m_fwatcher = NULL;
}

void DeleteDirectoriesTestCase::runTest()
{
    m_running = true;
    foreach(QString dir, QDir(m_tempPath).entryList(QDir::NoDotAndDotDot |
                                                     QDir::System |
                                                     QDir::Hidden  |
                                                     QDir::AllDirs |
                                                     QDir::Files
                                                     ))
    {
        rmTree(m_tempPath + QDir::separator() + dir);
    }
    qDebug() << "removed" << int(numberOfDirectories) << "directories";

    timeout.singleShot(3000, this, SLOT(stopTest()));
}


void DeleteDirectoriesTestCase::onDirDeleted(QString dirPath)
{

    deltaEvents[dirPath] = QDateTime::currentMSecsSinceEpoch() - selfCreateEvents[dirPath];
    selfCreateEvents.remove(dirPath);
    delEvents++;
    if (!selfCreateEvents.count())
    {
        //test has already passed.
        disconnect(this, SLOT(stopTest()));
        timeout.stop();
        stopTest();
    }
}

void DeleteDirectoriesTestCase::stopTest()
{
    if(m_running)
    {
        bool passed = false;
        m_running = false;
        if (selfCreateEvents.count() == 0)
        {
            qDebug() << "received " << delEvents << "delete events.";
            passed = true;
        }
        else
        {
            qDebug() << "Missing Events: " << selfCreateEvents.keys().length();
            passed = false;

        }
        tearDown();
        TestBase::stopTest(passed);
    }
}
