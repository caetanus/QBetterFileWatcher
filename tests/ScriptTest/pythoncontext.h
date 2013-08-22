#ifndef PYTHONCONTEXT_H
#define PYTHONCONTEXT_H

#include <QObject>
#include <PythonQt.h>
#include <PythonQtConversion.h>

namespace Python {

class PythonContext : public QObject
{
    Q_OBJECT
    static bool m_pythonEnvironLoaded;
    static PythonQtObjectPtr m_ctx;
    
    PythonContext(QWidget *parent = 0);
public:
    static PythonQtObjectPtr getContext();

    static bool moduleExists(QString moduleName);

    static QString repr(PythonQtObjectPtr obj);

    static bool isModule(QString variable);

    static PythonQtObjectPtr getFullQualifiedNameVar(PythonQtObjectPtr ctx, QString FQN);


    static PythonQtObjectPtr getFullQualifiedNameVar(QString FQN);


};

}
#endif // PYTHONCONTEXT_H
