#include <QCoreApplication>
#include <QObject>
#include <QStringList>
#include <QBetterFileWatcher>
#include <QSocketNotifier>
#include <QCoreApplication>


class Watcher: public QBetterFileWatcher
{
    Q_OBJECT

public:
    Watcher() : QBetterFileWatcher()
    {
        connect(this, SIGNAL(debugInformation(QStringList)),
                this, SLOT(printDebugInformation(QStringList)));
    }

public slots:
    void printDebugInformation(QStringList info)
    {
        qDebug() << info;
    }

};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Watcher watcher;
    watcher.watchDirectory(argv[1]);
    watcher.start();
    return a.exec();
}

#include "main.moc"
