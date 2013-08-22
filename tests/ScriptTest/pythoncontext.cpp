#include "pythoncontext.h"

using namespace Python;
PythonQtObjectPtr PythonContext::m_ctx = (PythonQt::init(PythonQt::RedirectStdOut),
                                         PythonQt::self()->getMainModule());
bool PythonContext::m_pythonEnvironLoaded = false;

PythonQtObjectPtr PythonContext::getContext()
{
    if (!m_pythonEnvironLoaded) {
        m_ctx = PythonQt::self()->getMainModule();
        PythonQt::self()->setImporter(NULL); //used to set qrc as import module path files starting with ':'
        PythonQtObjectPtr unique_module = PythonQt::self()->createUniqueModule();
        unique_module.evalScript("# -*- coding: utf-8 -*-\n"
                                 "import sys\n"
                                 "syspath = sys.path\n"
                                 "import os\n"
                                 "sysstdout = sys.stdout\n"
                                 "sysstderr = sys.stderr\n"
                                 "reload(sys)\n"
                                 "sys.setdefaultencoding('utf-8')\n"
                                 "sys.stdout = sysstdout\n"
                                 "sys.stderr = sysstderr\n"
                                 "sys.path = syspath\n"
                                 );

        m_pythonEnvironLoaded = true;
    }
    return m_ctx;
}

bool PythonContext::moduleExists(QString moduleName)
{

    getContext().evalScript(QString("__tmp__value = '%1' in globals()").arg(moduleName));
    bool exists = m_ctx.getVariable("__tmp_value").toBool();
    if (exists)
        return isModule(moduleName);

    return false;
}

QString PythonContext::repr(PythonQtObjectPtr obj)
{
    return PythonQtConv::PyObjGetRepresentation(obj.object());
}

bool PythonContext::isModule(QString variable)
{
    getContext().evalScript("from types import ModuleType");
    getContext().evalScript(QString("__tmp__value = type(globals()['%1']) is ModuleType").arg(variable));
    return getContext().getVariable("__tmp_value").toBool();
}

PythonQtObjectPtr PythonContext::getFullQualifiedNameVar(PythonQtObjectPtr ctx, QString FQN)
{
    foreach(QString var, FQN.split("."))
    {
        ctx = ctx.getVariable(var);
    }
    return ctx;
}

PythonQtObjectPtr PythonContext::getFullQualifiedNameVar(QString FQN)
{
    return getFullQualifiedNameVar(getContext(), FQN);

}
