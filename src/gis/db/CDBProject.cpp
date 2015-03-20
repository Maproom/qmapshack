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
#include "gis/db/macros.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/qms/CQmsProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
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

        query.prepare("UPDATE folders SET key=:key WHERE id=:id");
        query.bindValue(":key", getKey());
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

    setText(CGisListWks::eColumnName, name);
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
    query.prepare("SELECT id FROM folders WHERE key=:key");
    query.bindValue(":key", getKey());
    QUERY_EXEC(return );
    if(query.next())
    {
        id = query.value(0).toULongLong();
        setupName("----");
        valid = true;

    }
}

void CDBProject::postStatus()
{
    CEvtW2DAckInfo * info = new CEvtW2DAckInfo(true, getId(), db.connectionName());

    const int N = childCount();
    for(int n = 0; n < N; n++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(n));
        if(item)
        {
            info->keysChildren << item->getKey().item;
        }
    }

    updateItems();

    CGisWidget::self().postEventForDb(info);
}

bool CDBProject::saveAs()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();

    QString filter = "*.qms";
    QString fn = QFileDialog::getSaveFileName(&CMainWindow::self(), QObject::tr("Save GIS data to..."), path, "*.gpx;; *.qms", &filter);

    if(fn.isEmpty())
    {
        return false;
    }

    bool res = false;
    if(filter == "*.gpx")
    {
        res = CGpxProject::saveAs(fn, *this);
    }
    else if(filter == "*.qms")
    {
        res = CQmsProject::saveAs(fn, *this);
    }
    else
    {
        return false;
    }

    path = QFileInfo(fn).absolutePath();
    cfg.setValue("Paths/lastGisPath", path);

    return res;
}

bool CDBProject::save()
{
    QSqlQuery query(db);

    int N = childCount();
    QProgressDialog progress(QObject::tr("Save ..."), QObject::tr("Abort save"), 0, 100);
    progress.setWindowModality(Qt::WindowModal);

    CEvtW2DAckInfo * info = new CEvtW2DAckInfo(true, getId(), db.connectionName());

    try
    {
        for(int i = 0; i < N; i++)
        {
            progress.setValue(i * 100 / N);
            if (progress.wasCanceled())
            {
                throw 0;
            }


            IGisItem * item = dynamic_cast<IGisItem*>(child(i));
            if(item == 0)
            {
                continue;
            }

            if(item->text(CGisListWks::eColumnDecoration).isEmpty())
            {
                info->keysChildren << item->getKey().item;
                continue;
            }

            quint64 idItem = 0;

            // serialize complete history of item
            QByteArray data;
            QDataStream in(&data, QIODevice::WriteOnly);
            in.setByteOrder(QDataStream::LittleEndian);
            in.setVersion(QDataStream::Qt_5_2);
            in << item->getHistory();

            // test if item exists in database
            query.prepare("SELECT id FROM items WHERE key=:key");
            query.bindValue(":key", item->getKey().item);
            QUERY_EXEC(throw -1);

            QBuffer buffer;
            buffer.open(QIODevice::ReadWrite);
            QPixmap pixmap = item->getIcon();
            pixmap.save(&buffer, "PNG");
            buffer.seek(0);


            if(query.next())
            {
                // item exits -> update item data in database
                idItem = query.value(0).toULongLong();
                query.prepare("UPDATE items SET type=:type, key=:key, icon=:icon, name=:name, comment=:comment, data=:data WHERE id=:id");
                query.bindValue(":type",    item->type());
                query.bindValue(":key",     item->getKey().item);
                query.bindValue(":icon",    buffer.data());
                query.bindValue(":name",    item->getName());
                query.bindValue(":comment", item->getInfo());
                query.bindValue(":data", data);
                query.bindValue(":id", idItem);
                QUERY_EXEC(throw -1);
            }
            else
            {
                // item is unknown to database -> create item in database
                query.prepare("INSERT INTO items (type, key, icon, name, comment, data) VALUES (:type, :key, :icon, :name, :comment, :data)");
                query.bindValue(":type",    item->type());
                query.bindValue(":key",     item->getKey().item);
                query.bindValue(":icon",    buffer.data());
                query.bindValue(":name",    item->getName());
                query.bindValue(":comment", item->getInfo());
                query.bindValue(":data", data);
                QUERY_EXEC(throw -1);

                query.prepare("SELECT last_insert_rowid() from items");
                QUERY_EXEC(throw -1);
                query.next();
                idItem = query.value(0).toULongLong();
                if(idItem == 0)
                {
                    qDebug() << "childId equals 0. bad.";
                    throw -1;
                }
            }

            // check if relation already exists.
            query.prepare("SELECT id FROM folder2item WHERE parent=:parent AND child=:child");
            query.bindValue(":parent", id);
            query.bindValue(":child", idItem);
            QUERY_EXEC(; );

            if(!query.next())
            {
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

        info->updateLostFound = true;
        CGisWidget::self().postEventForDb(info);
        setText(CGisListWks::eColumnDecoration,"");
    }
    catch(int n)
    {
        if(n < 0)
        {
            delete info;
        }
        else
        {
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
        switch(item.type)
        {
        case IGisItem::eTypeWpt:
            new CGisItemWpt(item.id, db, this);
            break;

        case IGisItem::eTypeTrk:
            new CGisItemTrk(item.id, db, this);
            break;

        case IGisItem::eTypeRte:
            new CGisItemRte(item.id, db, this);
            break;

        case IGisItem::eTypeOvl:
            new CGisItemOvlArea(item.id, db, this);
            break;

        default:;
        }
    }

    setToolTip(CGisListWks::eColumnName, getInfo());
    postStatus();    
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
    setToolTip(CGisListWks::eColumnName, getInfo());
    postStatus();
}

