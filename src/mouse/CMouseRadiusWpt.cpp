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
#include "mouse/CMouseRadiusWpt.h"
#include "units/IUnit.h"

#include <QtWidgets>
#include <proj_api.h>


#include <QtWidgets>

CMouseRadiusWpt::CMouseRadiusWpt(CGisItemWpt &wpt, CGisDraw * gis, CCanvas * parent)
    : IMouse(gis, parent),
      key(wpt.getKey()),
      wptPosition(wpt.getPosition() * DEG_TO_RAD),
      avoid(wpt.isAvoid())
{
    cursor  = QCursor(QPixmap(":/cursors/cursorRadiusWpt.png"), 0, 0);
    wpt.setHideArea(true);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

CMouseRadiusWpt::~CMouseRadiusWpt()
{
}

void CMouseRadiusWpt::draw(QPainter& p, CCanvas::redraw_e, const QRect&)
{
    QPointF pos = point;
    gis->convertPx2Rad(pos);
    dist = GPS_Math_Distance(pos.rx(),pos.ry(),wptPosition.x(),wptPosition.y());
    QPointF screenPos = wptPosition;
    gis->convertRad2Px(screenPos);
    qreal radius = CGisItemWpt::calcRadius(wptPosition,screenPos,dist,gis);

    CGisItemWpt::drawCircle(p,screenPos,radius,avoid,false);
}

void CMouseRadiusWpt::mousePressEvent(QMouseEvent * e)
{
    point  = e->pos();
    if(e->button() == Qt::RightButton)
    {
        QMutexLocker lock(&IGisItem::mutexItems);

        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWorkspace::self().getItemByKey(key));
        if(wpt != nullptr)
        {
            wpt->setHideArea(false);
        }
        canvas->resetMouse();
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }
    else if(e->button() == Qt::LeftButton)
    {
        mapMove = true;
        lastPoint = point;
    }
}

void CMouseRadiusWpt::mouseMoveEvent(QMouseEvent * e)
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

    lastPoint = point;
    canvas->update();
}

void CMouseRadiusWpt::mouseReleaseEvent(QMouseEvent *e)
{
    point = e->pos();
    if(!mapDidMove && e->button()==Qt::LeftButton)
    {
        QMutexLocker lock(&IGisItem::mutexItems);

        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWorkspace::self().getItemByKey(key));
        if(wpt != nullptr)
        {
            wpt->setProximity(dist);
            wpt->setHideArea(false);
        }
        canvas->resetMouse();
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }

    mapMove     = false;
    mapDidMove    = false;
}

void CMouseRadiusWpt::wheelEvent(QWheelEvent*)
{
    canvas->update();
}

void CMouseRadiusWpt::afterMouseLostEvent(QMouseEvent *e)
{
    if (e->type() == QEvent::MouseMove)
    {
        lastPoint = e->pos();
    }
    mapMove = e->buttons() & Qt::LeftButton;
    mapDidMove = true;
}
