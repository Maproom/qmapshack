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

#include "gis/db/macros.h"
#include "gis/db/IDBFolder.h"
#include "gis/db/CDBFolderGroup.h"
#include "gis/db/CDBFolderProject.h"
#include "gis/db/CDBFolderOther.h"
#include "gis/db/CDBItem.h"
#include "gis/IGisItem.h"
#include "gis/CGisWidget.h"

#include <QtSql>

IDBFolder::IDBFolder(bool isLoadable, QSqlDatabase& db, type_e type, quint64 id, QTreeWidgetItem *parent)
    : QTreeWidgetItem(parent, type)
    , isLoadable(isLoadable)
    , db(db)
    , id(id)
{
    setupFromDB();
}

IDBFolder::IDBFolder(bool isLoadable, QSqlDatabase& db, type_e type, quint64 id, QTreeWidget * parent)
    : QTreeWidgetItem(parent, type)
    , isLoadable(isLoadable)
    , db(db)
    , id(id)
{
    setupFromDB();
}

IDBFolder::~IDBFolder()
{

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


//void IDBFolder::expanding()
//{
//    action_info_t info(eActD2WInfoProject, db, id);
//    CGisWidget::self().queueActionForWks(info);

//    qDeleteAll(takeChildren());
//    expanding(info);
//}


//void IDBFolder::update(const action_info_t& info)
//{
//    if(id == info.id)
//    {
//        QSqlQuery query(db);

//        // save state to restore later
//        bool expanded = isExpanded();
//        // take all children as anything could have happened
//        qDeleteAll(takeChildren());

//        // update text and tooltip
//        query.prepare("SELECT name, comment FROM folders WHERE id=:id");
//        query.bindValue(":id", id);
//        QUERY_EXEC(return);
//        query.next();

//        setText(eColumnName, query.value(0).toString());
//        setToolTip(eColumnName, query.value(1).toString());

//        // count folders linked to this folder
//        query.prepare("SELECT COUNT() FROM folder2folder WHERE parent=:id");
//        query.bindValue(":id", id);
//        QUERY_EXEC(return);
//        query.next();

//        qint32 nFolders = query.value(0).toInt();

//        // count items linked to this folder
//        query.prepare("SELECT COUNT() FROM folder2item WHERE parent=:id");
//        query.bindValue(":id", id);
//        QUERY_EXEC(return);
//        query.next();

//        qint32 nItems = query.value(0).toInt();

//        if(nFolders || nItems)
//        {
//            setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
//        }
//        else
//        {
//            setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
//        }


//        if(expanded)
//        {
//            expanding(info);
//        }

//        setCheckState(eColumnCheckbox, info.isLoaded ? Qt::Checked : Qt::Unchecked);
//    }
//    else
//    {
//        for(int i = 0; i < childCount(); i++)
//        {
//            IDBFolder * folder = dynamic_cast<IDBFolder*>(child(i));
//            if(folder)
//            {
//                folder->update(info);
//            }
//        }
//    }
//}

//void IDBFolder::toggle(quint64 idFolder)
//{
//    if(id == idFolder)
//    {
//        if(checkState(IDBFolder::eColumnCheckbox) == Qt::Checked)
//        {
//            action_t action1(eActD2WShowProject, db, id);
//            CGisWidget::self().queueActionForWks(action1);
//            action_t action2(eActD2WShowAllItems, db, id);
//            CGisWidget::self().queueActionForWks(action2);
//        }
//        else
//        {
//            action_t action(eActD2WHideProject, db, id);
//            CGisWidget::self().queueActionForWks(action);
//        }
//    }
//    else
//    {
//        for(int i = 0; i < childCount(); i++)
//        {
//            IDBFolder * folder = dynamic_cast<IDBFolder*>(child(i));
//            if(folder)
//            {
//                folder->toggle(idFolder);
//            }
//        }
//    }
//}

void IDBFolder::remove(quint64 idFolder)
{
    if(id == idFolder)
    {
        IDBFolder * folder = dynamic_cast<IDBFolder*>(parent());
        if(folder == 0)
        {
            return;
        }
        quint64 idParent = folder->getId();

//        action_t action(eActD2WHideProject, db, id);
//        CGisWidget::self().queueActionForWks(action);

        remove(idParent, idFolder);
    }
    else
    {
        for(int i = 0; i < childCount(); i++)
        {
            IDBFolder * folder = dynamic_cast<IDBFolder*>(child(i));
            if(folder)
            {
                folder->remove(idFolder);
            }
        }
    }
}



void IDBFolder::setupFromDB()
{
    if(id == 0)
    {
        return;
    }


    QSqlQuery query(db);

    query.prepare("SELECT name, comment FROM folders WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return);
    query.next();

    setText(eColumnName, query.value(0).toString());
    setToolTip(eColumnName, query.value(1).toString());

    query.prepare("SELECT EXISTS(SELECT 1 FROM folder2folder WHERE parent=:id LIMIT 1)");
    query.bindValue(":id", id);
    QUERY_EXEC(return);
    query.next();

    if(query.value(0).toInt() == 1)
    {
        setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    }
    else
    {
        query.prepare("SELECT EXISTS(SELECT 1 FROM folder2item WHERE parent=:id LIMIT 1)");
        query.bindValue(":id", id);
        QUERY_EXEC(return);
        query.next();
        if(query.value(0).toInt() == 1)
        {
            setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
        }
    }

    if(isLoadable)
    {
//        action_info_t info(eActD2WInfoProject, db, id);
//        CGisWidget::self().queueActionForWks(info);
//        setCheckState(eColumnCheckbox, info.isLoaded ? Qt::Checked : Qt::Unchecked);
    }
}

//void IDBFolder::expanding(const action_info_t& info)
//{
//    QSqlQuery query(db);

//    // folders 1st
//    query.prepare("SELECT t1.child, t2.type FROM folder2folder AS t1, folders AS t2 WHERE t1.parent = :id AND t2.id = t1.child ORDER BY t2.id");
//    query.bindValue(":id", id);
//    QUERY_EXEC(return);
//    while(query.next())
//    {
//        quint64 idChild     = query.value(0).toULongLong();
//        quint32 typeChild   = query.value(1).toInt();
//        IDBFolder::createFolderByType(db, typeChild, idChild, this);
//    }

//    // tracks 2nd
//    query.prepare("SELECT t1.child FROM folder2item AS t1, items AS t2 WHERE t1.parent = :id AND t2.id = t1.child AND t2.type=:type ORDER BY t2.id");
//    query.bindValue(":id", id);
//    query.bindValue(":type", IGisItem::eTypeTrk);
//    QUERY_EXEC(return);
//    while(query.next())
//    {
//        quint64 idChild = query.value(0).toULongLong();
//        CDBItem * item = new CDBItem(db, idChild, this);
//        item->setCheckState(eColumnCheckbox, info.keysChildren.contains(item->getKey()) ? Qt::Checked : Qt::Unchecked);
//    }

//    // routes 3rd
//    query.prepare("SELECT t1.child FROM folder2item AS t1, items AS t2 WHERE t1.parent = :id AND t2.id = t1.child AND t2.type=:type ORDER BY t2.id");
//    query.bindValue(":id", id);
//    query.bindValue(":type", IGisItem::eTypeRte);
//    QUERY_EXEC(return);
//    while(query.next())
//    {
//        quint64 idChild = query.value(0).toULongLong();
//        CDBItem * item = new CDBItem(db, idChild, this);
//        item->setCheckState(eColumnCheckbox, info.keysChildren.contains(item->getKey()) ? Qt::Checked : Qt::Unchecked);
//    }

//    //waypoints 4th
//    query.prepare("SELECT t1.child FROM folder2item AS t1, items AS t2 WHERE t1.parent = :id AND t2.id = t1.child AND t2.type=:type ORDER BY t2.id");
//    query.bindValue(":id", id);
//    query.bindValue(":type", IGisItem::eTypeWpt);
//    QUERY_EXEC(return);
//    while(query.next())
//    {
//        quint64 idChild = query.value(0).toULongLong();
//        CDBItem * item = new CDBItem(db, idChild, this);
//        item->setCheckState(eColumnCheckbox, info.keysChildren.contains(item->getKey()) ? Qt::Checked : Qt::Unchecked);
//    }

//    // overlays 5th
//    query.prepare("SELECT t1.child FROM folder2item AS t1, items AS t2 WHERE t1.parent = :id AND t2.id = t1.child AND t2.type=:type ORDER BY t2.id");
//    query.bindValue(":id", id);
//    query.bindValue(":type", IGisItem::eTypeOvl);
//    QUERY_EXEC(return);
//    while(query.next())
//    {
//        quint64 idChild = query.value(0).toULongLong();
//        CDBItem * item = new CDBItem(db, idChild, this);
//        item->setCheckState(eColumnCheckbox, info.keysChildren.contains(item->getKey()) ? Qt::Checked : Qt::Unchecked);
//    }
//}

void IDBFolder::remove(quint64 idParent, quint64 idFolder)
{
    QSqlQuery query(db);
    // delete this particular relation first
    query.prepare("DELETE FROM folder2folder WHERE parent=:parent AND child=:child");
    query.bindValue(":parent", idParent);
    query.bindValue(":child", idFolder);
    QUERY_EXEC(;);

    query.prepare("SELECT EXISTS(SELECT 1 FROM folder2folder WHERE child=:id LIMIT 1)");
    query.bindValue(":id", idFolder);
    QUERY_EXEC(;);
    // if there is no other relation delete the children, too.
    if(!query.next() || (query.value(0).toInt() == 0))
    {
        query.prepare("SELECT child FROM folder2folder WHERE parent=:id");
        query.bindValue(":id", idFolder);
        QUERY_EXEC(;);
        while(query.next())
        {
            remove(idFolder, query.value(0).toULongLong());
        }

        // remove the child items relations
        query.prepare("DELETE FROM folder2item WHERE parent=:id");
        query.bindValue(":id", idFolder);
        QUERY_EXEC(;);

        // and remove the folder
        query.prepare("DELETE FROM folders WHERE id=:id");
        query.bindValue(":id", idFolder);
        QUERY_EXEC(;);
    }
}
