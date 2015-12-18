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

#include "CMainWindow.h"
#include "gis/CGisWidget.h"
#include "gis/db/CDBProject.h"
#include "gis/db/CSelectSaveAction.h"
#include "gis/db/IDB.h"
#include "gis/db/macros.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/qms/CQmsProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSettings.h"



#include <QtSql>
#include <QtWidgets>
CDBProject::CDBProject(CGisListWks * parent)
    : IGisProject(eTypeDb, "", parent)
    , id(0)
{
    setIcon(CGisListWks::eColumnIcon,QIcon("://icons/32x32/DBProject.png"));
}

CDBProject::CDBProject(const QString& dbName, quint64 id, CGisListWks *parent)
    : IGisProject(eTypeDb, dbName, parent)
    , id(id)
{
    setIcon(CGisListWks::eColumnIcon,QIcon("://icons/32x32/DBProject.png"));
    db = QSqlDatabase::database(dbName);

    QSqlQuery query(db);
    query.prepare("SELECT date, name, data FROM folders WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return );
    query.next();

    QString date    = query.value(0).toString();
    QString name    = query.value(1).toString();
    QByteArray data = query.value(2).toByteArray();

    if(data.isEmpty())
    {
        metadata.name = name;
        metadata.time = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");

        query.prepare("UPDATE folders SET keyqms=:keyqms WHERE id=:id");
        query.bindValue(":keyqms", getKey());
        query.bindValue(":id", id);
        QUERY_EXEC(return );
    }
    else
    {
        QDataStream in(&data, QIODevice::ReadOnly);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setVersion(QDataStream::Qt_5_2);
        *this << in;
        filename = dbName;
    }

    setupName(name);
    setToolTip(CGisListWks::eColumnName, getInfo());
    updateItems();
    valid = true;
}

CDBProject::~CDBProject()
{
    CEvtW2DAckInfo * info = new CEvtW2DAckInfo(false, getId(), db.connectionName());
    CGisWidget::self().postEventForDb(info);
}

void CDBProject::restoreDBLink()
{
    db = QSqlDatabase::database(filename);

    QSqlQuery query(db);
    query.prepare("SELECT id FROM folders WHERE keyqms=:keyqms");
    query.bindValue(":keyqms", getKey());
    QUERY_EXEC(return );
    if(query.next())
    {
        id = query.value(0).toULongLong();
        setupName("----");
        valid = true;
    }
}

void CDBProject::setupName(const QString &defaultName)
{
    IGisProject::setupName(defaultName);

    // look for a parent folder's name to be used as suffix
    QSqlQuery query(db);
    query.prepare("SELECT t1.name FROM folders AS t1 WHERE id=(SELECT parent FROM folder2folder WHERE child=:id) AND (t1.type=:type1 OR t1.type=:type2)");
    query.bindValue(":id", id);
    query.bindValue(":type1", IDBFolder::eTypeGroup);
    query.bindValue(":type2", IDBFolder::eTypeProject);
    QUERY_EXEC();
    if(query.next())
    {
        nameSuffix   = query.value(0).toString();
    }
    setText(CGisListWks::eColumnName, getNameEx());
}


void CDBProject::postStatus()
{
    // collect the keys of all child items and post them to the database view
    CEvtW2DAckInfo * info = new CEvtW2DAckInfo(true, getId(), db.connectionName());

    bool changedItems   = false;
    const int N         = childCount();
    for(int n = 0; n < N; n++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(n));
        if(item)
        {
            info->keysChildren << item->getKey().item;
            changedItems |= item->isChanged();
        }
    }

    // update item counters and track/waypoint correlation
    // updateItems(); <--- don't! this is causing a crash
    if(!changedItems)
    {
        setText(CGisListWks::eColumnDecoration,"");
    }

    CGisWidget::self().postEventForDb(info);
}

