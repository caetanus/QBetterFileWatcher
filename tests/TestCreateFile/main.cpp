#include <QCoreApplication>
#include <QObject>
#include <QMap>
#include <QDir>
#include <QUuid>

#include <QBetterFileWatcher>

class CreateFileTestCase : public QObject
{
    Q_OBJECT
    QString m_tempPath;

    void createTempDirectory()
    {
        QString path = QDir::tempPath() + QDir::separator() + QUuid::createUuid().toByteArray();
        dpath.mkpath(path);
        m_tempPath = path;
    }
protected:
    void createRandomFile()
    {


    }


public:
    QMap<QString, int> selfEvents;
    CreateFileTestCase()
    {
        setUp();
    }
    void setUp()
    {
        createTempDirectory();

    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    return a.exec();
}

#include "main.moc"
