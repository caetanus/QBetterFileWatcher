#pragma once
#include <QDir>
#include <QBetterFileWatcher>
#include <QTimer>
#include <QDateTime>
#include "testbase.h"

class DeleteDirectoriesTestCase : public TestBase
{
    Q_OBJECT
    QBetterFileWatcher* m_fwatcher;
    bool m_running;
    QTimer timeout;
public:
    QMap<QString, quint64> deltaEvents;
    int delEvents;
    explicit DeleteDirectoriesTestCase();
    void setUp();
    void tearDown();
    void runTest();
    
signals:
    
public slots:
   void onDirDeleted(QString path);
   void stopTest();
};

