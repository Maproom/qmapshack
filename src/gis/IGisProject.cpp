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

#include "gis/IGisProject.h"
#include "gis/IGisItem.h"
#include "gis/CGisListWks.h"
#include "gis/CGisDraw.h"

#include <QtWidgets>

IGisProject::IGisProject(const QString& key, const QString &filename, CGisListWks *parent)
    : QTreeWidgetItem(parent)
    , key(key)
    , filename(filename)
    , valid(false)
{
    if(key.isEmpty())
    {
        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData((char*)this, sizeof(*this));
        this->key = md5.result().toHex();
    }
}

IGisProject::~IGisProject()
{

}

QString IGisProject::getInfo()
{
    QString str = metadata.name.isEmpty() ? text(0) : metadata.name;

    if(metadata.time.isValid())
    {
        str += "\n";
        str += IUnit::datetime2string(metadata.time, false);
    }


    if(!metadata.desc.isEmpty())
    {
        str += "\n";

        if(metadata.desc.count() < 200)
        {
            str += metadata.desc;
        }
        else
        {
            str += metadata.desc.left(197) + "...";
        }
    }


    return str;
}

IGisItem * IGisProject::getItemByKey(const QString& key)
{
    for(int i = 0; i < childCount(); i++)
    {

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey() == key)
        {
            return item;
        }
    }
    return 0;
}

void IGisProject::getItemByPos(const QPointF& pos, QList<IGisItem *> &items)
{
    for(int i = 0; i < childCount(); i++)
    {

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(item->isCloseTo(pos))
        {
            items << item;
        }
    }
}


void IGisProject::delItemByKey(const QString& key)
{
    QList<QTreeWidgetItem*> items;
    for(int i = childCount(); i > 0; i--)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i-1));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey() == key)
        {
            QString msg = QObject::tr("Are you sure you want to delete '%1' from project '%2'?").arg(item->getName()).arg(text(0));
            QMessageBox::StandardButtons res = QMessageBox::question(0, QObject::tr("Delete..."), msg, QMessageBox::Ok|QMessageBox::No, QMessageBox::Ok);
            if(res != QMessageBox::Ok)
            {
                continue;
            }

            items << takeChild(i-1);
            setText(1,"*");
        }
    }
    qDeleteAll(items);
}

void IGisProject::editItemByKey(const QString& key)
{
    for(int i = childCount(); i > 0; i--)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i-1));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey() == key)
        {
            item->edit();
        }
    }
}

void IGisProject::drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, QSet<QString> &seenKeys, CGisDraw * gis)
{
    for(int i = 0; i < childCount(); i++)
    {
        if(gis->needsRedraw())
        {
            break;
        }

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(seenKeys.contains(item->getKey()))
        {
            continue;
        }
        seenKeys << item->getKey();

        item->drawItem(p, viewport, blockedAreas, gis);
    }

}

void IGisProject::drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis)
{
    for(int i = 0; i < childCount(); i++)
    {

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }


        item->drawItem(p, viewport, gis);
    }

}

void IGisProject::drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, QSet<QString> &seenKeys, const QFontMetricsF& fm, CGisDraw * gis)
{

    for(int i = 0; i < childCount(); i++)
    {
        if(gis->needsRedraw())
        {
            break;
        }

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(seenKeys.contains(item->getKey()))
        {
            continue;
        }
        seenKeys << item->getKey();

        item->drawLabel(p, viewport, blockedAreas, fm, gis);
    }

}
