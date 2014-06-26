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
#include "map/CMap.h"

#include <QtWidgets>

QMutex CDemItem::mutexActiveDems(QMutex::Recursive);

CDemItem::CDemItem(QTreeWidget * parent, CMap *map)
    : QTreeWidgetItem(parent)
    , map(map)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
}

CDemItem::~CDemItem()
{

}

void CDemItem::updateIcon()
{
    if(filenames.isEmpty())
    {
        return;
    }

    QPixmap img("://icons/32x32Mmap.png");
    QFileInfo fi(filenames.first());
    if(fi.suffix().toLower() == "vrt")
    {
        img = QPixmap("://icons/32x32/MimeVRT.png");
    }

    if(isActivated())
    {
        QPainter p(&img);
        p.drawPixmap(0,0,QPixmap("://icons/16x16/redGlow.png"));
    }

    setIcon(0,QIcon(img));
}

bool CDemItem::isActivated()
{
    QMutexLocker lock(&mutexActiveDems);
    return !files.isEmpty();
}

bool CDemItem::toggleActivate()
{
    QMutexLocker lock(&mutexActiveDems);
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

void CDemItem::deactivate()
{
    QMutexLocker lock(&mutexActiveDems);
    qDeleteAll(files);
    files.clear();

    updateIcon();
    moveToBottom();

    setFlags(flags() & ~Qt::ItemIsDragEnabled);
}


bool CDemItem::activate()
{
    QMutexLocker lock(&mutexActiveDems);

    if(!files.isEmpty())
    {
        qDeleteAll(files);
        files.clear();
    }

    foreach(const QString& filename, filenames)
    {
        IDem * m = 0;
        // load map by suffix
        QFileInfo fi(filename);
        if(fi.suffix().toLower() == "vrt")
        {
            m = new CDemVRT(filename, map);
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

    setFlags(flags() | Qt::ItemIsDragEnabled);
    return true;
}

void CDemItem::moveToTop()
{
    QTreeWidget * w = treeWidget();
    QMutexLocker lock(&mutexActiveDems);

    w->takeTopLevelItem(w->indexOfTopLevelItem(this));
    w->insertTopLevelItem(0, this);

    map->emitSigCanvasUpdate();
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
        if(item && item->files.isEmpty())
        {
            break;
        }
    }
    w->insertTopLevelItem(row, this);

    map->emitSigCanvasUpdate();
}
