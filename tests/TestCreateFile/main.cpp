#include <QCoreApplication>
#include <QObject>
#include <QMap>
#include <QDir>
#include <QUuid>
#include <QDateTime>
#include <QTimer>

#include <QBetterFileWatcher>

class CreateFileTestCase : public QObject
{
    Q_OBJECT
    QString m_tempPath;
    QBetterFileWatcher* m_fwatcher;
    QTimer timeout;

    void createTempDirectory()
    {
        QString path = QDir::tempPath() + QDir::separator() + QUuid::createUuid().toByteArray();
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
    CreateFileTestCase()
    {
        m_fwatcher = NULL;
    }
    void setUp()
    {
        selfEvents.clear();
        createTempDirectory();
        m_fwatcher = new QBetterFileWatcher();
        connect(m_fwatcher, SIGNAL(fileCreated(QString)), this, SLOT(onFileCreated(QString)));
        m_fwatcher->watchDirectory(m_tempPath);
    }
    void tearDown()
    {
        m_fwatcher->unwatchDirectory(m_tempPath);
        QDir().rmpath(m_tempPath);
        delete m_fwatcher;
        m_fwatcher = NULL;
    }
    void runTests()
    {
        timeout.singleShot(300, this, SLOT(stopTest()));
        const int numberOfFiles = 1024;
        qDebug() << "creating " << numberOfFiles;
        for (int i = 0; i < numberOfFiles; i++)
        {
            createRandomFile();
        }
    }
public slots:
    void onFileCreated(QString filepath)
    {
        deltaEvents[filepath] = QDateTime::currentMSecsSinceEpoch() - selfEvents[filepath];
    }

    void stopTest()
    {
        if (selfEvents.count() == 0)
        {
            qDebug() << "Passed!";
        }
        else
        {
            qDebug() << "Failed";
            //qDebug() << "Missing Events: " << selfEvents.keys();

        }
        foreach(QString file, deltaEvents.keys())
        {
            qDebug() << file << deltaEvents[file];

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
