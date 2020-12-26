#ifndef CPLUGINCONSOLE_H
#define CPLUGINCONSOLE_H

#include "gui/PythonQtScriptingConsole.h"
#include "PythonQt.h"

class CPluginConsole : public PythonQtScriptingConsole
{
public:
    CPluginConsole(QWidget* parent, const PythonQtObjectPtr& context, Qt::WindowFlags i = 0);
};

#endif // CPLUGINCONSOLE_H
