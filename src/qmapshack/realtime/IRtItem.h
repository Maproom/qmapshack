/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef IRTITEM_H
#define IRTITEM_H

#include <QTreeWidgetItem>
#include <QMutex>

class CRtDraw;

class IRtItem : public QTreeWidgetItem
{
public:
    IRtItem(QTreeWidget * parent);
    virtual ~IRtItem() = default;

    static QMutex mutexItems;

    enum column_e
    {
        eColumnIcon = 0
        ,eColumnCheckBox = eColumnIcon
        ,eColumnDecoration = eColumnIcon
        ,eColumnName = 1
    };

    virtual void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt) = 0;
};

#endif //IRTITEM_H

