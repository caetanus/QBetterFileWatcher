#pragma once

#include <QObject>
#include <QMap>
#include <QDir>
#include <QUuid>
#include <QDateTime>
#include <QTimer>
#include "testbase.h"

#include <QBetterFileWatcher>


class CreateDirectoriesTestCase : public TestBase
{
    Q_OBJECT
    QBetterFileWatcher* m_fwatcher;
    bool m_running;
    QTimer timeout;


public:
    QMap<QString, quint64> deltaEvents;
    int createEvents;
    CreateDirectoriesTestCase();
    void setUp();
    void tearDown();
    void runTest();
public slots:
    void onDirCreated(QString filepath);
    void stopTest();

};
