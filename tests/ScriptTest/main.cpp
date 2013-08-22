#include "pythoncontext.h"
#include "PythonQtScriptingConsole.h"
#include "QBetterFileWatcher"
#include <QApplication>
#include <PythonQt.h>

class Watcher: public QBetterFileWatcher
{
    Q_OBJECT

public:
    Watcher(QString path): QBetterFileWatcher()
    {
        connect(this, SIGNAL(directoryCreated(QString)),
                this, SLOT(onCreatedEvent(QString)), Qt::QueuedConnection);
        connect(this, SIGNAL(directoryDeleted(QString)),
                this, SLOT(onDirDeletedEvent(QString)), Qt::QueuedConnection);
        connect(this, SIGNAL(fileCreated(QString)),
                this, SLOT(onCreatedEvent(QString)), Qt::QueuedConnection);
        connect(this, SIGNAL(fileDeleted(QString)),
                this, SLOT(onDeletedEvent(QString)), Qt::QueuedConnection);
        connect(this, SIGNAL(directoryMoved(QString,QString)),
                this, SLOT(onMovedDirEvent(QString,QString)), Qt::QueuedConnection);
        connect(this, SIGNAL(fileMoved(QString,QString)),
                this, SLOT(onMovedEvent(QString,QString)), Qt::QueuedConnection);
        watchDirectory(path);
        start();

    }
signals:
    void genericEvent(QString, QString);
    void genericEvent(QString, QString, QString);

public slots:
    void onMovedEvent(QString from, QString to)
    {
        emit genericEvent("directoryMoved", from, to);
    }

    void onMovedDirEvent(QString from, QString to)
    {

        emit genericEvent("fileMoved", from, to);
    }
    void onCreatedEvent(QString path)
    {
        emit genericEvent("fileCreated", path);
    }

    void onDirCreatedEvent(QString path)
    {
        emit genericEvent("dirCreated", path);
    }

    void onDeletedEvent(QString path)
    {
        emit genericEvent("fileDeleted", path);
    }

    void onDirDeletedEvent(QString path)
    {
        emit genericEvent("dirDeleted", path);
    }
};


class PyQBetterFileWatcher: public QObject
{
    Q_OBJECT
public slots:
    Watcher* new_Watcher(QString path){ return new Watcher(path); }
    void delete_Watcher(Watcher* o) { delete o; }

};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PythonQtObjectPtr ctx = Python::PythonContext::getContext();
    User::Views::PythonQtScriptingConsole console(NULL, ctx);
    console.show();
    PythonQt::self()->addDecorators(new PyQBetterFileWatcher());
    PythonQt::self()->registerClass(&Watcher::staticMetaObject, "QWatcher");
    ctx.evalFile(":/TestBase.py");

    return app.exec();
}
#include "main.moc"
