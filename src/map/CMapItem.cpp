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
//    moveToEndOfActive();
    return true;
}
