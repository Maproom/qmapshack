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

#include "mouse/CMouseMoveWpt.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "gis/WptIcons.h"
#include "canvas/CCanvas.h"
#include "units/IUnit.h"
#include "GeoMath.h"

#include <QtWidgets>
#include <proj_api.h>

CMouseMoveWpt::CMouseMoveWpt(CGisItemWpt &wpt, CGisDraw * gis, CCanvas *parent)
    : IMouse(gis, parent)
{
    cursor  = QCursor(QPixmap(":/cursors/cursorMoveWpt.png"),0,0);
    key     = wpt.getKey();
    icon    = getWptIconByName(wpt.getIconName(), focus);
    origPos = wpt.getPosition() * DEG_TO_RAD;
}

CMouseMoveWpt::~CMouseMoveWpt()
{

}

void CMouseMoveWpt::draw(QPainter& p,  bool needsRedraw, const QRect &rect)
{
    QString val, unit, str;
    qreal d, a1 = 0, a2 = 0;
    QPointF p1 = origPos;
    QPointF p2 = newPos;

    d = GPS_Math_Distance(p1.x(), p1.y(), p2.x(), p2.y(), a1, a2);
    IUnit::self().meter2distance(d, val, unit);
    str = QString("%1 %2, %3").arg(val).arg(unit).arg(a2, 0, 'f', 1);

    gis->convertRad2Px(p1);
    gis->convertRad2Px(p2);


    QPointF p11 = p1 + QPoint(17 * cos((a1 - 90) * DEG_TO_RAD), 17 * sin((a1 - 90) * DEG_TO_RAD));
    QPointF p22 = p2 + QPoint(21 * cos((a2 + 90) * DEG_TO_RAD), 21 * sin((a2 + 90) * DEG_TO_RAD));


    QPen pen(Qt::darkBlue, 3);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::MiterJoin);
    p.setPen(pen);

    p.setBrush(Qt::NoBrush);
    p.drawEllipse(p1, 16, 16);
    p.drawEllipse(p2, 16, 16);
    p.drawLine(p11,p22);

    p.save();
    p.translate(p22);
    p.rotate(a2 + 180);
    QPolygonF arrow;
    arrow << QPointF(0,0) << QPointF(5, -20) << QPointF(0, -10) << QPointF(-5, -20);
    p.setBrush(Qt::NoBrush);
    p.drawPolygon(arrow);
    p.restore();

    CCanvas::drawText(str, p, (p2 + QPoint(0, -30)).toPoint(), Qt::darkBlue);

    p.drawPixmap(p1 - focus, icon);
    p.drawPixmap(p2 - focus, icon);

}

void CMouseMoveWpt::slotPanCanvas()
{
    IMouse::slotPanCanvas();

    newPos  = point;
    gis->convertPx2Rad(newPos);
}

void CMouseMoveWpt::mousePressEvent(QMouseEvent * e)
{
    point  = e->pos();
    if(e->button() == Qt::RightButton)
    {
        canvas->resetMouse();
        canvas->update();
    }
    else if(e->button() == Qt::LeftButton)
    {
        QPointF pos = e->pos();
        gis->convertPx2Rad(pos);
        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWidget::self().getItemByKey(key));
        if(wpt != 0)
        {
            wpt->setPosition(pos * RAD_TO_DEG);
        }
        canvas->resetMouse();
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }
}

void CMouseMoveWpt::mouseMoveEvent(QMouseEvent * e)
{
    point  = e->pos();
    newPos  = point;
    gis->convertPx2Rad(newPos);

    panCanvas(point);
}

void CMouseMoveWpt::mouseReleaseEvent(QMouseEvent *e)
{
    point  = e->pos();
}

void CMouseMoveWpt::wheelEvent(QWheelEvent * e)
{
    canvas->update();
}

