#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QDebug>

class AbstractWatcher: public QObject
{
    Q_OBJECT


public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool watchDirectory(QString path) = 0;
    virtual bool unwatchDirectory(QString path) = 0;

protected:
    void debug(QStringList debugInfo);

signals:
#ifdef DEBUG_INFORMATION
    void debugInformation(QStringList data);
#endif
    void failWatchingDirectory(QString path);
    void directoryCreated(QString path);
    void directoryDeleted(QString path);
    void directoryChanged(QString path);
    void directoryMoved(QString oldPath, QString newPath);
    void fileCreated(QString path);
    void fileDeleted(QString path);
    void fileMoved(QString oldPath, QString newPath);
    void fileUpdated(QString path);
};


Q_DECLARE_INTERFACE(AbstractWatcher, "AbstractWatcher/1.0")
