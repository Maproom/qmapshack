/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/CGisListDB.h"
#include "gis/db/CDBFolderGroup.h"
#include "gis/db/CDBFolderOther.h"
#include "gis/db/CDBFolderProject.h"
#include "gis/db/CDBItem.h"
#include "gis/db/CSearchDatabase.h"
#include "gis/db/IDBFolder.h"
#include "gis/db/macros.h"

#include <QtSql>
#include <QtWidgets>

CSearchDatabase::CSearchDatabase(IDBFolder &dbFolder, CGisListDB *parent)
    : QDialog(parent)
    , dbFolder(dbFolder)
{
    setupUi(this);

    labelName->setText(tr("Search database '%1':").arg(dbFolder.getDBName()));

    connect(pushSearch, &QPushButton::clicked, this, &CSearchDatabase::slotSearch);
    connect(pushClose, &QPushButton::clicked, this, &CSearchDatabase::accept);
    connect(treeResult, &QTreeWidget::itemChanged, this, &CSearchDatabase::slotItemChanged);
}

void CSearchDatabase::slotItemChanged(QTreeWidgetItem * item, int column)
{

    if((column != CGisListDB::eColumnCheckbox) || internalEdit)
    {
        return;
    }

    IDBFolder * folder = dynamic_cast<IDBFolder*>(item);
    if(folder != nullptr)
    {
        const int N = folder->childCount();
        for(int i = 0; i < N; i++)
        {
            IDBFolder * childFolder = dynamic_cast<IDBFolder*>(folder->child(i));
            if(childFolder != nullptr)
            {
                childFolder->setCheckState(CGisListDB::eColumnCheckbox, Qt::Checked);
            }

            CDBItem * childItem = dynamic_cast<CDBItem*>(folder->child(i));
            if(childItem != nullptr)
            {
                childItem->setCheckState(CGisListDB::eColumnCheckbox, Qt::Checked);
            }
        }
    }
    else
    {
        emit sigItemChanged(item, column);
    }
}

void CSearchDatabase::slotSearch()
{
    internalEdit = true;

    treeResult->clear();

    QSqlDatabase& db = dbFolder.getDb();
    QSqlQuery query(db);
    dbFolder.search(lineQuery->text(), query);

    QMap<quint64, IDBFolder*> folders;

    while(query.next())
    {
        quint64 itemId = query.value(0).toULongLong();

        QSqlQuery query2(db);
        query2.prepare("SELECT t1.id, t1.type FROM folders AS t1 WHERE id=(SELECT parent FROM folder2item WHERE child=:id)");
        query2.bindValue(":id", itemId);
        if(!query2.exec())
        {
            qWarning() << query2.lastQuery();
            qWarning() << query2.lastError();
            continue;
        }

        while(query2.next())
        {
            quint64 folderId = query2.value(0).toULongLong();
            quint32 type = query2.value(1).toUInt();

            IDBFolder * folder = nullptr;

            if(!folders.contains(folderId))
            {
                switch(type)
                {
                case IDBFolder::eTypeProject:
                    folder = new CDBFolderProject(db, folderId, 0);
                    break;

                case IDBFolder::eTypeOther:
                    folder = new CDBFolderOther(db, folderId, 0);
                    break;

                default:
                    continue;
                }

                folders[folderId] = folder;

                addWithParentFolders(treeResult, folder, folders, db);
            }
            else
            {
                folder = folders[folderId];
            }

            CDBItem * item = new CDBItem(db, itemId, folder);
            item->setCheckState(CGisListDB::eColumnCheckbox, Qt::Unchecked);
        }
    }

    treeResult->expandAll();
    treeResult->header()->resizeSections(QHeaderView::ResizeToContents);

    internalEdit = false;
}

void CSearchDatabase::addWithParentFolders(QTreeWidget * result, IDBFolder * child, QMap<quint64, IDBFolder*>& folders, QSqlDatabase& db)
{
    QSqlQuery query(db);

    query.prepare("SELECT t1.id, t1.type FROM folders AS t1 WHERE id=(SELECT parent FROM folder2folder WHERE child=:id)");
    query.bindValue(":id", child->getId());
    QUERY_EXEC(return );

    while(query.next())
    {
        quint64 folderId = query.value(0).toULongLong();
        quint32 type = query.value(1).toUInt();

        if(!folders.contains(folderId))
        {
            IDBFolder * folder = nullptr;

            switch(type)
            {
            case IDBFolder::eTypeProject:
                folder = new CDBFolderProject(db, folderId, 0);
                break;

            case IDBFolder::eTypeOther:
                folder = new CDBFolderOther(db, folderId, 0);
                break;

            case IDBFolder::eTypeGroup:
                folder = new CDBFolderGroup(db, folderId, 0);
                break;

            default:
                ;
            }

            if(folder != nullptr)
            {
                folders[folderId] = folder;
                folder->addChild(child);
                addWithParentFolders(result, folder, folders, db);
            }
            else
            {
                result->addTopLevelItem(child);
            }
        }
        else
        {
            folders[folderId]->addChild(child);
        }
    }
}
