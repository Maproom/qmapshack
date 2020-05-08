/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/CGisDatabase.h"
#include "gis/db/IDBFolderSql.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CGisDatabase * CGisDatabase::pSelf = nullptr;

CGisDatabase::CGisDatabase(QWidget *parent)
    : QWidget(parent)
{
    pSelf = this;
    setupUi(this);

    SETTINGS;
    treeDB->header()->restoreState(cfg.value("Database/treeDB/state", treeDB->header()->saveState()).toByteArray());

    connect(treeDB,  &CGisListDB::sigChanged,  this, &CGisDatabase::slotHelpText);
    connect(actionShowSummaryDropZones, &QAction::toggled, widgetSummary, &CGisSummary::setVisible);

    QList<QAction*> actions;
    actions << actionShowSummaryDropZones;
    treeDB->addGlobalActions(actions);

    actionShowSummaryDropZones->setChecked(cfg.value("Database/isSummaryVisible", true).toBool());

    QTimer::singleShot(1, this, SLOT(slotHelpText()));
}

CGisDatabase::~CGisDatabase()
{
    SETTINGS;
    cfg.setValue("Database/treeDB/state", treeDB->header()->saveState());
    cfg.setValue("Database/isSummaryVisible", actionShowSummaryDropZones->isChecked());
}

void CGisDatabase::slotHelpText()
{
    bool databaseAvailable = treeDB->topLevelItemCount() != 0;
    frameHelp->setVisible(!databaseAvailable);
    widgetSummary->setVisible(actionShowSummaryDropZones->isChecked());
}

void CGisDatabase::postEventForDb(QEvent * event)
{
    QCoreApplication::postEvent(treeDB, event);
}

void CGisDatabase::sendEventForDb(QEvent * event)
{
    QCoreApplication::sendEvent(treeDB, event);
}

void CGisDatabase::createDatabase()
{
    treeDB->slotAddDatabase();
}
