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
#include "gis/IGisItem.h"
#include "gis/db/CDBFolderDatabase.h"
#include "gis/db/CDBFolderGroup.h"
#include "gis/db/CDBFolderOther.h"
#include "gis/db/CDBFolderProject.h"
#include "gis/db/CDBItem.h"
#include "gis/db/IDBFolder.h"
#include "gis/db/macros.h"

#include <QtSql>

IDBFolder::IDBFolder(bool isLoadable, QSqlDatabase& db, type_e type, quint64 id, QTreeWidgetItem *parent)
    : QTreeWidgetItem(parent, type)
    , db(db)
    , id(id)
    , isLoadable(isLoadable)
{
}

IDBFolder::IDBFolder(bool isLoadable, QSqlDatabase& db, type_e type, quint64 id, QTreeWidget * parent)
    : QTreeWidgetItem(parent, type)
    , db(db)
    , id(id)
    , isLoadable(isLoadable)
{
}

IDBFolder::~IDBFolder()
{
}

bool IDBFolder::operator<(const QTreeWidgetItem &other) const
{
    const IDBFolder * folder = dynamic_cast<const IDBFolder*>(&other);
    if(folder == 0)
    {
        return false;
    }


    return text(CGisListDB::eColumnName) < folder->text(CGisListDB::eColumnName);
}

IDBFolder * IDBFolder::createFolderByType(QSqlDatabase& db, int type, quint64 id, QTreeWidgetItem * parent)
{
    switch(type)
    {
    case eTypeGroup:
        return new CDBFolderGroup(db, id, parent);

    case eTypeProject:
        return new CDBFolderProject(db, id, parent);

    case eTypeOther:
        return new CDBFolderOther(db, id, parent);

    default:
        return 0;
    }
}

QString IDBFolder::getDBName()
{
    return db.connectionName();
}

CDBFolderDatabase * IDBFolder::getDBFolder()
{
    if(type() == eTypeDatabase)
    {
        return dynamic_cast<CDBFolderDatabase*>(this);
    }

    IDBFolder * folder = dynamic_cast<IDBFolder*>(parent());
    if(folder != 0)
    {
        return folder->getDBFolder();
    }
    return 0;
}

IDBFolder * IDBFolder::getFolder(quint64 idFolder)
{
    if(id == idFolder)
    {
        return this;
    }

    const int N = childCount();
    for(int n = 0; n < N; n++)
    {
        IDBFolder * folder1 = dynamic_cast<IDBFolder*>(child(n));
        if(folder1 == 0)
        {
            return 0;
        }

        IDBFolder * folder2 = folder1->getFolder(idFolder);
        if(folder2)
        {
            return folder2;
        }
    }

    return 0;
}

quint64 IDBFolder::addFolder(type_e type, const QString& name)
{
    quint64 idChild = IDBFolder::addFolderToDb(type, name, id, db);
    if(idChild != 0)
    {
        IDBFolder::createFolderByType(db, type, idChild, this);
        expanding();
    }
    return idChild;
}

quint64 IDBFolder::addFolderToDb(type_e type, const QString& name, quint64 idParent, QSqlDatabase& db)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO folders (name, type) VALUES (:name, :type)");
    query.bindValue(":name", name);
    query.bindValue(":type", type);
    QUERY_EXEC(return 0);

    query.prepare("SELECT last_insert_rowid() from folders");
    QUERY_EXEC(return 0);
    query.next();
    quint64 idChild = query.value(0).toULongLong();
    if(idChild == 0)
    {
        qDebug() << "CGisListDB::slotAddFolder(): childId equals 0. bad.";
        return 0;
    }

    query.prepare("INSERT INTO folder2folder (parent, child) VALUES (:parent, :child)");
    query.bindValue(":parent", idParent);
    query.bindValue(":child", idChild);
    QUERY_EXEC(return 0);

    return idChild;
}

void IDBFolder::expanding()
{
    qDeleteAll(takeChildren());
    addChildren(QSet<QString>());

    CEvtD2WReqInfo * evt = new CEvtD2WReqInfo(getId(), getDBName());
    CGisWidget::self().postEventForWks(evt);
}

