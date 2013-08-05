#include "testdeletefiles.h"
const int numberOfFiles = 128;

DeleteFilesTestCase::DeleteFilesTestCase()
{
    m_fwatcher = NULL;
}

void DeleteFilesTestCase::setUp()
{
    delEvents = 0;
    selfCreateEvents.clear();
    createTemporaryDirectory();
    m_fwatcher = new QBetterFileWatcher();
    connect(m_fwatcher, SIGNAL(fileDeleted(QString)), this, SLOT(onFileDeleted(QString)));

    qDebug() << "creating " << numberOfFiles << "files.";
    for (int i = 0; i < numberOfFiles; i++)
    {
        createRandomFile();
    }

    m_fwatcher->watchDirectory(m_tempPath);
    m_fwatcher->start();

}


void DeleteFilesTestCase::tearDown()
{
    disconnect(this, SLOT(onFileDeleted(QString)));
    m_fwatcher->unwatchDirectory(m_tempPath);
    m_fwatcher->stop();
    rmTree(m_tempPath);
    //delete m_fwatcher;
    m_fwatcher = NULL;
}

void DeleteFilesTestCase::runTest()
{
    m_running = true;
    foreach(QString file, QDir(m_tempPath).entryList(QDir::NoDotAndDotDot |
                                                     QDir::System |
                                                     QDir::Hidden  |
                                                     QDir::AllDirs |
                                                     QDir::Files
                                                     ))
    {
        QFile(m_tempPath + QDir::separator() + file).remove();
    }
    qDebug() << "removed" << int(numberOfFiles) << "files";

    timeout.singleShot(3000, this, SLOT(stopTest()));
}


void DeleteFilesTestCase::onFileDeleted(QString filepath)
{
    deltaEvents[filepath] = QDateTime::currentMSecsSinceEpoch() - selfCreateEvents[filepath];
    selfCreateEvents.remove(filepath);
    delEvents++;
    if (!selfCreateEvents.count())
    {
        //test has already passed.
        disconnect(this, SLOT(stopTest()));
        timeout.stop();
        stopTest();
    }
}

void DeleteFilesTestCase::stopTest()
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
