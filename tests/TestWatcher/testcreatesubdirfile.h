#pragma once

#include <QObject>
#include <QMap>
#include <QDir>
#include <QUuid>
#include <QDateTime>
#include <QTimer>
#include "testbase.h"

#include <QBetterFileWatcher>


class CreateSubdirFileTestCase : public TestBase
{
    Q_OBJECT
    QBetterFileWatcher* m_fwatcher;
    bool m_running;
    QTimer timeout;


public:
    QMap<QString, quint64> deltaEvents;
    int createEvents;
    CreateSubdirFileTestCase();
    void setUp();
    void tearDown();
    void runTest();
public slots:
    void onFileCreated(QString filepath);
    void stopTest();

};
