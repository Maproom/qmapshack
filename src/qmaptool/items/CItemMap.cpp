/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "canvas/CCanvas.h"
#include "canvas/IDrawContext.h"
#include "CMainWindow.h"
#include "items/CItemMap.h"
#include "items/CItemMapLayer.h"
#include "items/CItemTreeWidget.h"

#include <QtWidgets>

CItemMap::CItemMap(const QString &filename)
    : CGdalFile(CGdalFile::eTypeProj)
    , IItem(filename)
{
    setText(CItemTreeWidget::eColumnName, QFileInfo(filename).completeBaseName());
    setIcon(CItemTreeWidget::eColumnName, QIcon("://icons/32x32/FolderMap.png"));

    reload();
}

CItemMap::~CItemMap()
{
    unload();
}

QPointF CItemMap::getScale() const
{
    return QPointF(xscale, yscale);
}

void CItemMap::reload()
{
    load(filename);
    setToolTip(CItemTreeWidget::eColumnName,filename + "\n"  + getInfo());

    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(f.read(1024));
    hash = md5.result().toHex();
    f.close();
}

void CItemMap::drawBoundingBox(QPainter& p, IDrawContext * dc)
{
    QPointF pt1  = ref1;
    QPointF pt2  = ref2;
    QPointF pt3  = ref3;
    QPointF pt4  = ref4;

    dc->convertCoord2Map(pt1);
    dc->convertCoord2Map(pt2);
    dc->convertCoord2Map(pt3);
    dc->convertCoord2Map(pt4);

    dc->convertMap2Screen(pt1);
    dc->convertMap2Screen(pt2);
    dc->convertMap2Screen(pt3);
    dc->convertMap2Screen(pt4);

    p.setPen(QPen(Qt::red, 2));
    QPolygonF line;
    line << pt1 << pt2 << pt3 << pt4 << pt1;
    p.drawPolyline(line);

}
