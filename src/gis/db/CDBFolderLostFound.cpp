/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/db/CDBFolderLostFound.h"
#include "gis/db/CDBItem.h"
#include "gis/db/macros.h"

#include <QtSql>

CDBFolderLostFound::CDBFolderLostFound(QSqlDatabase& db, QTreeWidgetItem *parent)
    : IDBFolder(false, db, eTypeLostFound, 0, parent)
{
    setToolTip(eColumnName, QObject::tr("All your data grouped by folders."));    
    setText(eColumnName, QObject::tr("Lost & Found"));

    update();
}

CDBFolderLostFound::~CDBFolderLostFound()
{

}

void CDBFolderLostFound::update()
{
    int cnt = 0;
    QSqlQuery query(db);

    qDeleteAll(takeChildren());

    query.prepare("SELECT id FROM items AS t1 WHERE NOT EXISTS(SELECT * FROM folder2item WHERE child=t1.id) ORDER BY t1.type, t1.name");
    QUERY_EXEC(return);

    while(query.next())
    {
        quint64 id = query.value(0).toULongLong();
        new CDBItem(db, id, this);
        cnt++;
    }

    if(cnt)
    {
        setText(eColumnName, QObject::tr("Lost & Found (%1)").arg(cnt));
        setIcon(eColumnCheckbox, QIcon("://icons/32x32/DeleteMultiple.png"));
    }
    else
    {
        setText(eColumnName, QObject::tr("Lost & Found"));
        setIcon(eColumnCheckbox, QIcon("://icons/32x32/Empty.png"));
    }
}

void CDBFolderLostFound::clear()
{
    QSqlQuery query(db);

    query.prepare("DELETE FROM items WHERE id NOT IN (SELECT child from folder2item)");
    QUERY_EXEC(return);

    update();
}
