/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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
#include "gis/db/CDBFolderSqlite.h"
#include "gis/db/CDBFolderProject.h"
#include "gis/db/CDBFolderOther.h"
#include "gis/db/CDBItem.h"
#include "gis/db/macros.h"

#include <QtWidgets>
#include <QtSql>

CDBFolderSqlite::CDBFolderSqlite(const QString& filename, const QString& name, QTreeWidget *parent)
    : IDBFolderSql(IDB::db, parent)
    , filename(filename)
{
    setToolTip(CGisListDB::eColumnName, tr("All your data grouped by folders."));

    setText(CGisListDB::eColumnName, name);

    if(setupDB(filename, name, error))
    {
        setupFromDB();

        setIcon(CGisListDB::eColumnCheckbox, QIcon("://icons/32x32/SQLite.png"));
        setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    }
    else
    {
        IDB::db.close();

        setIcon(CGisListDB::eColumnCheckbox, QIcon("://icons/32x32/SQLiteNoConn.png"));
    }
}

QString CDBFolderSqlite::getDBInfo() const
{
    QString str = "<div style='font-weight: bold;'>" + IDB::db.connectionName() + "</div><br />";
    str += tr("SQLite Database") + "<br />";

    QString path = IDB::db.databaseName();
    #ifndef Q_OS_WIN
    if(path.startsWith(QDir::homePath()))
    {
        path = "~" + path.remove(0, QDir::homePath().length());
    }
    #endif

    str += tr("File: ") + QString("<i>%1</i>").arg(path);

    if(!isUsable())
    {
        str += "<br />" + tr("Error: ") + QString("<span style=\"color:#f00; font-weight:bold;\">%1</span>").arg(error);
    }

    return str;
}

void CDBFolderSqlite::search(const QString& str, QTreeWidget * result)
{
    QSqlQuery query(IDB::db);
    query.prepare("SELECT id FROM searchindex WHERE comment MATCH :str");
    query.bindValue(":str", str);
    QUERY_EXEC(return );

    QMap<quint64, IDBFolder*> folders;

    while(query.next())
    {
        quint64 itemId = query.value(0).toULongLong();

        QSqlQuery query2(IDB::db);
        query2.prepare("SELECT t1.id, t1.type, t1.name FROM folders AS t1 WHERE id=(SELECT parent FROM folder2item WHERE child=:id)");
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
            QString name = query2.value(2).toString();

            IDBFolder * folder = nullptr;

            if(!folders.contains(folderId))
            {
                switch(type)
                {
                case IDBFolder::eTypeProject:
                    folder = new CDBFolderProject(IDB::db, folderId, 0);
                    break;
                case IDBFolder::eTypeOther:
                    folder = new CDBFolderOther(IDB::db, folderId, 0);
                    break;
                default:
                    continue;
                }

                folders[folderId] = folder;
                result->addTopLevelItem(folder);
            }
            else
            {
                folder = folders[folderId];
            }

            new CDBItem(IDB::db, itemId, folder);

        }
    }

    result->header()->resizeSections(QHeaderView::ResizeToContents);
}
