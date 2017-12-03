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
#include "gis/WptIcons.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CDraw.h"
#include "mouse/CMouseRadiusWpt.h"
#include "units/IUnit.h"

#include <QtWidgets>
#include <proj_api.h>


#include <QtWidgets>

CMouseRadiusWpt::CMouseRadiusWpt(CGisItemWpt &wpt, CGisDraw * gis, CCanvas * parent)
    : IMouse(gis, parent)
{
    cursor  = QCursor(QPixmap(":/cursors/cursorRadiusWpt.png"), 0, 0);
    key     = wpt.getKey();
    origProximity = wpt.getProximity();
    wptPosition = wpt.getPosition() * DEG_TO_RAD;
}

CMouseRadiusWpt::~CMouseRadiusWpt()
{
}

void CMouseRadiusWpt::draw(QPainter& p, CCanvas::redraw_e, const QRect&)
{
}

void CMouseRadiusWpt::mousePressEvent(QMouseEvent * e)
{
    point  = e->pos();
    if(e->button() == Qt::RightButton)
    {
        QMutexLocker lock(&IGisItem::mutexItems);

        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWidget::self().getItemByKey(key));
        if(wpt != nullptr)
        {
            wpt->setProximity(origProximity);
        }
        canvas->resetMouse();
        canvas->update();
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
    else
    {
        QMutexLocker lock(&IGisItem::mutexItems);

        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWidget::self().getItemByKey(key));
        if(wpt)
        {
            QPointF pos = point;
            gis->convertPx2Rad(pos);
            qreal dist = GPS_Math_Distance(pos.rx(),pos.ry(),wptPosition.rx(),wptPosition.ry());
            wpt->setProximity(dist);
        }
        else
        {
            canvas->resetMouse();
        }
    }

    lastPoint = point;
    canvas->update();
}

void CMouseRadiusWpt::mouseReleaseEvent(QMouseEvent *e)
{
    point = e->pos();
    if(!mapDidMove)
    {
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
