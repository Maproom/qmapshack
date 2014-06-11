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
#include <QtGui>

QMutex CMapItem::mutexActiveMaps(QMutex::Recursive);

CMapItem::CMapItem(QListWidget *parent, CMap * map)
    : QListWidgetItem(parent)
    , map(map)
{

}

CMapItem::~CMapItem()
{

}

void CMapItem::updateIcon()
{
    if(filenames.isEmpty())
    {
        return;
    }

    QPixmap img("://icons/32x32/map.png");
    QFileInfo fi(filenames.first());
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

    if(isActivated())
    {
        QPainter p(&img);
        p.drawPixmap(0,0,QPixmap("://icons/16x16/redGlow.png"));
    }

    setIcon(QIcon(img));    
}

bool CMapItem::isActivated()
{
    QMutexLocker lock(&mutexActiveMaps);
    return !files.isEmpty();
}

bool CMapItem::toggleActivate()
{
    QMutexLocker lock(&mutexActiveMaps);
    if(files.isEmpty())
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
    qDeleteAll(files);
    files.clear();

    updateIcon();
    moveToBottom();
}


bool CMapItem::activate()
{
    QMutexLocker lock(&mutexActiveMaps);

    if(!files.isEmpty())
    {
        qDeleteAll(files);
        files.clear();
    }

    foreach(const QString& filename, filenames)
    {
        IMap * m = 0;
        // load map by suffix
        QFileInfo fi(filename);
        if(fi.suffix().toLower() == "rmap")
        {
            m = new CMapRMAP(filename, map);
        }
        else if(fi.suffix().toLower() == "jnx")
        {
            m = new CMapJNX(filename, map);
        }
        else if(fi.suffix().toLower() == "img")
        {
            m = new CMapIMG(filename, map);
        }
        else if(fi.suffix().toLower() == "vrt")
        {
            m = new CMapVRT(filename, map);
        }
        else if(fi.suffix().toLower() == "map")
        {
            m = new CMapMAP(filename, map);
        }

        // if map is activated sucessfully add to the list of map files
        // else delete all previous loaded maps and abort
        if(m && m->activated())
        {
            files << m;
        }
        else
        {
            qDeleteAll(files);
            files.clear();
            updateIcon();
            return false;
        }
    }
    // no mapfiles loaded? Bad.
    if(files.isEmpty())
    {
        return false;
    }
    updateIcon();
    moveToBottom();
    return true;
}

void CMapItem::moveToTop()
{
    QListWidget * w = listWidget();
    QMutexLocker lock(&mutexActiveMaps);

    w->takeItem(w->row(this));
    w->insertItem(0, this);

    map->emitSigCanvasUpdate();
}


void CMapItem::moveToBottom()
{

    int row;
    QListWidget * w = listWidget();
    QMutexLocker lock(&mutexActiveMaps);

    w->takeItem(w->row(this));
    for(row = 0; row < w->count(); row++)
    {
        CMapItem * item = dynamic_cast<CMapItem*>(w->item(row));
        if(item && item->files.isEmpty())
        {
            break;
        }
    }
    w->insertItem(row, this);

    map->emitSigCanvasUpdate();
}
