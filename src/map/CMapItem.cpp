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

#include "map/CMapDraw.h"
#include "map/CMapGEMF.h"
#include "map/CMapIMG.h"
#include "map/CMapItem.h"
#include "map/CMapJNX.h"
#include "map/CMapMAP.h"
#include "map/CMapRMAP.h"
#include "map/CMapTMS.h"
#include "map/CMapVRT.h"
#include "map/CMapWMTS.h"
#include "map/IMapProp.h"
#include <QtGui>

QMutex CMapItem::mutexActiveMaps(QMutex::Recursive);

CMapItem::CMapItem(QTreeWidget *parent, CMapDraw * map)
    : QTreeWidgetItem(parent)
    , map(map)
{
    // it's everything but not drag-n-drop until it gets activated
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
}

CMapItem::~CMapItem()
{
}

void CMapItem::saveConfig(QSettings& cfg) const
{
    if(mapfile.isNull())
    {
        return;
    }

    cfg.beginGroup(key);
    mapfile->saveConfig(cfg);
    cfg.endGroup();
}

void CMapItem::loadConfig(QSettings& cfg)
{
    if(mapfile.isNull())
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
    if(filename.isEmpty())
    {
        return;
    }

    QPixmap img("://icons/32x32/Map.png");
    QFileInfo fi(filename);
    if(fi.suffix().toLower() == "rmap")
    {
        img = QPixmap("://icons/32x32/MimeRMAP.png");
    }
    else if(fi.suffix().toLower() == "jnx")
    {
        img = QPixmap("://icons/32x32/MimeJNX.png");
    }
    else if(fi.suffix().toLower() == "vrt")
    {
        img = QPixmap("://icons/32x32/MimeVRT.png");
    }
    else if(fi.suffix().toLower() == "img")
    {
        img = QPixmap("://icons/32x32/MimeIMG.png");
    }
    else if(fi.suffix().toLower() == "map")
    {
        img = QPixmap("://icons/32x32/MimeMAP.png");
    }
    else if(fi.suffix().toLower() == "wmts")
    {
        img = QPixmap("://icons/32x32/MimeWMTS.png");
    }
    else if(fi.suffix().toLower() == "tms")
    {
        img = QPixmap("://icons/32x32/MimeTMS.png");
    }
    else if(fi.suffix().toLower() == "gemf")
    {
        img = QPixmap("://icons/32x32/MimeGEMF.png");
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
    // remove mapfile setup dialog as child of this item
    showChildren(false);

    // remove mapfile object
    delete mapfile;

    // maybe used to reflect changes in the icon
    updateIcon();
    // move to bottom of the active map list
    moveToBottom();

    // deny drag-n-drop again
    setFlags(flags() & ~Qt::ItemIsDragEnabled);
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
    else if(fi.suffix().toLower() == "wmts")
    {
        mapfile = new CMapWMTS(filename, map);
    }
    else if(fi.suffix().toLower() == "tms")
    {
        mapfile = new CMapTMS(filename, map);
    }
    else if(fi.suffix().toLower() == "gemf")
    {
        mapfile = new CMapGEMF(filename, map);
    }

    updateIcon();
    // no mapfiles loaded? Bad.
    if(mapfile.isNull())
    {
        return false;
    }

    // if map is activated successfully add to the list of map files
    // else delete all previous loaded maps and abort
    if(!mapfile->activated())
    {
        delete mapfile;
        return false;
    }

    setToolTip(0, mapfile->getCopyright());

    // append list of active map files
    moveToBottom();

    // an active map is subject to drag-n-drop
    setFlags(flags() | Qt::ItemIsDragEnabled);

    /*
        As the map file setup is stored in the context of the CMapDraw object
        the configuration has to be loaded via the CMapDraw object to select
        the correct group context in the QSetting object.
        This call will result into a call of loadConfig() of this CMapItem
        object.
     */
    map->loadConfigForMapItem(this);

    // Add the mapfile setup dialog as child of this item
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
