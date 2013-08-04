#pragma once

#include <QObject>
#include <QMap>
#include <QDir>
#include <QUuid>
#include <QDateTime>
#include <QTimer>
#include "TestBase.h"

#include <QBetterFileWatcher>

bool rmTree(QString completePath);

class CreateFileTestCase : public TestBase
{
    Q_OBJECT
    QString m_tempPath;
    QBetterFileWatcher* m_fwatcher;
    bool m_running;
    QTimer timeout;

    void createTempDirectory();
protected:
    void createRandomFile(int size=1024);


public:
    QMap<QString, quint64> selfEvents;
    QMap<QString, quint64> deltaEvents;
    int createEvents;
    CreateFileTestCase();
    void setUp();
    void tearDown();
    void runTests();
public slots:
    void onFileCreated(QString filepath);
    void stopTest();

};