void CDBProject::updateItem(IGisItem *& item, quint64 idItem)
{
    QSqlQuery query(db);

    // serialize complete history of item
    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_5_2);
    in << item->getHistory();

    // prepare icon to be saved
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QPixmap pixmap = item->getIcon();
    pixmap.save(&buffer, "PNG");
    buffer.seek(0);

    query.prepare("UPDATE items SET type=:type, keyqms=:keyqms, icon=:icon, name=:name, comment=:comment, data=:data, hash=:hash WHERE id=:id AND hash=:oldhash");
    query.bindValue(":type",    item->type());
    query.bindValue(":keyqms",  item->getKey().item);
    query.bindValue(":icon",    buffer.data());
    query.bindValue(":name",    item->getName());
    query.bindValue(":comment", item->getInfo());
    query.bindValue(":data",    data);
    query.bindValue(":hash",    item->getHash());
    query.bindValue(":id",      idItem);
    query.bindValue(":oldhash", item->getLastDatabaseHash());
    QUERY_EXEC(throw -1);

    if(query.numRowsAffected())
    {
        // the update has been successful.
        // set current hash as database hash.
        item->setLastDatabaseHash(idItem, db);
    }
    else
    {
        // there are two reasons why an update does not affect a row
        // 1) the hash is different because another user changed the item
        // 2) the id was not fond because another user removed the item
        //
        // Let's test for 1) and get the last user and timestamp of the change
        query.prepare("SELECT last_user, last_change FROM items WHERE id=:id");
        query.bindValue(":id", id);
        QUERY_EXEC(throw -1);
        if(query.next())
        {
            QString user = query.value(0).toString();
            QString date = query.value(1).toString();

            QString msg = QObject::tr(
                "The item %1 has been changed by %2 (%3). \n\n"
                "To solve this conflict you can create and save a clone, force your version or drop "
                "your version and take the one from the database"
                ).arg(item->getNameEx()).arg(user).arg(date);

            QMessageBox msgBox(QMessageBox::Question, QObject::tr("Conflict with database..."), msg, QMessageBox::NoButton, CMainWindow::self().getBestWidgetForParent());
            QAbstractButton* pButClone  = msgBox.addButton(QObject::tr("Clone && Save"), QMessageBox::YesRole);
            QAbstractButton* pButForce  = msgBox.addButton(QObject::tr("Force Save"), QMessageBox::ApplyRole);
            QAbstractButton* pButUpdate = msgBox.addButton(QObject::tr("Take remote"), QMessageBox::DestructiveRole);
            msgBox.addButton(QMessageBox::Abort);

            msgBox.exec();

            if(msgBox.clickedButton() == pButClone)
            {
                IGisItem * item2    = item->createClone();
                quint64 idItem      = insertItem(item2);

                delete item;
                item = item2;

                query.prepare("INSERT INTO folder2item (parent, child) VALUES (:parent, :child)");
                query.bindValue(":parent", id);
                query.bindValue(":child", idItem);
                QUERY_EXEC(throw -1);

            }
            else if(msgBox.clickedButton() == pButForce)
            {
                query.prepare("UPDATE items SET type=:type, keyqms=:keyqms, icon=:icon, name=:name, comment=:comment, data=:data, hash=:hash WHERE id=:id");
                query.bindValue(":type",    item->type());
                query.bindValue(":keyqms",  item->getKey().item);
                query.bindValue(":icon",    buffer.data());
                query.bindValue(":name",    item->getName());
                query.bindValue(":comment", item->getInfo());
                query.bindValue(":data",    data);
                query.bindValue(":hash",    item->getHash());
                query.bindValue(":id",      idItem);
                QUERY_EXEC(throw -1);

                item->setLastDatabaseHash(idItem, db);
            }
            else if(msgBox.clickedButton() == pButUpdate)
            {
                item->updateFromDB(idItem, db);
            }
            else // abort
            {
                throw -1;
            }
        }
        else
        {
            // seems to be case 2)
            //gee it's gone!
        }
    }
}

