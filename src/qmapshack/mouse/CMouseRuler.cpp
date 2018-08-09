/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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
#include "GeoMath.h"
#include "helpers/CDraw.h"
#include "mouse/CMouseRuler.h"
#include "units/IUnit.h"

#include <QtWidgets>

CMouseRuler::CMouseRuler(CGisDraw *gis, CCanvas *canvas, CMouseAdapter *mouse)
    : IMouse(gis, canvas, mouse)
{
    cursor = QCursor(QPixmap(":/cursors/cursorRuler.png"),0,0);

    const QString msg = tr(
        "<b>Distance Ruler</b><br/>"
        "Use the left mouse button to add points. "
        "A right click will pause updating the ruler. "
        "Another left click will continue to add points. "
        "Another right click will close the ruler tool."
        );

    canvas->reportStatus("CMouseRuler", msg);
}

CMouseRuler::~CMouseRuler()
{
    canvas->reportStatus("CMouseRuler", "");
}

void CMouseRuler::rightButtonDown(const QPoint& pos)
{
    switch(mode)
    {
    case eModePaused:
        IMouse::rightButtonDown(pos);
        break;

    case eModeEdit:
        mode = eModePaused;
        line.pop_back();
        break;
    }

    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CMouseRuler::leftButtonDown(const QPoint& pos)
{
    mode = eModeEdit;

    QPointF coord = pos;
    gis->convertPx2Rad(coord);

    if(line.isEmpty())
    {
        line << coord;
    }

    line << coord;

    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CMouseRuler::mouseMoved(const QPoint& pos)
{
    if(line.isEmpty() || mode != eModeEdit)
    {
        return;
    }

    QPointF coord = pos;
    gis->convertPx2Rad(coord);

    line.last() = coord;

    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CMouseRuler::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect)
{
    if(line.size() < 2)
    {
        return;
    }

    QPolygonF line1 = line;
    gis->convertRad2Px(line1);

    p.setPen(QPen(Qt::white, 5));
    p.drawPolyline(line1);

    p.setPen(QPen(Qt::blue, 3));
    p.drawPolyline(line1);

    const int N = line.size();
    for(int n = 1; n < N; n++)
    {
        QPointF pt1 = line[n-1];
        QPointF pt2 = line[n];

        qreal a1, a2;
        qreal d = GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt2.y(), a1, a2);

        if(d == 0)
        {
            continue;
        }

        QString val;
        QString unit;
        IUnit::self().meter2distance(d, val, unit);

        QString str = val + unit;

        qreal ele1 = canvas->getElevationAt(pt1);
        qreal ele2 = canvas->getElevationAt(pt2);

        if((ele1 != NOFLOAT) && (ele2 != NOFLOAT))
        {
            int delta = qRound(ele2 - ele1);

            IUnit::self().meter2elevation(delta, val, unit);
            str += QString(", %1 %2%3").arg(delta > 0 ? QChar(0x2197) : QChar(0x2198)).arg(val).arg(unit);
        }

        QLineF seg(line1[n-1], line1[n]);
        p.save();
        p.translate(seg.center().toPoint());
        p.rotate(a1 + ((a1 > 0) ? -90 : 90));
        CDraw::text(str, p, QPoint(0,0), Qt::black);
        p.restore();
    }
}

