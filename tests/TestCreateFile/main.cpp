#include <QCoreApplication>
#include <QObject>
#include <QMap>
#include <QDir>
#include <QUuid>
#include <QDateTime>
#include <QTimer>

#include <QBetterFileWatcher>


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


class CreateFileTestCase : public QObject
{
    Q_OBJECT
    QString m_tempPath;
    QBetterFileWatcher* m_fwatcher;
    QTimer timeout;

    void createTempDirectory()
    {
        QString path = QDir::tempPath() + QDir::separator() + QString("QBetterFileWatcherTest-") + QUuid::createUuid().toByteArray();
        QDir().mkpath(path);
        m_tempPath = path;
    }
protected:
    void createRandomFile(int size=1024)
    {
        char* data = (char*)malloc(size); //unitialized pointer, but.. who cares?

        QFile tempFile(m_tempPath + QDir::separator() + QUuid::createUuid().toByteArray());
        selfEvents[tempFile.fileName()] = QDateTime::currentMSecsSinceEpoch();
        tempFile.open(QIODevice::WriteOnly);

        tempFile.write(data, size);
        tempFile.close();
        free(data);
    }


public:
    QMap<QString, quint64> selfEvents;
    QMap<QString, quint64> deltaEvents;
    int createEvents;
    CreateFileTestCase()
    {
        m_fwatcher = NULL;
    }
    void setUp()
    {
        createEvents = 0;
        selfEvents.clear();
        createTempDirectory();
        m_fwatcher = new QBetterFileWatcher();
        connect(m_fwatcher, SIGNAL(fileCreated(QString)), this, SLOT(onFileCreated(QString)));
        m_fwatcher->watchDirectory(m_tempPath);
        m_fwatcher->start();
    }
    void tearDown()
    {
        m_fwatcher->unwatchDirectory(m_tempPath);
        rmTree(m_tempPath);
        delete m_fwatcher;
        m_fwatcher = NULL;
    }
    void runTests()
    {
        const int numberOfFiles = 4096;
        qDebug() << "creating " << numberOfFiles << "files.";
        for (int i = 0; i < numberOfFiles; i++)
        {
            createRandomFile();
        }
        timeout.singleShot(3000, this, SLOT(stopTest()));
    }
public slots:
    void onFileCreated(QString filepath)
    {
        deltaEvents[filepath] = QDateTime::currentMSecsSinceEpoch() - selfEvents[filepath];
        selfEvents.remove(filepath);
        createEvents++;
        if (!selfEvents.count())
        {
            //test has already passed.
            timeout.stop();
            stopTest();
        }
    }

    void stopTest()
    {
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
        QCoreApplication::exit();
    }

};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CreateFileTestCase t;
    t.setUp();
    t.runTests();
    return a.exec();
}

#include "main.moc"
