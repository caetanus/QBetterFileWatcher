#pragma once

#include <QObject>
#include <QMap>
#include <QDir>
#include <QUuid>
#include <QDateTime>
#include <QTimer>
#include "testbase.h"

#include <QBetterFileWatcher>


class CreateSubDirectoriesTestCase : public TestBase
{
    Q_OBJECT
    QBetterFileWatcher* m_fwatcher;
    bool m_running;
    QTimer timeout;
    QStringList m_parentDirs;

public:
    QMap<QString, quint64> deltaEvents;
    int createEvents;
    CreateSubDirectoriesTestCase();
    void setUp();
    void tearDown();
    void runTest();
public slots:
    void createSubDir();
    void onDirCreated(QString filepath);
    void stopTest();

};
