#include "testmovefile.h"

MoveFileTestCase::MoveFileTestCase()
{
    m_fwatcher = NULL;
    m_passed = false;
    m_gotEvent = false;
    m_timeout = new QTimer(this);
}

void MoveFileTestCase::setUp()
{
    createEvents = 0;
    selfCreateEvents.clear();
    createTemporaryDirectory();
    m_randomFile = createRandomFile();
    m_randomFileNew = m_tempPath + QDir::separator() + QUuid::createUuid().toByteArray();
    m_fwatcher = new QBetterFileWatcher();
    connect(m_fwatcher, SIGNAL(fileMoved(QString,QString)),
            this, SLOT(onFileMoved(QString, QString)));
    m_fwatcher->watchDirectory(m_tempPath);
    m_fwatcher->start();

}

void MoveFileTestCase::tearDown()
{
    disconnect(this, SLOT(onFileMoved(QString, QString)));
    m_timeout->stop();
    m_fwatcher->unwatchDirectory(m_tempPath);
    m_fwatcher->stop();
    rmTree(m_tempPath);
    delete m_fwatcher;
    m_fwatcher = NULL;
}




void MoveFileTestCase::runTest()
{
    m_running = true;

    QFile::rename(m_randomFile, m_randomFileNew);
    connect(this->m_timeout, SIGNAL(timeout()),
            this, SLOT(onTimeout()));

    //TODO: investigate why this timers wont working.
    m_timeout->setSingleShot(true);
    m_timeout->setInterval(10);
    m_timeout->start();


}


void MoveFileTestCase::onFileMoved(QString oldFilePath, QString newFilePath)
{
    moveEvent.first = oldFilePath;
    moveEvent.second = newFilePath;
    m_timeout->stop();
    if (oldFilePath == m_randomFile && newFilePath == m_randomFileNew)
        m_passed = true;
    stopTest();

}
void MoveFileTestCase::onTimeout()
{
    qDebug() << "weee!";
    stopTest();
}

void MoveFileTestCase::stopTest()
{
    if (!m_passed)
    {
        if (m_gotEvent)
        {
            qDebug() << "Test was expecting: "
                     << m_randomFile
                     << m_randomFileNew;
            qDebug() << "Got:"
                     << moveEvent.first
                     << moveEvent.second;
        }
        else
        {
            qDebug() << "Test ain't got move event.";
        }

    }
    TestBase::stopTest(m_passed);
}

