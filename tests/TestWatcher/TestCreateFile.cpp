#include "TestCreateFile.h"


bool rmTree(QString completePath)
{

    bool result = true;
    QDir dir(completePath);

    if (dir.exists(completePath)) {
        QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot |
                                                  QDir::System |
                                                  QDir::Hidden  |
                                                  QDir::AllDirs |
                                                  QDir::Files,
                                                  QDir::DirsFirst);
         foreach(QFileInfo info, entries) {
            if (info.isDir()) {
                result = rmTree(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(completePath);
    }
    return result;
}


void CreateFileTestCase::createTempDirectory()
{
    QString path = QDir::tempPath() + QDir::separator() + QString("QBetterFileWatcherTest-") + QUuid::createUuid().toByteArray();
    QDir().mkpath(path);
    m_tempPath = path;
}

void CreateFileTestCase::createRandomFile(int size)
{
    char* data = (char*)malloc(size); //unitialized pointer, but.. who cares?

    QFile tempFile(m_tempPath + QDir::separator() + QUuid::createUuid().toByteArray());
    selfEvents[tempFile.fileName()] = QDateTime::currentMSecsSinceEpoch();
    tempFile.open(QIODevice::WriteOnly);

    tempFile.write(data, size);
    tempFile.close();
    free(data);
}

CreateFileTestCase::CreateFileTestCase()
{
    m_fwatcher = NULL;
}

void CreateFileTestCase::setUp()
{
    qDebug() << "Running Test: CreateFileTestCase";
    createEvents = 0;
    selfEvents.clear();
    createTempDirectory();
    m_fwatcher = new QBetterFileWatcher();
    connect(m_fwatcher, SIGNAL(fileCreated(QString)), this, SLOT(onFileCreated(QString)));
    m_fwatcher->watchDirectory(m_tempPath);
    m_fwatcher->start();

}


void CreateFileTestCase::tearDown()
{
    m_fwatcher->unwatchDirectory(m_tempPath);
    m_fwatcher->stop();
    disconnect(this, SLOT(onFileCreated(QString)));
    rmTree(m_tempPath);
    delete m_fwatcher;
    m_fwatcher = NULL;
}

void CreateFileTestCase::runTests()
{
    const int numberOfFiles = 4096;
    m_running = true;
    qDebug() << "creating " << numberOfFiles << "files.";
    for (int i = 0; i < numberOfFiles; i++)
    {
        createRandomFile();
    }
    timeout.singleShot(3000, this, SLOT(stopTest()));
}


void CreateFileTestCase::onFileCreated(QString filepath)
{
    deltaEvents[filepath] = QDateTime::currentMSecsSinceEpoch() - selfEvents[filepath];
    selfEvents.remove(filepath);
    createEvents++;
    if (!selfEvents.count())
    {
        //test has already passed.
        disconnect(this, SLOT(stopTest()));
        timeout.stop();
        stopTest();
    }
}

void CreateFileTestCase::stopTest()
{
    if(m_running)
    {
        m_running = false;
        if (selfEvents.count() == 0)
        {
            qDebug() << "received " << createEvents << "create events.";
            qDebug() << "PASSED!";
        }
        else
        {
            qDebug() << "FAILED!";
            qDebug() << "Missing Events: " << selfEvents.keys().length();

        }
        tearDown();
        TestBase::stopTest();
    }
}