void IDBFolder::update(CEvtW2DAckInfo * info)
{
    if(info->id != id)
    {
        // forward call if not for local ID
        for(int i = 0; i < childCount(); i++)
        {
            IDBFolder * folder = dynamic_cast<IDBFolder*>(child(i));
            if(folder)
            {
                folder->update(info);
            }
        }
        return;
    }

    setCheckState(CGisListDB::eColumnCheckbox, info->isLoaded ? Qt::Checked : Qt::Unchecked);

    QSqlQuery query(db);
    // update text and tooltip
    query.prepare("SELECT name, comment FROM folders WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return );
    query.next();

    setText(CGisListDB::eColumnName, query.value(0).toString());
    setToolTip(CGisListDB::eColumnName, query.value(1).toString());

    // count folders linked to this folder
    query.prepare("SELECT COUNT() FROM folder2folder WHERE parent=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return );
    query.next();

    qint32 nFolders = query.value(0).toInt();

    // count items linked to this folder
    query.prepare("SELECT COUNT() FROM folder2item WHERE parent=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return );
    query.next();

    qint32 nItems = query.value(0).toInt();

    // set indicator according to items
    if(nFolders || nItems)
    {
        setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    }
    else
    {
        setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
    }

    if(isExpanded())
    {
        qDeleteAll(takeChildren());
        addChildren(info->keysChildren);
    }
}

void IDBFolder::toggle()
{
    if(checkState(CGisListDB::eColumnCheckbox) == Qt::Checked)
    {
        CEvtD2WShowFolder * evt1 = new CEvtD2WShowFolder(getId(), getDBName());
        CGisWidget::self().postEventForWks(evt1);

        QSqlQuery query(db);
        if(getId() == 0)
        {
            query.prepare("SELECT id, type FROM items AS t1 WHERE NOT EXISTS(SELECT * FROM folder2item WHERE child=t1.id) ORDER BY t1.type, t1.name");
        }
        else
        {
            query.prepare("SELECT t1.child, t2.type FROM folder2item AS t1, items AS t2 WHERE t1.parent = :id AND t2.id = t1.child ORDER BY t2.id");
            query.bindValue(":id", getId());
        }
        QUERY_EXEC(return );

        CEvtD2WShowItems * evt2 = new CEvtD2WShowItems(getId(), getDBName());
        while(query.next())
        {
            evt2->items << evt_item_t(query.value(0).toULongLong(), query.value(1).toUInt());
        }
        CGisWidget::self().postEventForWks(evt2);
    }
    else
    {
        CEvtD2WHideFolder * evt1 = new CEvtD2WHideFolder(getId(), getDBName());
        CGisWidget::self().postEventForWks(evt1);
    }
}

void IDBFolder::remove()
{
    IDBFolder * folder = dynamic_cast<IDBFolder*>(parent());
    if(folder == 0)
    {
        return;
    }
    remove(folder->getId(), getId());

    CEvtD2WHideFolder * evt1 = new CEvtD2WHideFolder(getId(), getDBName());
    CGisWidget::self().postEventForWks(evt1);
}

void IDBFolder::setupFromDB()
{
    if(id == 0)
    {
        return;
    }

    QSqlQuery query(db);

    // get basic properties like name and key
    query.prepare("SELECT key, name, comment FROM folders WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return );
    query.next();

    key = query.value(0).toString();
    setText(CGisListDB::eColumnName, query.value(1).toString());
    setToolTip(CGisListDB::eColumnName, query.value(2).toString());

    // check if folder has child folders (to set expand indicator)
    query.prepare("SELECT EXISTS(SELECT 1 FROM folder2folder WHERE parent=:id LIMIT 1)");
    query.bindValue(":id", id);
    QUERY_EXEC(return );
    query.next();

    if(query.value(0).toInt() == 1)
    {
        setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    }
    else
    {
        // check for child items (to set expand indicator)
        query.prepare("SELECT EXISTS(SELECT 1 FROM folder2item WHERE parent=:id LIMIT 1)");
        query.bindValue(":id", id);
        QUERY_EXEC(return );
        query.next();
        if(query.value(0).toInt() == 1)
        {
            setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
        }
    }

    // if the folder is loadable the checkbox has to be displayed and
    // an event to query the state has to be sent to the workspace
    if(isLoadable)
    {
        setCheckState(CGisListDB::eColumnCheckbox, Qt::Unchecked);
        CEvtD2WReqInfo * evt = new CEvtD2WReqInfo(getId(), getDBName());
        CGisWidget::self().postEventForWks(evt);
    }
}

void IDBFolder::addChildren(const QSet<QString>& activeChildren)
{
    QSqlQuery query(db);

    // folders 1st
    query.prepare("SELECT t1.child, t2.type FROM folder2folder AS t1, folders AS t2 WHERE t1.parent = :id AND t2.id = t1.child ORDER BY t2.id");
    query.bindValue(":id", id);
    QUERY_EXEC(return );
    while(query.next())
    {
        quint64 idChild     = query.value(0).toULongLong();
        quint32 typeChild   = query.value(1).toInt();
        IDBFolder::createFolderByType(db, typeChild, idChild, this);
    }

    sortChildren(CGisListDB::eColumnName, Qt::AscendingOrder);

    // tracks 2nd
    query.prepare("SELECT t1.child FROM folder2item AS t1, items AS t2 WHERE t1.parent = :id AND t2.id = t1.child AND t2.type=:type ORDER BY t2.id");
    query.bindValue(":id", id);
    query.bindValue(":type", IGisItem::eTypeTrk);
    QUERY_EXEC(return );
    while(query.next())
    {
        quint64 idChild = query.value(0).toULongLong();
        CDBItem * item = new CDBItem(db, idChild, this);
        item->setCheckState(CGisListDB::eColumnCheckbox, activeChildren.contains(item->getKey()) ? Qt::Checked : Qt::Unchecked);
    }

    // routes 3rd
    query.prepare("SELECT t1.child FROM folder2item AS t1, items AS t2 WHERE t1.parent = :id AND t2.id = t1.child AND t2.type=:type ORDER BY t2.id");
    query.bindValue(":id", id);
    query.bindValue(":type", IGisItem::eTypeRte);
    QUERY_EXEC(return );
    while(query.next())
    {
        quint64 idChild = query.value(0).toULongLong();
        CDBItem * item = new CDBItem(db, idChild, this);
        item->setCheckState(CGisListDB::eColumnCheckbox, activeChildren.contains(item->getKey()) ? Qt::Checked : Qt::Unchecked);
    }

    //waypoints 4th
    query.prepare("SELECT t1.child FROM folder2item AS t1, items AS t2 WHERE t1.parent = :id AND t2.id = t1.child AND t2.type=:type ORDER BY t2.id");
    query.bindValue(":id", id);
    query.bindValue(":type", IGisItem::eTypeWpt);
    QUERY_EXEC(return );
    while(query.next())
    {
        quint64 idChild = query.value(0).toULongLong();
        CDBItem * item = new CDBItem(db, idChild, this);
        item->setCheckState(CGisListDB::eColumnCheckbox, activeChildren.contains(item->getKey()) ? Qt::Checked : Qt::Unchecked);
    }

    // overlays 5th
    query.prepare("SELECT t1.child FROM folder2item AS t1, items AS t2 WHERE t1.parent = :id AND t2.id = t1.child AND t2.type=:type ORDER BY t2.id");
    query.bindValue(":id", id);
    query.bindValue(":type", IGisItem::eTypeOvl);
    QUERY_EXEC(return );
    while(query.next())
    {
        quint64 idChild = query.value(0).toULongLong();
        CDBItem * item = new CDBItem(db, idChild, this);
        item->setCheckState(CGisListDB::eColumnCheckbox, activeChildren.contains(item->getKey()) ? Qt::Checked : Qt::Unchecked);
    }
}

void IDBFolder::remove(quint64 idParent, quint64 idFolder)
{
    QSqlQuery query(db);
    // delete this particular relation first
    query.prepare("DELETE FROM folder2folder WHERE parent=:parent AND child=:child");
    query.bindValue(":parent", idParent);
    query.bindValue(":child", idFolder);
    QUERY_EXEC();

    query.prepare("SELECT EXISTS(SELECT 1 FROM folder2folder WHERE child=:id LIMIT 1)");
    query.bindValue(":id", idFolder);
    QUERY_EXEC();
    // if there is no other relation delete the children, too.
    if(!query.next() || (query.value(0).toInt() == 0))
    {
        query.prepare("SELECT child FROM folder2folder WHERE parent=:id");
        query.bindValue(":id", idFolder);
        QUERY_EXEC();
        while(query.next())
        {
            remove(idFolder, query.value(0).toULongLong());
        }

        // remove the child items relations
        query.prepare("DELETE FROM folder2item WHERE parent=:id");
        query.bindValue(":id", idFolder);
        QUERY_EXEC();

        // and remove the folder
        query.prepare("DELETE FROM folders WHERE id=:id");
        query.bindValue(":id", idFolder);
        QUERY_EXEC()
    }
}
