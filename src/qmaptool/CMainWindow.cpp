/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler oliver.eichler@gmx.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#include "CAbout.h"
#include "CMainWindow.h"
#include "help/CHelp.h"
#include "helpers/CSettings.h"
#include "setup/CSetupExtTools.h"
#include "setup/IAppSetup.h"
#include "tool/CToolAddOverview.h"
#include "tool/CToolBox.h"
#include "tool/CToolCutMap.h"
#include "tool/CToolExport.h"
#include "tool/CToolGrid.h"
#include "tool/CToolPalettize.h"
#include "tool/CToolRefMap.h"
#include "units/CCoordFormatSetup.h"
#include "units/CUnitsSetup.h"
#include "units/IUnit.h"
#include "version.h"

CMainWindow * CMainWindow::pSelf = nullptr;

CMainWindow::CMainWindow()
{
    SETTINGS;
    IUnit::setUnitType((IUnit::type_e)cfg.value("Units/units", IUnit::eTypeMetric).toInt(), this);
    IUnit::setCoordFormat((IUnit::coord_format_e)cfg.value("Units/coordFormat", IUnit::eCoordFormat1).toInt());

    pSelf = this;
    setupUi(this);
    setWindowTitle(WHAT_STR);

    canvas->setToolInterface(toolStack);

    connect(actionAbout, &QAction::triggered, this, &CMainWindow::slotAbout);
    connect(actionSetupExtTools, &QAction::triggered, this, &CMainWindow::slotSetupExtTools);
    connect(actionSetupUnits, &QAction::triggered, this, &CMainWindow::slotSetupUnits);
    connect(actionSetupCoordFormat, &QAction::triggered, this, &CMainWindow::slotSetupCoordFormat);
    connect(actionHelp, &QAction::triggered, this, &CMainWindow::slotHelp);
    connect(&IAppSetup::self(), &IAppSetup::sigSetupChanged, this, &CMainWindow::slotSetupChanged);

    menuWindow->addAction(dockTools->toggleViewAction());
    menuWindow->addAction(dockShell->toggleViewAction());
    prepareMenuForMac();

    toolBox = new CToolBox(this);
    toolStack->addWidget(toolBox);

    toolAddOverview = new CToolAddOverview(toolBox);
    toolBox->addItem(toolAddOverview, QIcon("://icons/32x32/AddOverview.png"), toolAddOverview->objectName());

    toolCutMap = new CToolCutMap(toolBox);
    toolBox->addItem(toolCutMap, QIcon("://icons/32x32/CutMap.png"), toolCutMap->objectName());

    toolRefMap = new CToolRefMap(toolBox);
    toolBox->addItem(toolRefMap, QIcon("://icons/32x32/ReferenceMap.png"), toolRefMap->objectName());

    toolPalettize = new CToolPalettize(toolBox);
    toolBox->addItem(toolPalettize, QIcon("://icons/32x32/Rasterize.png"), toolPalettize->objectName());

    toolExport = new CToolExport(toolBox);
    toolBox->addItem(toolExport, QIcon("://icons/32x32/Export.png"), toolExport->objectName());

    toolGrid = new CToolGrid(this);
    toolStack->addWidget(toolGrid);

    // start ---- restore window geometry -----
    if ( cfg.contains("MainWindow/geometry"))
    {
        restoreGeometry(cfg.value("MainWindow/geometry").toByteArray());
    }
    else
    {
        QTimer::singleShot(500, this, SLOT(showMaximized()));
    }

    if ( cfg.contains("MainWindow/state"))
    {
        restoreState(cfg.value("MainWindow/state").toByteArray());
    }
    // end ---- restore window geometry -----
    //toolStack->setCurrentIndex(cfg.value("Tool/Stack/current",0).toInt());
    toolBox->setCurrentIndex(cfg.value("Tool/Box/current", 0).toInt());
    actionShowToolHelp->setChecked(cfg.value("Tool/showHelp", true).toBool());
    mapFont = cfg.value("Canvas/mapFont", font()).value<QFont>();
    actionFlipMouseWheel->setChecked(cfg.value("Canvas/flipMouseWheel", false).toBool());
}

CMainWindow::~CMainWindow()
{
    SETTINGS;
    cfg.setValue("MainWindow/state", saveState());
    cfg.setValue("MainWindow/geometry", saveGeometry());

    cfg.setValue("Canvas/mapFont", mapFont);
    cfg.setValue("Canvas/flipMouseWheel", actionFlipMouseWheel->isChecked());

    cfg.setValue("Units/units", IUnit::self().type);
    cfg.setValue("Units/coordFormat", IUnit::getCoordFormat());

    cfg.setValue("Tool/Box/current", toolBox->currentIndex());
    cfg.setValue("Tool/showHelp", actionShowToolHelp->isChecked());
}

QString CMainWindow::getUser()
{
    QString user = getenv("USER");
    if(user.isEmpty())
    {
        user = getenv("USERNAME"); //for windows

        if(user.isEmpty())
        {
            user = "QMapTool";
        }
    }

    return user;
}

void CMainWindow::prepareMenuForMac()
{
    dockTools->toggleViewAction()->setMenuRole(QAction::NoRole);
}

void CMainWindow::makeShellVisible()
{
    dockShell->show();
}

void CMainWindow::startGridTool(CItemRefMap *item)
{
    toolGrid->registerItem(item);
    toolStack->setCurrentWidget(toolGrid);
}

void CMainWindow::showToolBox()
{
    toolStack->setCurrentWidget(toolBox);
}

void CMainWindow::slotAbout()
{
    CAbout dlg(this);
    dlg.exec();
}

void CMainWindow::slotSetupExtTools()
{
    CSetupExtTools dlg(this);
    dlg.exec();
}

void CMainWindow::slotSetupUnits()
{
    CUnitsSetup dlg(this);
    dlg.exec();
}

void CMainWindow::slotSetupCoordFormat()
{
    CCoordFormatSetup dlg(this);
    dlg.exec();
}

void CMainWindow::slotSetupChanged()
{
    toolStack->setupChanged();
}

void CMainWindow::slotHelp()
{
    if(help.isNull())
    {
        help = new CHelp(
            IAppSetup::self().helpFile(),
            "qthelp://qmt/doc/doc/html/QMapTool/QMTDocMain.html",
            this
            );
        addDockWidget(Qt::AllDockWidgetAreas, help);
    }

    help->setVisible(true);
}
