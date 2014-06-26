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

#include "map/CMap.h"
#include "map/CMapItem.h"
#include "map/CMapJNX.h"
#include "map/CMapRMAP.h"
#include "map/CMapIMG.h"
#include "map/CMapVRT.h"
#include "map/CMapMAP.h"
#include "map/CMapPropSetup.h"
#include <QtGui>

QMutex CMapItem::mutexActiveMaps(QMutex::Recursive);

CMapItem::CMapItem(QTreeWidget *parent, CMap * map)
    : QTreeWidgetItem(parent)
    , map(map)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
}

CMapItem::~CMapItem()
{

}

void CMapItem::saveConfig(QSettings& cfg)
{
    if(!isActivated())
    {
        return;
    }

    cfg.beginGroup(key);
    mapfile->saveConfig(cfg);
    cfg.endGroup();
}

void CMapItem::loadConfig(QSettings& cfg)
{
    if(!isActivated())
    {
        return;
    }

    cfg.beginGroup(key);
    mapfile->loadConfig(cfg);
    cfg.endGroup();
}


void CMapItem::showChildren(bool yes)
{

    if(yes && !mapfile.isNull())
    {
        QTreeWidget * tw = treeWidget();

        QTreeWidgetItem * item = new QTreeWidgetItem(this);
        item->setFlags(Qt::ItemIsEnabled);        
        tw->setItemWidget(item, 0, mapfile->getSetup());
    }
    else
    {
        QList<QTreeWidgetItem*> items = takeChildren();
        qDeleteAll(items);
        delete mapfile->getSetup();
    }

}

void CMapItem::updateIcon()
{
    if(filename.isNull())
    {
        return;
    }

    QPixmap img("://icons/32x32/map.png");
    QFileInfo fi(filename);
    if(fi.suffix().toLower() == "rmap")
    {
        img = QPixmap("://icons/32x32/mime_rmap.png");
    }
    else if(fi.suffix().toLower() == "jnx")
    {
        img = QPixmap("://icons/32x32/mime_jnx.png");
    }
    else if(fi.suffix().toLower() == "vrt")
    {
        img = QPixmap("://icons/32x32/mime_vrt.png");
    }
    else if(fi.suffix().toLower() == "img")
    {
        img = QPixmap("://icons/32x32/mime_img.png");
    }
    else if(fi.suffix().toLower() == "map")
    {
        img = QPixmap("://icons/32x32/mime_map.png");
    }

    setIcon(0,QIcon(img));
}

bool CMapItem::isActivated()
{
    QMutexLocker lock(&mutexActiveMaps);
    return !mapfile.isNull();
}

bool CMapItem::toggleActivate()
{
    QMutexLocker lock(&mutexActiveMaps);
    if(mapfile.isNull())
    {
        return activate();
    }
    else
    {
        deactivate();
        return false;
    }
}

void CMapItem::deactivate()
{
    QMutexLocker lock(&mutexActiveMaps);
    delete mapfile;

    updateIcon();
    moveToBottom();

    setFlags(flags() & ~Qt::ItemIsDragEnabled);

    showChildren(false);
}


bool CMapItem::activate()
{
    QMutexLocker lock(&mutexActiveMaps);

    delete mapfile;

    // load map by suffix
    QFileInfo fi(filename);
    if(fi.suffix().toLower() == "rmap")
    {
        mapfile = new CMapRMAP(filename, map);
    }
    else if(fi.suffix().toLower() == "jnx")
    {
        mapfile = new CMapJNX(filename, map);
    }
    else if(fi.suffix().toLower() == "img")
    {
        mapfile = new CMapIMG(filename, map);
    }
    else if(fi.suffix().toLower() == "vrt")
    {
        mapfile = new CMapVRT(filename, map);
    }
    else if(fi.suffix().toLower() == "map")
    {
        mapfile = new CMapMAP(filename, map);
    }

    // if map is activated sucessfully add to the list of map files
    // else delete all previous loaded maps and abort
    if(mapfile.isNull() || !mapfile->activated())
    {
        delete mapfile;
        updateIcon();
        return false;
    }

    // no mapfiles loaded? Bad.
    if(mapfile.isNull())
    {
        return false;
    }
    updateIcon();
    moveToBottom();

    setFlags(flags() | Qt::ItemIsDragEnabled);

    showChildren(true);
    return true;
}

void CMapItem::moveToTop()
{
    QTreeWidget * w = treeWidget();
    QMutexLocker lock(&mutexActiveMaps);

    w->takeTopLevelItem(w->indexOfTopLevelItem(this));
    w->insertTopLevelItem(0, this);

    map->emitSigCanvasUpdate();
}


void CMapItem::moveToBottom()
{

    int row;
    QTreeWidget * w = treeWidget();
    QMutexLocker lock(&mutexActiveMaps);

    w->takeTopLevelItem(w->indexOfTopLevelItem(this));
    for(row = 0; row < w->topLevelItemCount(); row++)
    {
        CMapItem * item = dynamic_cast<CMapItem*>(w->topLevelItem(row));
        if(item && item->mapfile.isNull())
        {
            break;
        }
    }
    w->insertTopLevelItem(row, this);

    map->emitSigCanvasUpdate();
}
