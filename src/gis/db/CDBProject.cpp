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

#include "gis/db/CDBProject.h"
#include "gis/CGisWidget.h"
#include "gis/qms/CQmsProject.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/db/macros.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "helpers/CSettings.h"

#include <QtSql>
#include <QtWidgets>
CDBProject::CDBProject(CGisListWks * parent)
    : IGisProject(eTypeDb, "", parent)
{
    setIcon(0,QIcon("://icons/32x32/DBProject.png"));
}

CDBProject::CDBProject(const QString& dbName, quint64 id, CGisListWks *parent)
    : IGisProject(eTypeDb, dbName, parent)
    , id(id)    
{
    setIcon(0,QIcon("://icons/32x32/DBProject.png"));
    db = QSqlDatabase::database(dbName);

    QSqlQuery query(db);
    query.prepare("SELECT date, name, data FROM folders WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return);
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
        QUERY_EXEC(return);
    }
    else
    {
        QDataStream in(&data, QIODevice::ReadOnly);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setVersion(QDataStream::Qt_5_2);
        *this << in;
    }

    setText(0, name);
    setToolTip(0, getInfo());
    valid = true;
}

CDBProject::~CDBProject()
{
    CGisWidget::self().queueActionForDb(action_info_t(eActW2DInfoProject, db, id));
}

void CDBProject::restoreDBLink()
{
    db = QSqlDatabase::database(filename);

    QSqlQuery query(db);
    query.prepare("SELECT id FROM folders WHERE key=:key");
    query.bindValue(":key", getKey());   
    QUERY_EXEC(return);
    if(query.next())
    {
        id = query.value(0).toULongLong();
        setupName("----");
        valid = true;
    }

    action_info_t info(eActW2DInfoProject, db, id);
    info.isLoaded = true;
    CGisWidget::self().queueActionForDb(info);
}

void CDBProject::saveAs()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();

    QString filter = "*.qms";
    QString fn = QFileDialog::getSaveFileName(0, QObject::tr("Save GIS data to..."), path, "*.gpx;; *.qms", &filter);

    if(fn.isEmpty())
    {
        return;
    }

    if(filter == "*.gpx")
    {
        CGpxProject::saveAs(fn, *this);
    }
    else if(filter == "*.qms")
    {
        CQmsProject::saveAs(fn, *this);
    }
    else
    {
        return;
    }

    path = QFileInfo(fn).absolutePath();
    cfg.setValue("Paths/lastGisPath", path);
}

void CDBProject::save()
{
    QSqlQuery query(db);
    action_info_t info(eActW2DInfoProject, db, id);

    int N = childCount();
    QProgressDialog progress(QObject::tr("Save ..."), QObject::tr("Abort save"), 0, 100);
    progress.setWindowModality(Qt::WindowModal);

    for(int i = 0; i < N; i++)
    {
        progress.setValue(i * 100 / N);
        if (progress.wasCanceled())
        {
            return;
        }


        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
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
        QUERY_EXEC(return);

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
            QUERY_EXEC(return;);

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
            QUERY_EXEC(return;);

            query.prepare("SELECT last_insert_rowid() from items");
            QUERY_EXEC(return;);
            query.next();
            idItem = query.value(0).toULongLong();
            if(idItem == 0)
            {
                qDebug() << "childId equals 0. bad.";
                return;
            }
        }

        // check if relation already exists.
        query.prepare("SELECT id FROM folder2item WHERE parent=:parent AND child=:child");
        query.bindValue(":parent", id);
        query.bindValue(":child", idItem);
        QUERY_EXEC(;);

        if(!query.next())
        {
            // create relation
            query.prepare("INSERT INTO folder2item (parent, child) VALUES (:parent, :child)");
            query.bindValue(":parent", id);
            query.bindValue(":child", idItem);
            QUERY_EXEC(return);
        }

        info.keysChildren << item->getKey().item;
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
    QUERY_EXEC(return);

    info.isLoaded = true;
    CGisWidget::self().queueActionForDb(info);

    markAsSaved();
}

void CDBProject::showItem(quint64 idChild)
{
    if(idChild == 0)
    {
        return showAllItems();
    }

    QSqlQuery query(db);
    query.prepare("SELECT key, type FROM items WHERE id=:id");
    query.bindValue(":id", idChild);
    QUERY_EXEC(return);
    if(!query.next())
    {
        return;
    }

    QString key     = query.value(0).toString();
    qint32  type    = query.value(1).toInt();

    const int N = childCount();
    for(int i = 0; i < N; i++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey().item == key)
        {
            return;
        }
    }

    IGisItem * item;
    switch(type)
    {
        case IGisItem::eTypeWpt:
            item = new CGisItemWpt(idChild, db, this);
            break;
        case IGisItem::eTypeTrk:
            item = new CGisItemTrk(idChild, db, this);
            break;
        case IGisItem::eTypeRte:
            item = new CGisItemRte(idChild, db, this);
            break;
        case IGisItem::eTypeOvl:
            item = new CGisItemOvlArea(idChild, db, this);
            break;
        default:
            item = 0;
    }
}

void CDBProject::hideItem(quint64 idChild)
{
    QSqlQuery query(db);
    query.prepare("SELECT key FROM items WHERE id=:id");
    query.bindValue(":id", idChild);
    QUERY_EXEC(return);
    if(!query.next())
    {
        return;
    }


    QString key = query.value(0).toString();

    const int N = childCount();
    for(int i = 0; i < N; i++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey().item == key)
        {
            delete item;
            return;
        }
    }

}

void CDBProject::showAllItems()
{
    QSqlQuery query(db);
    action_info_t info(eActW2DInfoProject, db, id);

    query.prepare("SELECT t1.child, t2.type FROM folder2item AS t1, items AS t2 WHERE t1.parent = :id AND t2.id = t1.child ORDER BY t2.id");
    query.bindValue(":id", id);
    QUERY_EXEC(return);
    while(query.next())
    {
        quint64 idChild = query.value(0).toULongLong();
        qint32  type    = query.value(1).toInt();

        IGisItem * item;
        switch(type)
        {
            case IGisItem::eTypeWpt:
                item = new CGisItemWpt(idChild, db, this);
                break;
            case IGisItem::eTypeTrk:
                item = new CGisItemTrk(idChild, db, this);
                break;
            case IGisItem::eTypeRte:
                item = new CGisItemRte(idChild, db, this);
                break;
            case IGisItem::eTypeOvl:
                item = new CGisItemOvlArea(idChild, db, this);
                break;
            default:
                item = 0;
        }
        if(item)
        {
            info.keysChildren << item->getKey().item;
        }
    }

    info.isLoaded = true;
    CGisWidget::self().queueActionForDb(info);

    setToolTip(0, getInfo());
}
