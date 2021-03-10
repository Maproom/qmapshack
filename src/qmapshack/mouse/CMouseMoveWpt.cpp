/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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
#include "gis/CGisDraw.h"
#include "gis/CGisWorkspace.h"
#include "gis/GeoMath.h"
#include "gis/proj_x.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CDraw.h"
#include "helpers/CWptIconManager.h"
#include "mouse/CMouseAdapter.h"
#include "mouse/CMouseMoveWpt.h"
#include "units/IUnit.h"

#include <QtWidgets>

CMouseMoveWpt::CMouseMoveWpt(CGisItemWpt &wpt, CGisDraw * gis, CCanvas *canvas, CMouseAdapter *mouse)
    : IMouse(gis, canvas, mouse),
    key(wpt.getKey()),
    origPos(wpt.getPosition() * DEG_TO_RAD),
    radius(wpt.getProximity()),
    avoid(wpt.isNogo())
{
    cursor  = QCursor(QPixmap(":/cursors/cursorMoveWpt.png"), 0, 0);
    icon    = CWptIconManager::self().getWptIconByName(wpt.getIconName(), focus);
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

    qreal r = CGisItemWpt::calcRadius(newPos, p2, radius, gis);
    CGisItemWpt::drawCircle(p, p2, r, avoid, false);

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

    CDraw::text(str, p, (p2 + QPoint(0, -30)), Qt::darkBlue);

    p.drawPixmap(p1 - focus, icon);
    p.drawPixmap(p2 - focus, icon);
}

void CMouseMoveWpt::rightButtonDown(const QPoint& point)
{
    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWorkspace::self().getItemByKey(key));
    if(wpt != nullptr)
    {
        wpt->setHideArea(false);
    }
    canvas->resetMouse();
    canvas->triggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseMoveWpt::mouseMoved(const QPoint& pos)
{
    newPos = pos;
    gis->convertPx2Rad(newPos);
    canvas->update();
}

void CMouseMoveWpt::leftClicked(const QPoint& point)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    QPointF pos(point);
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

void CMouseMoveWpt::scaleChanged()
{
    canvas->update();
}

