#pragma once

#include <QObject>
#include <QMap>
#include <QDir>
#include <QUuid>
#include <QDateTime>
#include <QTimer>
#include <QPair>

#include <QBetterFileWatcher>

#include "testbase.h"

class MoveFileTestCase : public TestBase
{
    Q_OBJECT
    QBetterFileWatcher* m_fwatcher;
    bool m_running;
    bool m_passed;
    QString m_randomFile;
    QString m_randomFileNew;
    QPair<QString, QString> moveEvent;
    bool m_gotEvent;
    QTimer* m_timeout;


public:
    QMap<QString, quint64> deltaEvents;
    int createEvents;
    MoveFileTestCase();
    void setUp();
    void tearDown();
    void runTest();
public slots:
    void onFileMoved(QString oldFilePath, QString newFilePath);
    void onTimeout();
    void stopTest();

};
