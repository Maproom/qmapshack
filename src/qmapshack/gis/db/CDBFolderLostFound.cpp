/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "gis/CGisWorkspace.h"
#include "gis/db/CDBFolderLostFound.h"
#include "gis/db/CDBItem.h"
#include "gis/db/macros.h"

#include <QtSql>

CDBFolderLostFound::CDBFolderLostFound(QSqlDatabase& db, QTreeWidgetItem *parent)
    : IDBFolder(true, db, eTypeLostFound, 0, parent)
{
    setToolTip(CGisListDB::eColumnName, tr("All your data grouped by folders."));
    CDBFolderLostFound::setupFromDB();

    setCheckState(CGisListDB::eColumnCheckbox, Qt::Unchecked);
    CEvtD2WReqInfo * evt = new CEvtD2WReqInfo(getId(), getDBName());
    CGisWorkspace::self().postEventForWks(evt);
}

CDBFolderLostFound::~CDBFolderLostFound()
{
}

void CDBFolderLostFound::setupFromDB()
{
    int cnt = 0;
    QSqlQuery query(db);

    qDeleteAll(takeChildren());

    QUERY_RUN("SELECT id FROM items AS t1 WHERE NOT EXISTS(SELECT * FROM folder2item WHERE child=t1.id) ORDER BY t1.type, t1.name", return );
    while(query.next())
    {
        quint64 id = query.value(0).toULongLong();
        new CDBItem(db, id, this);
        cnt++;
    }

    if(cnt)
    {
        setText(CGisListDB::eColumnName, tr("Lost & Found (%1)").arg(cnt));
        setIcon(CGisListDB::eColumnCheckbox, QIcon("://icons/32x32/DeleteMultiple.png"));
    }
    else
    {
        setText(CGisListDB::eColumnName, tr("Lost & Found"));
        setIcon(CGisListDB::eColumnCheckbox, QIcon("://icons/32x32/Empty.png"));
    }

    CEvtD2WUpdateLnF * evt = new CEvtD2WUpdateLnF(getId(), getDBName());
    CGisWorkspace::self().postEventForWks(evt);
}

void CDBFolderLostFound::update(CEvtW2DAckInfo * info)
{
    if(info->id != 0)
    {
        return;
    }
    setCheckState(CGisListDB::eColumnCheckbox, info->checkState);
}

bool CDBFolderLostFound::update()
{
    setupFromDB();
    return true;
}

void CDBFolderLostFound::expanding()
{
    const int N = childCount();
    for(int i = 0; i < N; i++)
    {
        CDBItem * item = dynamic_cast<CDBItem*>(child(i));
        if(item)
        {
            item->updateAge();
        }
    }
}

void CDBFolderLostFound::clear()
{
    QSqlQuery query(db);

    QUERY_RUN("DELETE FROM items WHERE id NOT IN (SELECT child from folder2item)", return )

    setupFromDB();
}

bool CDBFolderLostFound::delItem(CDBItem * item)
{
    QSqlQuery query(db);

    if(checkState(CGisListDB::eColumnCheckbox) == Qt::Checked)
    {
        CEvtD2WHideItems * evt = new CEvtD2WHideItems(getId(), getDBName());
        evt->keys << item->getKey();
        CGisWorkspace::self().postEventForWks(evt);
    }

    query.prepare("DELETE FROM items WHERE id=:id AND id NOT IN (SELECT child from folder2item)");
    query.bindValue(":id", item->getId());
    QUERY_EXEC(return false);

    return true;
}
