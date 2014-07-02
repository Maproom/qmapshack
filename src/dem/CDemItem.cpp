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

#include "dem/CDemItem.h"
#include "dem/CDemVRT.h"
#include "dem/CDemDraw.h"
#include "dem/IDemProp.h"

#include <QtWidgets>

QMutex CDemItem::mutexActiveDems(QMutex::Recursive);

CDemItem::CDemItem(QTreeWidget * parent, CDemDraw *dem)
    : QTreeWidgetItem(parent)
    , dem(dem)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
}

CDemItem::~CDemItem()
{

}

void CDemItem::saveConfig(QSettings& cfg)
{
    if(demfile.isNull())
    {
        return;
    }

    cfg.beginGroup(key);
    demfile->saveConfig(cfg);
    cfg.endGroup();

}

void CDemItem::loadConfig(QSettings& cfg)
{
    if(demfile.isNull())
    {
        return;
    }

    cfg.beginGroup(key);
    demfile->loadConfig(cfg);
    cfg.endGroup();

}

void CDemItem::showChildren(bool yes)
{
    if(yes && !demfile.isNull())
    {
        QTreeWidget * tw = treeWidget();

        QTreeWidgetItem * item = new QTreeWidgetItem(this);
        item->setFlags(Qt::ItemIsEnabled);
        tw->setItemWidget(item, 0, demfile->getSetup());
    }
    else
    {
        QList<QTreeWidgetItem*> items = takeChildren();
        qDeleteAll(items);
        delete demfile->getSetup();
    }
}


void CDemItem::updateIcon()
{
    if(filename.isEmpty())
    {
        return;
    }

    QPixmap img("://icons/32x32/Map.png");
    QFileInfo fi(filename);
    if(fi.suffix().toLower() == "vrt")
    {
        img = QPixmap("://icons/32x32/MimeDemVRT.png");
    }

    setIcon(0,QIcon(img));
}

bool CDemItem::isActivated()
{
    QMutexLocker lock(&mutexActiveDems);
    return !demfile.isNull();
}

bool CDemItem::toggleActivate()
{
    QMutexLocker lock(&mutexActiveDems);
    if(demfile.isNull())
    {
        return activate();
    }
    else
    {
        deactivate();
        return false;
    }
}

void CDemItem::deactivate()
{
    QMutexLocker lock(&mutexActiveDems);
    // remove demfile setup dialog as child of this item
    showChildren(false);

    // remove demfile object
    delete demfile;

    // maybe used to reflect changes in the icon
    updateIcon();
    // move to bottom of the active dem list
    moveToBottom();

    // deny drag-n-drop again
    setFlags(flags() & ~Qt::ItemIsDragEnabled);
}


bool CDemItem::activate()
{
    QMutexLocker lock(&mutexActiveDems);

    // remove demfile object
    delete demfile;

    // load map by suffix
    QFileInfo fi(filename);
    if(fi.suffix().toLower() == "vrt")
    {
        demfile = new CDemVRT(filename, dem);
    }

    updateIcon();

    // no mapfiles loaded? Bad.
    if(demfile.isNull())
    {
        return false;
    }

    // if map is activated sucessfully add to the list of map files
    // else delete all previous loaded maps and abort
    if(!
            demfile->activated())
    {
        delete demfile;
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
    dem->loadConfigForDemItem(this);

    // Add the demfile setup dialog as child of this item
    showChildren(true);
    return true;
}

void CDemItem::moveToTop()
{
    QTreeWidget * w = treeWidget();
    QMutexLocker lock(&mutexActiveDems);

    w->takeTopLevelItem(w->indexOfTopLevelItem(this));
    w->insertTopLevelItem(0, this);

    dem->emitSigCanvasUpdate();
}


void CDemItem::moveToBottom()
{

    int row;
    QTreeWidget * w = treeWidget();
    QMutexLocker lock(&mutexActiveDems);

    w->takeTopLevelItem(w->indexOfTopLevelItem(this));
    for(row = 0; row < w->topLevelItemCount(); row++)
    {
        CDemItem * item = dynamic_cast<CDemItem*>(w->topLevelItem(row));
        if(item && item->demfile.isNull())
        {
            break;
        }
    }
    w->insertTopLevelItem(row, this);

    dem->emitSigCanvasUpdate();
}
