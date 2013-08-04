QBetterFileWatcher
==================

An improved FileWatcher for Qt4 that works on Windows, Linux and Mac.

The oficial QFileSystemWatcher lacks lots of signals, providing just when a directory was changed, but nothing else.
this projects aims to provide a complete Crossplatform File System Watcher for Qt4


How to use:
-----------

* Compile it.

```sh
qmake && make
```

* Include it to your .pro file

```pro
include(QBetterFileWatcher.prf)
```

- main.cpp

```cpp
/*Example of Usage*/

#include <QCoreApplication>
#include <QObject>

#include <QBetterFileWatcher>

class MyWatcher: public QObject 
{
  Q_OBJECT
  QBetterFileWatcher m_myWatcher;
  
public:
   MyWatcher()
  {
    m_myWatcher.watchDirectory("path-to-directory");
    connect(m_myWatcher, SIGNAL(fileCreated(QString)), this, SLOT(onFileCreated(QString)));
    m_myWatcher.start();
  }

slots:
  void onFileCreated(QString filePath)
  {
    qDebug() << "the file: " << filePath << "was created.";
  }

};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MyWatcher watcher;
    return a.exec();
}


/*
  because it is a class itself in cpp, we need to include the filename.moc to metaobject generation of qt,
  if you're creating using headers don't use it.
*/
#include "main.moc"


```
