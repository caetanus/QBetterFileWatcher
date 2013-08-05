#include "testcreatesubdirs.h"
static const int numberOfDirectories = 128;

CreateSubDirectoriesTestCase::CreateSubDirectoriesTestCase()
{
    m_fwatcher = NULL;
}

void CreateSubDirectoriesTestCase::setUp()
{
    createEvents = 0;
    selfCreateEvents.clear();
    createTemporaryDirectory();
    m_fwatcher = new QBetterFileWatcher();
    connect(m_fwatcher, SIGNAL(directoryCreated(QString)), this, SLOT(onDirCreated(QString)));
    m_fwatcher->watchDirectory(m_tempPath);
    m_fwatcher->start();

}


void CreateSubDirectoriesTestCase::tearDown()
{
    disconnect(this, SLOT(onDirCreated(QString)));
    m_fwatcher->unwatchDirectory(m_tempPath);
    m_fwatcher->stop();
    rmTree(m_tempPath);
    delete m_fwatcher;
    m_fwatcher = NULL;
}

void CreateSubDirectoriesTestCase::createSubDir()
{
    foreach(QString path, m_parentDirs)
    {
        QString subpath(path + QDir::separator() + QUuid::createUuid().toByteArray());
        QDir().mkpath(subpath);
        selfCreateEvents[subpath] = QDateTime::currentMSecsSinceEpoch();
    }

}

void CreateSubDirectoriesTestCase::runTest()
{

    m_running = true;
    qDebug() << "creating " << numberOfDirectories << "directories.";
    for (int i = 0; i < numberOfDirectories; i++)
    {
        QString path(m_tempPath + QDir::separator() + QUuid::createUuid().toByteArray());
        m_parentDirs << path;
        QDir().mkpath(path);
        selfCreateEvents[path] = QDateTime::currentMSecsSinceEpoch();
    }

    //timeout.singleShot(3000, this, SLOT(stopTest()));
}


void CreateSubDirectoriesTestCase::onDirCreated(QString dirPath)
{
    deltaEvents[dirPath] = QDateTime::currentMSecsSinceEpoch() - selfCreateEvents[dirPath];
    selfCreateEvents.remove(dirPath);
    createEvents++;
    if (createEvents == numberOfDirectories)
    {
        createSubDir(); // this method is buggy, because whe need the mainthread to capture the create directory event.
        return;
    }
    else if (!selfCreateEvents.count() && createEvents > numberOfDirectories)
    {
        //test has already passed.
        disconnect(this, SLOT(stopTest()));
        timeout.stop();
        stopTest();
    }
}

void CreateSubDirectoriesTestCase::stopTest()
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

