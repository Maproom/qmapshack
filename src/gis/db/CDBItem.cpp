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

#include "gis/CGisListDB.h"
#include "gis/CGisWidget.h"
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
    query.prepare("SELECT type, keyqms, icon, name, comment FROM items WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return );
    if(query.next())
    {
        QPixmap pixmap;
        type = query.value(0).toInt();
        key  = query.value(1).toString();
        pixmap.loadFromData(query.value(2).toByteArray(), "PNG");
        setIcon(CGisListDB::eColumnCheckbox, pixmap);
        setText(CGisListDB::eColumnName, query.value(3).toString());

        // limit comment to 200 characters
        QString comment = query.value(4).toString();
        if(comment.size() > 200)
        {
            comment = comment.left(197) + "...";
        }
        setToolTip(CGisListDB::eColumnName, comment);
    }

    updateAge();
}


void CDBItem::updateAge()
{
    QSqlQuery query(db);
    query.prepare("SELECT trash FROM items WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return );
    if(!query.next())
    {
        return;
    }

    if(parent()->type() == IDBFolder::eTypeLostFound)
    {
        QString date = query.value(0).toString();
        QDateTime timestamp;

        // The time format can differ by database type
        if(date.contains('T'))
        {
            timestamp = QDateTime::fromString(date,"yyyy-MM-ddThh:mm:ss");
        }
        else
        {
            timestamp = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
        }

        if(timestamp.isValid())
        {
            quint64 diff = QDateTime::currentDateTimeUtc().toTime_t() - timestamp.toTime_t();
            if(diff < (60*60))
            {
                setText(CGisListDB::eColumnTime, tr("%1 min.").arg(diff/60));
            }
            else if(diff < (60*60*24))
            {
                setText(CGisListDB::eColumnTime, tr("%1 h").arg(diff/(60*60)));
            }
            else
            {
                setText(CGisListDB::eColumnTime, tr("%1 days").arg(diff/(60*60*24)));
            }
        }
    }
}

void CDBItem::toggle()
{
    IDBFolder * folder = dynamic_cast<IDBFolder*>(parent());
    if(nullptr == folder)
    {
        return;
    }

    if(checkState(CGisListDB::eColumnCheckbox) == Qt::Checked)
    {
        // make sure the project is shown on the workspace
        CEvtD2WShowFolder * evt1 = new CEvtD2WShowFolder(folder->getId(), folder->getDBName());
        CGisWidget::self().postEventForWks(evt1);

        // tell the project to load the item from the database
        CEvtD2WShowItems * evt2 = new CEvtD2WShowItems(folder->getId(), folder->getDBName());
        evt2->items << evt_item_t(id, type);
        CGisWidget::self().postEventForWks(evt2);
    }
    else
    {
        // tell the project to remove the item
        CEvtD2WHideItems * evt2 = new CEvtD2WHideItems(folder->getId(), folder->getDBName());
        evt2->keys << key;
        CGisWidget::self().postEventForWks(evt2);
    }
}

void CDBItem::remove()
{
    IDBFolder * folder = dynamic_cast<IDBFolder*>(parent());
    if(nullptr == folder)
    {
        return;
    }

    if(checkState(CGisListDB::eColumnCheckbox) == Qt::Checked)
    {
        CEvtD2WHideItems * evt = new CEvtD2WHideItems(folder->getId(), folder->getDBName());
        evt->keys << key;
        CGisWidget::self().postEventForWks(evt);
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM folder2item WHERE parent=:parent AND child=:child");
    query.bindValue(":parent", folder->getId());
    query.bindValue(":child", id);
    QUERY_EXEC();
}