quint64 CDBProject::insertItem(IGisItem * item)
{
    QSqlQuery query(db);

    // serialize complete history of item
    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_5_2);
    in << item->getHistory();

    // prepare icon to be saved
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QPixmap pixmap = item->getIcon();
    pixmap.save(&buffer, "PNG");
    buffer.seek(0);

    query.prepare("INSERT INTO items (type, keyqms, icon, name, comment, data, hash) VALUES (:type, :keyqms, :icon, :name, :comment, :data, :hash)");
    query.bindValue(":type",    item->type());
    query.bindValue(":keyqms",  item->getKey().item);
    query.bindValue(":icon",    buffer.data());
    query.bindValue(":name",    item->getName());
    query.bindValue(":comment", item->getInfo());
    query.bindValue(":data",    data);
    query.bindValue(":hash",    item->getHash());
    QUERY_EXEC(throw -1);

    quint64 idItem = IDB::getLastInsertID(db, "items");
    if(idItem == 0)
    {
        qDebug() << "childId equals 0. bad.";
        throw -1;
    }

    item->setLastDatabaseHash(idItem, db);

    return idItem;
}

bool CDBProject::save()
{
    bool clearProjectChangeFlag = true;
    int lastResult = CSelectSaveAction::eResultNone;

    QSqlQuery query(db);

    int N = childCount();
    PROGRESS_SETUP(QObject::tr("Save ..."), 0, N, CMainWindow::getBestWidgetForParent());

    CEvtW2DAckInfo * info = new CEvtW2DAckInfo(true, getId(), db.connectionName());

    try
    {
        for(int i = 0; i < N; i++)
        {
            PROGRESS(i, throw 0);

            IGisItem * item = dynamic_cast<IGisItem*>(child(i));
            if(item == 0)
            {
                continue;
            }

            // skip unchanged items
            if(!item->isChanged())
            {
                info->keysChildren << item->getKey().item;
                continue;
            }

            // test if item exists in database
            quint64 idItem      = 0;
            quint32 typeItem    = 0;
            query.prepare("SELECT id, type FROM items WHERE keyqms=:keyqms");
            query.bindValue(":keyqms", item->getKey().item);
            QUERY_EXEC(throw -1);

            if(query.next())
            {
                idItem      = query.value(0).toULongLong();
                typeItem    = query.value(1).toUInt();

                // check if relation already exists.
                query.prepare("SELECT id FROM folder2item WHERE parent=:parent AND child=:child");
                query.bindValue(":parent", id);
                query.bindValue(":child", idItem);
                QUERY_EXEC();

                if(!query.next())
                {
                    // item is already in database but folder relation does not exit
                    int result  = lastResult;

                    if(lastResult == CSelectSaveAction::eResultNone)
                    {
                        IGisItem * item1 = 0;

                        // load item from database for a compare
                        switch(typeItem)
                        {
                        case IGisItem::eTypeWpt:
                            item1 = new CGisItemWpt(idItem, db, 0);
                            break;

                        case IGisItem::eTypeTrk:
                            item1 = new CGisItemTrk(idItem, db, 0);
                            break;

                        case IGisItem::eTypeRte:
                            item1 = new CGisItemRte(idItem, db, 0);
                            break;

                        case IGisItem::eTypeOvl:
                            item1 = new CGisItemOvlArea(idItem, db, 0);
                            break;

                        default:
                            ;
                        }

                        if(item1 == 0)
                        {
                            qDebug() << "no item to compare!?.";
                            throw -1;
                        }


                        CSelectSaveAction dlg(item, item1, &progress);
                        dlg.exec();

                        result = dlg.getResult();
                        if(dlg.allOthersToo())
                        {
                            lastResult = result;
                        }
                    }
                    if(result == CSelectSaveAction::eResultNone)
                    {
                        // no decision by user, cancel operation.
                        // this is different to a skip as a skip will
                        // just skip saving the data, but the item to folder
                        // link will be still processed.
                        clearProjectChangeFlag = false;
                        continue;
                    }

                    // create relation
                    query.prepare("INSERT INTO folder2item (parent, child) VALUES (:parent, :child)");
                    query.bindValue(":parent", id);
                    query.bindValue(":child", idItem);
                    QUERY_EXEC(throw -1);

                    if(result == CSelectSaveAction::eResultSave)
                    {
                        // the item is in the database and has no relation to the folder -> update only if the user confirms.
                        updateItem(item, idItem);
                    }
                }
                else
                {
                    // the item is in the database and has a relation to the folder -> simply update item
                    updateItem(item, idItem);
                }
            }
            else
            {
                // the item is not in the database -> insert item and create relation to folder
                idItem = insertItem(item);

                // create relation
                query.prepare("INSERT INTO folder2item (parent, child) VALUES (:parent, :child)");
                query.bindValue(":parent", id);
                query.bindValue(":child", idItem);
                QUERY_EXEC(throw -1);
            }

            info->keysChildren << item->getKey().item;
            item->updateDecoration(IGisItem::eMarkNone, IGisItem::eMarkChanged);
        }

        // serialize metadata of project
        QByteArray data;
        QDataStream in(&data, QIODevice::WriteOnly);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setVersion(QDataStream::Qt_5_2);
        *this >> in;

        // update folder entry in database
        query.prepare("UPDATE folders SET name=:name, comment=:comment, data=:data WHERE id=:id");
        query.bindValue(":name", getName());
        query.bindValue(":comment", getInfo());
        query.bindValue(":data", data);
        query.bindValue(":id", getId());
        QUERY_EXEC(throw -1);

        // report status to database view
        info->updateLostFound = true;
        CGisWidget::self().postEventForDb(info);
        if(clearProjectChangeFlag)
        {
            setText(CGisListWks::eColumnDecoration,"");
        }
    }
    catch(int n)
    {
        if(n < 0)
        {
            // ooch, failed queries, better quit without much reporting
            delete info;
        }
        else
        {
            // abort by user, report status to database view
            info->updateLostFound = true;
            CGisWidget::self().postEventForDb(info);
        }
        return false;
    }
    return true;
}

