#include "CPluginConsole.h"
#include "gis/CGisWorkspace.h"
#include "plugin/wrappers/CGisWorkspaceWrapper.h"
#include "plugin/wrappers/IGisProjectWrapper.h"

CPluginConsole::CPluginConsole(QWidget* parent, const PythonQtObjectPtr& context, Qt::WindowFlags i) : PythonQtScriptingConsole(parent, context, i)
{
    //PythonQt::self()->registerClass( &Test::staticMetaObject, "example" );
    PythonQt::self()->registerCPPClass("IGisProject");
    PythonQt::self()->addDecorators( new CGisWorkspaceWrapper());
    PythonQt::self()->addDecorators( new IGisProjectWrapper());
    PythonQt::self()->addObject(context.object(), "Workspace", &CGisWorkspace::self());

    appendCommandPrompt();
}
