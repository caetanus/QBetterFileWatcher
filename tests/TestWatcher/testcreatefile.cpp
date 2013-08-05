#include "testcreatefile.h"

CreateFilesTestCase::CreateFilesTestCase()
{
    m_fwatcher = NULL;
}

void CreateFilesTestCase::setUp()
{
    createEvents = 0;
    selfCreateEvents.clear();
    createTemporaryDirectory();
    m_fwatcher = new QBetterFileWatcher();
    connect(m_fwatcher, SIGNAL(fileCreated(QString)), this, SLOT(onFileCreated(QString)));
    m_fwatcher->watchDirectory(m_tempPath);
    m_fwatcher->start();

}


void CreateFilesTestCase::tearDown()
{
    disconnect(this, SLOT(onFileCreated(QString)));
    m_fwatcher->unwatchDirectory(m_tempPath);
    m_fwatcher->stop();
    rmTree(m_tempPath);
    delete m_fwatcher;
    m_fwatcher = NULL;
}

void CreateFilesTestCase::runTest()
{
    const int numberOfFiles = 128;
    m_running = true;
    qDebug() << "creating " << numberOfFiles << "files.";
    for (int i = 0; i < numberOfFiles; i++)
    {
        createRandomFile();
    }
    //timeout.singleShot(3000, this, SLOT(stopTest()));
}


void CreateFilesTestCase::onFileCreated(QString filepath)
{
    deltaEvents[filepath] = QDateTime::currentMSecsSinceEpoch() - selfCreateEvents[filepath];
    selfCreateEvents.remove(filepath);
    createEvents++;
    if (!selfCreateEvents.count())
    {
        //test has already passed.
        disconnect(this, SLOT(stopTest()));
        timeout.stop();
        stopTest();
    }
}

void CreateFilesTestCase::stopTest()
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

