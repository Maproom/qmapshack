/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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
#include "gis/rte/CGisItemRte.h"
#include "mouse/CMouseEditRte.h"
#include "mouse/line/CScrOptEditLine.h"

#include <QtWidgets>


CMouseEditRte::CMouseEditRte(const QPointF &point, CGisDraw *gis, CCanvas *parent)
    : IMouseEditLine(IGisItem::key_t(), point, true, tr("Route"), gis, parent)
{
    startNewLine(point);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

CMouseEditRte::CMouseEditRte(CGisItemRte &rte, CGisDraw * gis, CCanvas * parent)
    : IMouseEditLine(rte.getKey(), rte, true, tr("Route"), gis, parent)
{
    canvas->reportStatus(key.item, tr("<b>Edit Route Points</b><br/>Select a function and a routing mode via the tool buttons. Next select a point of the line. Only points marked with a large square can be changed. The ones with a black dot are subpoints introduced by routing.<br/>"));

    if(!points.first().subpts.isEmpty())
    {
        scrOptEditLine->toolAutoRoute->setChecked(true);
    }
    else
    {
        scrOptEditLine->toolNoRoute->setChecked(true);
    }

    /*
        trigger complete update of GIS components to make sure all changes to
        the originating object are reflected on the canvas
     */
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}


CMouseEditRte::~CMouseEditRte()
{
}

void CMouseEditRte::mousePressEvent(QMouseEvent * e)
{
    canvas->reportStatus(key.item, "");
    IMouseEditLine::mousePressEvent(e);
}

IGisLine * CMouseEditRte::getGisLine()
{
    return dynamic_cast<CGisItemRte*>(CGisWidget::self().getItemByKey(key));
}

void CMouseEditRte::slotAbort()
{
    canvas->reportStatus(key.item,"");
    IMouseEditLine::slotAbortEx(false);
}

void CMouseEditRte::slotCopyToOrig()
{
    canvas->reportStatus(key.item,"");
    IMouseEditLine::slotCopyToOrig();
}


void CMouseEditRte::slotCopyToNew()
{
    QMutexLocker lock(&IGisItem::mutexItems);

    canvas->reportStatus(key.item,"");

    if(points.size() < 2)
    {
        return;
    }

    IGisProject * project = CGisWidget::self().selectProject();
    if(nullptr == project)
    {
        return;
    }

    /// @todo make this independent from track
    QString name;
    CGisItemRte * rte = dynamic_cast<CGisItemRte*>(CGisWidget::self().getItemByKey(key));
    if(rte != 0)
    {
        name = rte->getName();
    }

    name = QInputDialog::getText(CMainWindow::getBestWidgetForParent(), QObject::tr("Edit name..."), QObject::tr("Enter new route name."), QLineEdit::Normal, name);

    if(name.isEmpty())
    {
        return;
    }

    new CGisItemRte(points,name, project, NOIDX);

    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}
