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


    QTimer::singleShot(1, this, SLOT(slotHelpText()));
}

CGisDatabase::~CGisDatabase()
{
    SETTINGS;
    cfg.setValue("Database/treeDB/state", treeDB->header()->saveState());
}

void CGisDatabase::slotHelpText()
{
    bool databaseAvailable = treeDB->topLevelItemCount() != 0;
    frameHelp->setVisible(!databaseAvailable);
    widgetSummary->setVisible(databaseAvailable);
}

void CGisDatabase::postEventForDb(QEvent * event)
{
    QCoreApplication::postEvent(treeDB, event);
}

void CGisDatabase::sendEventForDb(QEvent * event)
{
    QCoreApplication::sendEvent(treeDB, event);
}

IDBFolder * CGisDatabase::getFolderById(quint64 id, const QString& db)
{
    const int N = treeDB->topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        IDBFolder * dbfolder = dynamic_cast<IDBFolder*>(treeDB->topLevelItem(n));
        if((dbfolder == nullptr) || (dbfolder->getDBName() != db))
        {
            continue;
        }

        return IDBFolder::createFolderByType(dbfolder->getDb(), IDBFolder::eTypeProject, id, nullptr);
    }

    return nullptr;
}
