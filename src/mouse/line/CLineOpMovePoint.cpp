/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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
#include "mouse/line/CLineOpMovePoint.h"
#include "units/IUnit.h"

#include <QtWidgets>

CLineOpMovePoint::CLineOpMovePoint(SGisLine &points, CCanvas& canvas, QObject *parent)
    : ILineOp(points, canvas, parent)
    , idxFocus(NOIDX)
{
    cursor = QCursor(QPixmap(":/cursors/cursorMovePoint.png"),0,0);
}

CLineOpMovePoint::~CLineOpMovePoint()
{
}

void CLineOpMovePoint::mousePressEvent(QMouseEvent * e)
{
    if(idxFocus == NOIDX)
    {
        ILineOp::mousePressEvent(e);
        return;
    }

}

void CLineOpMovePoint::mouseMoveEvent(QMouseEvent * e)
{
    if(mapMove)
    {
        ILineOp::mouseMoveEvent(e);
        return;
    }

    const QPoint& pos = e->pos();
    idxFocus = isCloseTo(pos);


    canvas.update();
}

void CLineOpMovePoint::mouseReleaseEvent(QMouseEvent *e)
{
    ILineOp::mouseReleaseEvent(e);
}

void CLineOpMovePoint::draw(QPainter& p)
{
    if(idxFocus == NOIDX)
    {
        return;
    }

    const IGisLine::point_t& pt = points[idxFocus];

    QRect r(0,0,9,9);
    r.moveCenter(pt.pixel.toPoint());

    p.setPen(Qt::red);
    p.setBrush(Qt::NoBrush);
    p.drawRect(r);

}

qint32 CLineOpMovePoint::isCloseTo(const QPoint& pos)
{
    const int N = points.size();
    for(int i = 0; i < N; i++)
    {
        const IGisLine::point_t& pt = points[i];
        if((pos - pt.pixel).manhattanLength() < 20)
        {
            return i;
        }
    }

    return NOIDX;
}
