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

#include "GeoMath.h"
#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "gis/IGisLine.h"
#include "gis/rte/router/CRouterSetup.h"
#include "mouse/CScrOptEditLine.h"
#include "mouse/CScrOptPoint.h"
#include "mouse/CScrOptRange.h"
#include "mouse/IMouseEditLine.h"
#include "units/IUnit.h"


#include <QtWidgets>

IMouseEditLine::IMouseEditLine(quint32 features, const QPointF& point, CGisDraw * gis, CCanvas * parent)
    : IMouse(gis, parent)
    , features(features)
{

}

IMouseEditLine::IMouseEditLine(quint32 features, IGisLine &src, CGisDraw *gis, CCanvas *parent)
    : IMouse(gis, parent)
    , features(features)
{

}

IMouseEditLine::~IMouseEditLine()
{
}

void IMouseEditLine::drawLine(const QPolygonF &l, QPainter& p)
{
}


void IMouseEditLine::draw(QPainter& p, bool needsRedraw, const QRect &rect)
{

}

void IMouseEditLine::mousePressEvent(QMouseEvent * e)
{

}

void IMouseEditLine::mouseMoveEvent(QMouseEvent * e)
{

}

void IMouseEditLine::mouseReleaseEvent(QMouseEvent *e)
{

}

void IMouseEditLine::wheelEvent(QWheelEvent * e)
{

}

void IMouseEditLine::slotDeletePoint()
{

}

void IMouseEditLine::slotSelectRange()
{

}

void IMouseEditLine::slotDeleteRange()
{

}

void IMouseEditLine::slotMovePoint()
{
}

void IMouseEditLine::slotAddPoint1()
{

}

void IMouseEditLine::slotAddPoint2()
{
}


void IMouseEditLine::slotAbort()
{
}

void IMouseEditLine::slotCopyToOrig()
{
}


