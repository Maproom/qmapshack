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

#include "gis/db/CDBItem.h"
#include "gis/db/IDBFolder.h"
#include "gis/db/macros.h"

#include <QtSql>

CDBItem::CDBItem(QSqlDatabase &db, quint64 id, IDBFolder *parent)
    : QTreeWidgetItem(parent)
    , db(db)
    , id(id)
{
    QSqlQuery query(db);
    query.prepare("SELECT type, key, icon, name, comment FROM items WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return);
    if(query.next())
    {
        QPixmap pixmap;
        type = query.value(0).toInt();
        key = query.value(1).toString();
        pixmap.loadFromData(query.value(2).toByteArray(), "PNG");
        setIcon(0, pixmap);
        setText(1, query.value(3).toString());
        setToolTip(1, query.value(4).toString());

    }
}

CDBItem::~CDBItem()
{

}

