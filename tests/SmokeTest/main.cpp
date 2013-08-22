#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QBetterFileWatcher>
#include <QSocketNotifier>
#include <QMainWindow>
#include <QPushButton>

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
    QApplication qapp(argc, argv);
    Watcher watcher;
    QMainWindow w;
    QPushButton b;
    b.setParent(&w);
    w.show();
    b.setText("tha fockin bottom");
    b.setMaximumSize(w.size());
    b.setMinimumSize(w.size());
    b.show();
    watcher.watchDirectory(argv[1]);
    watcher.start();
    return qapp.exec();
}

#include "main.moc"
