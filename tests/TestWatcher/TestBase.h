#pragma once

#include <QObject>

class TestBase : public QObject
{
    Q_OBJECT
public:
    //virtual TestBase(QObject *parent) = 0;
    
signals:
    void testStop();
public slots:
    virtual void stopTest();
};

