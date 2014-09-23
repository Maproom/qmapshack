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

#include "mouse/CMouseEditLine.h"
#include "gis/trk/CGisItemTrk.h"
#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"

#include <QtWidgets>

CMouseEditLine::CMouseEditLine(const CGisItemTrk& trk, CGisDraw *gis, CCanvas *parent)
    : IMouse(gis, parent)
{
    cursor  = QCursor(QPixmap(":/cursors/cursorMoveLine.png"),0,0);
}

CMouseEditLine::~CMouseEditLine()
{

}

void CMouseEditLine::draw(QPainter& p, const QRect &rect)
{

}

void CMouseEditLine::mousePressEvent(QMouseEvent * e)
{
    point  = e->pos();
    if(e->button() == Qt::RightButton)
    {
        canvas->resetMouse();
        canvas->update();
    }
    else if(e->button() == Qt::LeftButton)
    {
    }

}

void CMouseEditLine::mouseMoveEvent(QMouseEvent * e)
{
    point  = e->pos();

    panCanvas(point);
}

void CMouseEditLine::mouseReleaseEvent(QMouseEvent *e)
{

}

void CMouseEditLine::wheelEvent(QWheelEvent * e)
{

}
