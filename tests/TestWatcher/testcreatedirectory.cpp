#include "testcreatedirectory.h"
static const int numberOfDirectories = 128;

CreateDirectoriesTestCase::CreateDirectoriesTestCase()
{
    m_fwatcher = NULL;
}

void CreateDirectoriesTestCase::setUp()
{
    createEvents = 0;
    selfCreateEvents.clear();
    createTemporaryDirectory();
    m_fwatcher = new QBetterFileWatcher();
    connect(m_fwatcher, SIGNAL(directoryCreated(QString)), this, SLOT(onDirCreated(QString)));
    m_fwatcher->watchDirectory(m_tempPath);
    m_fwatcher->start();

}


void CreateDirectoriesTestCase::tearDown()
{
    disconnect(this, SLOT(onDirCreated(QString)));
    m_fwatcher->unwatchDirectory(m_tempPath);
    m_fwatcher->stop();
    rmTree(m_tempPath);
    delete m_fwatcher;
    m_fwatcher = NULL;
}

void CreateDirectoriesTestCase::runTest()
{

    m_running = true;
    qDebug() << "creating " << numberOfDirectories << "directories.";
    for (int i = 0; i < numberOfDirectories; i++)
    {
        QString path(m_tempPath + QDir::separator() + QUuid::createUuid().toByteArray());
        QDir().mkpath(path);
        selfCreateEvents[path] = QDateTime::currentMSecsSinceEpoch();
    }
    //timeout.singleShot(3000, this, SLOT(stopTest()));
}


void CreateDirectoriesTestCase::onDirCreated(QString dirPath)
{
    deltaEvents[dirPath] = QDateTime::currentMSecsSinceEpoch() - selfCreateEvents[dirPath];
    selfCreateEvents.remove(dirPath);
    createEvents++;
    if (!selfCreateEvents.count())
    {
        //test has already passed.
        disconnect(this, SLOT(stopTest()));
        timeout.stop();
        stopTest();
    }
}

void CreateDirectoriesTestCase::stopTest()
{
    if(m_running)
    {
        bool passed = false;
        m_running = false;
        if (selfCreateEvents.count() == 0)
        {
            qDebug() << "received " << createEvents << "create events.";
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

