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
#include "mouse/CMouseRadiusWpt.h"
#include "units/IUnit.h"

#include <QtWidgets>

CMouseRadiusWpt::CMouseRadiusWpt(CGisItemWpt& wpt, CGisDraw* gis, CCanvas* canvas, CMouseAdapter* mouse)
    : IMouse(gis, canvas, mouse),
    key(wpt.getKey()),
    wptPosition(wpt.getPosition() * DEG_TO_RAD),
    avoid(wpt.isNogo()),
    initial(true)
{
    startPos = GPS_Math_Wpt_Projection(wptPosition, wpt.getProximity(), 90 * DEG_TO_RAD);
    gis->convertRad2Px(startPos);
    cursor = QCursor(QPixmap(":/cursors/cursorRadiusWpt.png"), 0, 0);
    wpt.setHideArea(true);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

CMouseRadiusWpt::~CMouseRadiusWpt()
{
}

void CMouseRadiusWpt::draw(QPainter& p, CCanvas::redraw_e, const QRect&)
{
    QPointF pos = initial ? startPos : mouse->getPoint();
    gis->convertPx2Rad(pos);
    dist = GPS_Math_Distance(pos.rx(), pos.ry(), wptPosition.x(), wptPosition.y());
    QPointF screenPos = wptPosition;
    gis->convertRad2Px(screenPos);
    qreal radius = CGisItemWpt::calcRadius(wptPosition, screenPos, dist, gis);

    CGisItemWpt::drawCircle(p, screenPos, radius, avoid, initial);
}

void CMouseRadiusWpt::rightButtonDown(const QPoint& point)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(CGisWorkspace::self().getItemByKey(key));
    if(wpt != nullptr)
    {
        wpt->setHideArea(false);
    }
    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    initial = false;
}

void CMouseRadiusWpt::mouseMoved(const QPoint& pos)
{
    canvas->update();
    initial = false;
}

void CMouseRadiusWpt::mouseDragged(const QPoint& start, const QPoint& last, const QPoint& end)
{
    IMouse::mouseDragged(start, last, end);
    initial = false;
}

void CMouseRadiusWpt::leftClicked(const QPoint& point)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(CGisWorkspace::self().getItemByKey(key));
    if(wpt != nullptr)
    {
        wpt->setProximity(dist);
        wpt->setHideArea(false);
    }
    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    initial = false;
}

void CMouseRadiusWpt::scaleChanged()
{
    canvas->update();
}
