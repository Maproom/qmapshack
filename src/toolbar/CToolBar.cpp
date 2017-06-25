/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/

#include "CToolBar.h"
#include "helpers/CSettings.h"
#include "CMainWindow.h"
#include <QDebug>

const QStringList CToolBar::actionNames = {
    "actionAddMapView",
    "actionShowScale",
    "actionSetupMapFont",
    "actionShowGrid",
    "actionSetupGrid",
    "actionFlipMouseWheel",
    "actionSetupMapPaths",
    "actionPOIText",
    "actionNightDay",
    "actionMapToolTip",
    "actionSetupDEMPaths",
    "actionAbout",
    "actionHelp",
    "actionSetupMapView",
    "actionLoadGISData",
    "actionSaveGISData",
    "actionSetupTimeZone",
    "actionAddEmptyProject",
    "actionSearchGoogle",
    "actionCloseAllProjects",
    "actionSetupUnits",
    "actionSetupWorkspace",
    "actionImportDatabase",
    "actionVrtBuilder",
    "actionStoreView",
    "actionLoadView",
    "actionProfileIsWindow",
    "actionClose",
    "actionCloneMapView",
    "actionCreateRoutinoDatabase",
    "actionPrintMap",
    "actionSetupCoordFormat",
    "actionSetupMapBackground",
    "actionSetupWaypointIcons",
    "actionCloseTab",
    "actionQuickstart",
    "actionSetupToolbar",
    "actionToggleDocks"
};

CToolBar::CToolBar(QWidget * parent) : QToolBar(parent)
{
    SETTINGS;
    cfg.beginGroup("ToolBar");
    QString indicesStr = cfg.value("actions").toString();
    cfg.endGroup();
    if (!indicesStr.isEmpty())
    {
        actionIndices = indicesStr.split(",");
        for (const QString & indexStr : actionIndices)
        {
            const int index = indexStr.toInt();
            if (index >= 0 && index < actionNames.size())
            {
                QAction * action = CMainWindow::self().findChild<QAction *>(actionNames[index]);
                if (action != nullptr)
                {
                    QWidget::addAction(action);
                }
                else
                {
                    qWarning() << "error parsing config [ToolBar]->actions:" << actionNames[index] << "is not a valid action";
                }
            }
            else
            {
                qWarning() << "error parsing config [ToolBar]->actions:" << indexStr << "is out of range";
            }
        }
    }
}

CToolBar::~CToolBar()
{
    SETTINGS;
    cfg.beginGroup("ToolBar");
    cfg.setValue("actions",actionIndices.join(","));
    cfg.endGroup();
}

void CToolBar::clear()
{
    actionIndices.clear();
    QToolBar::clear();
}

void CToolBar::addAction(QAction * action)
{
    QString actionName = action->objectName();
    int index = actionNames.indexOf(actionName);
    if (index >= 0)
    {
        actionIndices << QString("%1").arg(index);
    }
    QWidget::addAction(action);
}