void CDBProject::showItems(CEvtD2WShowItems * evt)
{
    foreach(const evt_item_t &item, evt->items)
    {
        IGisItem * gisItem = 0;
        switch(item.type)
        {
        case IGisItem::eTypeWpt:
            gisItem = new CGisItemWpt(item.id, db, this);
            break;

        case IGisItem::eTypeTrk:
            gisItem = new CGisItemTrk(item.id, db, this);
            break;

        case IGisItem::eTypeRte:
            gisItem = new CGisItemRte(item.id, db, this);
            break;

        case IGisItem::eTypeOvl:
            gisItem = new CGisItemOvlArea(item.id, db, this);
            break;

        default:
            ;
        }

        /* [Issue #72] Database/Workspace inconsistency in QMS 1.4.0

           When an item with no key is loaded it is "healed". The healing
           will mark it as changed. To avoid this save all items that are
           marked as changed right after loading from the database.

         */
        if(gisItem && gisItem->isChanged())
        {
            bool success = true;
            try
            {
                updateItem(gisItem, item.id);
            }
            catch(int)
            {
                success = false;
            }

            if(success)
            {
                gisItem->updateDecoration(IGisItem::eMarkNone, IGisItem::eMarkChanged);
            }
        }
    }

    postStatus();
    setToolTip(CGisListWks::eColumnName, getInfo());
}

void CDBProject::hideItems(CEvtD2WHideItems * evt)
{
    IGisItem::key_t key;
    key.project = getKey();

    QMessageBox::StandardButtons last = QMessageBox::YesToAll;

    foreach(const QString &k, evt->keys)
    {
        key.item = k;
        delItemByKey(key, last);
    }

    postStatus();
    setToolTip(CGisListWks::eColumnName, getInfo());
}

