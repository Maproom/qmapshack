/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "poi/CPoiDraw.h"
#include "poi/CPoiItem.h"
#include "poi/CPoiPOI.h"

#include <QtWidgets>

QMutex CPoiItem::mutexActivePois(QMutex::Recursive);

CPoiItem::CPoiItem(QTreeWidget* parent, CPoiDraw* poi)
    : QTreeWidgetItem(parent)
    , poi(poi)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
}


void CPoiItem::saveConfig(QSettings& cfg)
{
    if(poifile.isNull())
    {
        return;
    }

    cfg.beginGroup(key);
    poifile->saveConfig(cfg);
    cfg.endGroup();
}

void CPoiItem::loadConfig(QSettings& cfg)
{
    if(poifile.isNull())
    {
        return;
    }

    cfg.beginGroup(key);
    poifile->loadConfig(cfg);
    cfg.endGroup();
}

void CPoiItem::showChildren(bool yes)
{
    if(yes && !poifile.isNull())
    {
        QTreeWidget* tw = treeWidget();

        QTreeWidgetItem* item = new QTreeWidgetItem(this);
        item->setFlags(Qt::ItemIsEnabled);
        tw->setItemWidget(item, 0, poifile->getSetup());
    }
    else
    {
        QList<QTreeWidgetItem*> items = takeChildren();
        qDeleteAll(items);
        delete poifile->getSetup();
    }
}


void CPoiItem::updateIcon()
{
    if(filename.isEmpty())
    {
        return;
    }

    QPixmap img("://icons/32x32/Poi.png");
    QFileInfo fi(filename);
    if(fi.suffix().toLower() == "poi")
    {
        img = QPixmap("://icons/32x32/MimePoiPOI.png");
    }

    setIcon(0, QIcon(img));
}

bool CPoiItem::isActivated()
{
    QMutexLocker lock(&mutexActivePois);
    return !poifile.isNull();
}

bool CPoiItem::toggleActivate()
{
    QMutexLocker lock(&mutexActivePois);
    if(poifile.isNull())
    {
        return activate();
    }
    else
    {
        deactivate();
        return false;
    }
}

void CPoiItem::deactivate()
{
    QMutexLocker lock(&mutexActivePois);
    // remove poifile setup dialog as child of this item
    showChildren(false);

    // remove poifile object
    delete poifile;

    // maybe used to reflect changes in the icon
    updateIcon();
    // move to bottom of the active poi list
    moveToBottom();

    // deny drag-n-drop again
    setFlags(flags() & ~Qt::ItemIsDragEnabled);
}


bool CPoiItem::activate()
{
    QMutexLocker lock(&mutexActivePois);

    // remove poifile object
    delete poifile;

    // load map by suffix
    QFileInfo fi(filename);
    if(fi.suffix().toLower() == "poi")
    {
        poifile = new CPoiPOI(filename, poi);
    }

    updateIcon();

    // no mapfiles loaded? Bad.
    if(poifile.isNull())
    {
        return false;
    }

    // if map is activated successfully add to the list of map files
    // else delete all previous loaded maps and abort
    if(!poifile->activated())
    {
        delete poifile;
        return false;
    }

    moveToBottom();

    setFlags(flags() | Qt::ItemIsDragEnabled);
    /*
       As the map file setup is stored in the context of the CMapDraw object
       the configuration has to be loaded via the CMapDraw object to select
       the correct group context in the QSetting object.
       This call will result into a call of loadConfig() of this CMapItem
       object.
     */
    poi->loadConfigForPoiItem(this);

    // Add the poifile setup dialog as child of this item
    showChildren(true);
    return true;
}

void CPoiItem::moveToTop()
{
    QTreeWidget* w = treeWidget();
    QMutexLocker lock(&mutexActivePois);

    w->takeTopLevelItem(w->indexOfTopLevelItem(this));
    w->insertTopLevelItem(0, this);

    poi->emitSigCanvasUpdate();
}


void CPoiItem::moveToBottom()
{
    int row;
    QTreeWidget* w = treeWidget();
    QMutexLocker lock(&mutexActivePois);

    w->takeTopLevelItem(w->indexOfTopLevelItem(this));
    for(row = 0; row < w->topLevelItemCount(); row++)
    {
        CPoiItem* item = dynamic_cast<CPoiItem*>(w->topLevelItem(row));
        if(item && item->poifile.isNull())
        {
            break;
        }
    }
    w->insertTopLevelItem(row, this);

    poi->emitSigCanvasUpdate();
}
