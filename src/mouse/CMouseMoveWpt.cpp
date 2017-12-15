/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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
#include "gis/CGisWorkspace.h"
#include "gis/WptIcons.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CDraw.h"
#include "mouse/CMouseMoveWpt.h"
#include "units/IUnit.h"

#include <QtWidgets>
#include <proj_api.h>

CMouseMoveWpt::CMouseMoveWpt(CGisItemWpt &wpt, CGisDraw * gis, CCanvas *parent)
    : IMouse(gis, parent),
      key(wpt.getKey()),
      origPos(wpt.getPosition()*DEG_TO_RAD),
      radius(wpt.getProximity()),
      avoid(wpt.isAvoid())
{
    icon = getWptIconByName(wpt.getIconName(), focus);
    cursor  = QCursor(QPixmap(":/cursors/cursorMoveWpt.png"), 0, 0);
    newPos  = origPos;
    wpt.setHideArea(true);
    canvas->triggerCompleteUpdate(CCanvas::eRedrawGis);
}

CMouseMoveWpt::~CMouseMoveWpt()
{
}

void CMouseMoveWpt::draw(QPainter& p, CCanvas::redraw_e, const QRect&)
{
    QString val, unit;
    qreal a1 = 0, a2 = 0;
    QPointF p1 = origPos;
    QPointF p2 = newPos;

    qreal d = GPS_Math_Distance(p1.x(), p1.y(), p2.x(), p2.y(), a1, a2);
    IUnit::self().meter2distance(d, val, unit);
    const QString &str = QString("%1 %2, %3%4").arg(val).arg(unit).arg(a2, 0, 'f', 1).arg(QChar(0260));

    gis->convertRad2Px(p1);
    gis->convertRad2Px(p2);

    qreal r = CGisItemWpt::calcRadius(newPos,p2,radius,gis);
    CGisItemWpt::drawCircle(p,p2,r,avoid,false);

    QPointF p11 = p1 + QPoint(17 * qCos((a1 - 90) * DEG_TO_RAD), 17 * qSin((a1 - 90) * DEG_TO_RAD));
    QPointF p22 = p2 + QPoint(21 * qCos((a2 + 90) * DEG_TO_RAD), 21 * qSin((a2 + 90) * DEG_TO_RAD));

    QPen pen(Qt::darkBlue, 3);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::MiterJoin);
    p.setPen(pen);

    p.setBrush(Qt::NoBrush);
    p.drawEllipse(p1, 16, 16);
    p.drawEllipse(p2, 16, 16);
    p.drawLine(p11, p22);

    p.save();
    p.translate(p22);
    p.rotate(a2 + 180);
    QPolygonF arrow;
    arrow << QPointF(0, 0) << QPointF(5, -20) << QPointF(0, -10) << QPointF(-5, -20);
    p.setBrush(Qt::NoBrush);
    p.drawPolygon(arrow);
    p.restore();

    CDraw::text(str, p, (p2 + QPoint(0, -30)).toPoint(), Qt::darkBlue);

    p.drawPixmap(p1 - focus, icon);
    p.drawPixmap(p2 - focus, icon);
}


void CMouseMoveWpt::mousePressEvent(QMouseEvent * e)
{
    point  = e->pos();
    if(e->button() == Qt::RightButton)
    {
        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWorkspace::self().getItemByKey(key));
        if(wpt != nullptr)
        {
            wpt->setHideArea(false);
        }
        canvas->resetMouse();
        canvas->triggerCompleteUpdate(CCanvas::eRedrawGis);
    }
    else if(e->button() == Qt::LeftButton)
    {
        mapMove = true;
        lastPoint = point;
    }
}

void CMouseMoveWpt::mouseMoveEvent(QMouseEvent * e)
{
    point  = e->pos();

    if(mapMove)
    {
        if(point != lastPoint)
        {
            QPoint delta = point - lastPoint;
            canvas->moveMap(delta);
            mapDidMove = true;
        }
    }
    else
    {
        newPos = point;
        gis->convertPx2Rad(newPos);
    }

    lastPoint = point;
    canvas->update();
}

void CMouseMoveWpt::mouseReleaseEvent(QMouseEvent *e)
{
    point = e->pos();
    if(!mapDidMove && (e->button() == Qt::LeftButton))
    {
        QMutexLocker lock(&IGisItem::mutexItems);

        QPointF pos = e->pos();
        gis->convertPx2Rad(pos);
        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWorkspace::self().getItemByKey(key));
        if(wpt != nullptr)
        {
            wpt->setPosition(pos * RAD_TO_DEG);
            wpt->setHideArea(false);
        }
        canvas->resetMouse();
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }

    mapMove     = false;
    mapDidMove    = false;
}

void CMouseMoveWpt::wheelEvent(QWheelEvent*)
{
    canvas->update();
}

void CMouseMoveWpt::afterMouseLostEvent(QMouseEvent *e)
{
    if (e->type() == QEvent::MouseMove)
    {
        lastPoint = e->pos();
    }
    mapMove = e->buttons() & Qt::LeftButton;
    mapDidMove = true;
}

