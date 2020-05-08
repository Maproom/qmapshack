/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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
#include "CMainWindow.h"
#include "gis/CGisWorkspace.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "mouse/CMouseEditArea.h"

#include <QtWidgets>

CMouseEditArea::CMouseEditArea(const QPointF& point, CGisDraw * gis, CCanvas * canvas, CMouseAdapter *mouse)
    : IMouseEditLine(IGisItem::key_t(), point, false, tr("Area"), gis, canvas, mouse)
{
    startNewLine(point);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

CMouseEditArea::CMouseEditArea(CGisItemOvlArea &area, CGisDraw * gis, CCanvas * canvas, CMouseAdapter *mouse)
    : IMouseEditLine(area.getKey(), area, false, tr("Area"), gis, canvas, mouse)
{
    canvas->reportStatus(key.item, tr("<b>Edit Area</b><br/>Select a function and a routing mode via the tool buttons. Next select a point of the line. Only points marked with a large square can be changed. The ones with a black dot are subpoints introduced by routing.<br/>") + docPanning);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

CMouseEditArea::~CMouseEditArea()
{
    canvas->reportStatus(key.item, "");
}

void CMouseEditArea::drawLine(const QPolygonF &l, const QColor color, int width, QPainter& p)
{
    p.setPen(QPen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(QBrush(Qt::magenta, Qt::BDiagPattern));
    p.drawPolygon(l);
}

IGisLine * CMouseEditArea::getGisLine() const
{
    return dynamic_cast<CGisItemOvlArea*>(CGisWorkspace::self().getItemByKey(key));
}

void CMouseEditArea::slotAbort()
{
    canvas->reportStatus(key.item, "");
    IMouseEditLine::slotAbortEx(false);
}

void CMouseEditArea::slotCopyToOrig()
{
    canvas->reportStatus(key.item, "");
    IMouseEditLine::slotCopyToOrig();
}

void CMouseEditArea::slotCopyToNew()
{
    canvas->reportStatus(key.item, "");

    if(points.size() < 3)
    {
        return;
    }

    IGisProject * project = nullptr;

    QString name;
    CGisItemOvlArea * area = dynamic_cast<CGisItemOvlArea*>(CGisWorkspace::self().getItemByKey(key));
    if(area != nullptr)
    {
        name = area->getName();
    }

    if(!IGisItem::getNameAndProject(name, project, tr("area")))
    {
        return;
    }

    {
        QMutexLocker lock(&IGisItem::mutexItems);
        new CGisItemOvlArea(points, name, project, NOIDX);
    }

    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}
