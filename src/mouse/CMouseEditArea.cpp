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

#include "mouse/CMouseEditArea.h"
#include "gis/CGisWidget.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "canvas/CCanvas.h"


#include <QtWidgets>

CMouseEditArea::CMouseEditArea(const QPointF& point, CGisDraw * gis, CCanvas * parent)
    : IMouseEditLine(point, gis, parent)
{
    cursor = cursor1 = QCursor(QPixmap(":/cursors/cursorMoveArea.png"),0,0);
}

CMouseEditArea::~CMouseEditArea()
{

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

void CMouseEditArea::slotCopyToNew()
{
    if(coords1.size() < 3)
    {
        return;
    }

    CGisProject * project = CGisWidget::self().selectProject();
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

    name = QInputDialog::getText(0, QObject::tr("Edit name..."), QObject::tr("Enter new track name."), QLineEdit::Normal, name);

    if(name.isEmpty())
    {
        return;
    }

    new CGisItemOvlArea(coords1,name, project, -1);

    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}
