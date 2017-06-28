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

#include "helpers/CToolBarConfig.h"
#include "CSettings.h"
#include "CMainWindow.h"
#include <QDebug>

const QStringList CToolBarConfig::actionNames = {
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
    "actionToggleMaps",
    "actionToggleDem",
    "actionToggleGis",
    "actionToggleRte"
};

const QStringList CToolBarConfig::defaultActionNames = {
    "actionSearchGoogle",
    "actionAddEmptyProject",
    "actionLoadGISData",
    "actionSaveGISData",
    "actionShowScale",
    "actionShowGrid",
    "actionPOIText",
    "actionNightDay",
    "actionMapToolTip",
    "actionProfileIsWindow",
    "actionSetupToolbar",
    "actionToggleMaps",
    "actionToggleDem",
    "actionToggleGis",
    "actionToggleRte"
};

CToolBarConfig::CToolBarConfig(QWidget * parent, QToolBar * toolBar) : QObject(parent), toolBar(toolBar)
{
    SETTINGS;
    cfg.beginGroup("ToolBar");
    QStringList actions = cfg.value("actions").toStringList();
    cfg.endGroup();
    if (actions.isEmpty())
    {
        setDefaultConfiguredActions();
    }
    else
    {
        setConfiguredActionsByName(actions);
    }
}

CToolBarConfig::~CToolBarConfig()
{
    SETTINGS;
    cfg.beginGroup("ToolBar");
    cfg.setValue("actions",configuredActionNames);
    cfg.endGroup();
}

QList<QAction *> CToolBarConfig::availableActions() const
{
    QList<QAction *> returnActions;

    for (const QString & name : actionNames)
    {
        QAction * action = getActionByName(name);
        if (action != nullptr)
        {
            returnActions << action;
        }
    }
    return returnActions;
}

QList<QAction *> CToolBarConfig::configuredActions() const
{
    QList<QAction *> returnActions;
    for (const QString & name : configuredActionNames)
    {
        QAction * action = getActionByName(name);
        if (action != nullptr)
        {
            returnActions << action;
        }
    }
    return returnActions;
}

void CToolBarConfig::setConfiguredActionsByName(const QStringList & names)
{
    toolBar->clear();
    configuredActionNames.clear();
    for (const QString & name : names)
    {
        QAction * action = getActionByName(name);
        if (action != nullptr)
        {
            configuredActionNames << name;
            toolBar->addAction(action);
        }
    }
}

void CToolBarConfig::setDefaultConfiguredActions()
{
    setConfiguredActionsByName(defaultActionNames);
}

QAction * CToolBarConfig::getActionByName(const QString & name) const
{
    QAction * action = CMainWindow::self().findChild<QAction *>(name);
    if (action == nullptr)
    {
        qWarning() << "error toolbar:" << name << "is not a valid action";
    }
    return action;
}
