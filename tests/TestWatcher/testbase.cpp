#include "testbase.h"
#include <QDebug>
#include <QCoreApplication>


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




void TestBase::createTemporaryDirectory()
{
    QString path = QDir::tempPath() + QDir::separator() + QString("QBetterFileWatcherTest-") + QUuid::createUuid().toByteArray();
    QDir().mkpath(path);
    m_tempPath = path;
}

void TestBase::createRandomFile(int size)
{
    char* data = (char*)malloc(size); //unitialized pointer, but.. who cares?

    QFile tempFile(m_tempPath + QDir::separator() + QUuid::createUuid().toByteArray());
    selfCreateEvents[tempFile.fileName()] = QDateTime::currentMSecsSinceEpoch();
    tempFile.open(QIODevice::WriteOnly);

    tempFile.write(data, size);
    tempFile.close();
    free(data);
}


void TestBase::stopTest(bool passed)
{
    emit testStop(this, passed);
}


TestRunner::TestRunner()
{
}

void TestRunner::testResult(TestBase *test, bool result)
{
    QString strResult;
    if (result)
    {
        strResult = "PASSED!";
        m_passed++;
    }
    else strResult = "FAILED!";
    qDebug() << test->metaObject()->className() << strResult << "\n\n";
    disconnect(test, SIGNAL(testStop(TestBase*,bool)),
               this, SLOT(testResult(TestBase*,bool)));
    //delete test;
    runNextTest();
}

void TestRunner::runNextTest()
{
    m_current++;
    if (m_current >= m_tests.length())
    {
        bool hasPassed = m_passed == m_tests.length();
        if (hasPassed)
        {
            qDebug() << "All tests has passed.";
            QCoreApplication::exit(0);
        }
        else
        {
            qDebug() << m_passed << "of" << m_tests.length() << "has passed.";
            QCoreApplication::exit(-1);
        }
        m_tests.clear();
        QCoreApplication::quit();

    }
    else {
        qDebug() <<  m_current+1 << "Of" << m_tests.length();
        m_tests[m_current]->setUp();
        qDebug() << "\n===============================================";
        qDebug() << "Testing: " << m_tests[m_current]->metaObject()->className();
        m_tests[m_current]->runTest();
    }
}

void TestRunner::run()
{
    m_current = -1;
    m_passed = 0;
    runNextTest();
}

TestRunner& TestRunner::operator <<(TestBase* test)
{
    connect(test, SIGNAL(testStop(TestBase*,bool)), this, SLOT(testResult(TestBase*,bool)));
    m_tests<<(test);
    return *this;
}
