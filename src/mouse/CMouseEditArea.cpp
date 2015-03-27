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

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CGisWidget.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "mouse/CMouseEditArea.h"

#include <QtWidgets>

CMouseEditArea::CMouseEditArea(const QPointF& point, CGisDraw * gis, CCanvas * parent)
    : IMouseEditLine(point, gis, parent)
{
    cursor = cursor1 = QCursor(QPixmap(":/cursors/cursorMoveArea.png"),0,0);
}

CMouseEditArea::CMouseEditArea(CGisItemOvlArea &area, CGisDraw * gis, CCanvas * parent)
    : IMouseEditLine(area, gis, parent)
{
    cursor = cursor1 = QCursor(QPixmap(":/cursors/cursorMoveArea.png"),0,0);
    key         = area.getKey();
    canvas->reportStatus(key.item, tr("<b>Edit Area</b><br/>Select a corner point for more options.<br/>"));
}

CMouseEditArea::~CMouseEditArea()
{
    canvas->reportStatus(key.item, "");
}

void CMouseEditArea::mousePressEvent(QMouseEvent * e)
{
    canvas->reportStatus(key.item, "");
    IMouseEditLine::mousePressEvent(e);
}


void CMouseEditArea::drawLine(const QPolygonF &l, QPainter& p)
{
    p.setPen(QPen(Qt::magenta, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(QBrush(Qt::magenta, Qt::BDiagPattern));
    p.drawPolygon(l);
}


IGisLine * CMouseEditArea::getGisLine()
{
    return dynamic_cast<CGisItemOvlArea*>(CGisWidget::self().getItemByKey(key));
}

void CMouseEditArea::slotAbort()
{
    canvas->reportStatus(key.item,"");
    IMouseEditLine::slotAbort();
}

void CMouseEditArea::slotCopyToOrig()
{
    canvas->reportStatus(key.item,"");
    IMouseEditLine::slotCopyToOrig();
}

void CMouseEditArea::slotCopyToNew()
{
    canvas->reportStatus(key.item,"");

    if(coords1.size() < 3)
    {
        return;
    }

    IGisProject * project = CGisWidget::self().selectProject();
    if(project == 0)
    {
        return;
    }

    QString name;
    CGisItemOvlArea * area = dynamic_cast<CGisItemOvlArea*>(CGisWidget::self().getItemByKey(key));
    if(area != 0)
    {
        name = area->getName();
    }

    name = QInputDialog::getText(&CMainWindow::self(), QObject::tr("Edit name..."), QObject::tr("Enter new area name."), QLineEdit::Normal, name);

    if(name.isEmpty())
    {
        return;
    }

    new CGisItemOvlArea(coords1,name, project, NOIDX);

    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}
