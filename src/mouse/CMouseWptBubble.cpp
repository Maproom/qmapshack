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

#include "canvas/CCanvas.h"
#include "gis/CGisWidget.h"
#include "gis/wpt/CGisItemWpt.h"
#include "mouse/CMouseWptBubble.h"

#include <QtWidgets>

CMouseWptBubble::CMouseWptBubble(const IGisItem::key_t &key, CGisDraw * gis, CCanvas * parent)
    : IMouse(gis, parent)
    , key(key)
{
    cursor      = QCursor(QPixmap("://cursors/cursorArrow.png"),0,0);
}

CMouseWptBubble::~CMouseWptBubble()
{
}

void CMouseWptBubble::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect)
{
}

void CMouseWptBubble::mousePressEvent(QMouseEvent * e)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    QPointF pos = e->pos();

    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWidget::self().getItemByKey(key));
    if(wpt)
    {
        wpt->mousePress(pos);
    }
    else
    {
        canvas->resetMouse();
    }
}

void CMouseWptBubble::mouseMoveEvent(QMouseEvent * e)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    QPointF pos = e->pos();

    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWidget::self().getItemByKey(key));
    if(wpt)
    {
        wpt->mouseMove(pos);
    }
    else
    {
        canvas->resetMouse();
    }
}

void CMouseWptBubble::mouseReleaseEvent(QMouseEvent *e)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    QPointF pos = e->pos();

    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWidget::self().getItemByKey(key));
    if(wpt)
    {
        wpt->mouseRelease(pos);
    }
    else
    {
        canvas->resetMouse();
    }
}

void CMouseWptBubble::wheelEvent(QWheelEvent * e)
{
}
