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

    setIcon(QIcon("://icons/32x32/map.png"));
    moveToEndOfActive();
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
            setIcon(QIcon("://icons/32x32/rmap.png"));
        }
        else if(fi.suffix().toLower() == "jnx")
        {
            m = new CMapJNX(filename, map);
            setIcon(QIcon("://icons/32x32/jnx.png"));
        }

        // if map is actived sucessfully add to the list of map files
        // else delete all previous loaded maps and abort
        if(m && m->activated())
        {
            files << m;
        }
        else
        {
            qDeleteAll(files);
            files.clear();
            return false;
        }
    }
    // no mapfiles loaded? Bad.
    if(files.isEmpty())
    {
        return false;
    }
    setIcon(QIcon("://icons/32x32/map-active.png"));
    moveToEndOfActive();
    return true;
}

void CMapItem::moveToTop()
{
    QListWidget * w = listWidget();
    QMutexLocker lock(&mutexActiveMaps);

    w->takeItem(w->row(this));
    w->insertItem(0, this);

//    CMapDB::self().saveActiveMapsList();
    map->emitSigCanvasUpdate();
}

void CMapItem::moveToEndOfActive()
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

//    CMapDB::self().saveActiveMapsList();
    // Changeing the order in the list will
    // change draw order. Update canvas.
    map->emitSigCanvasUpdate();
}
