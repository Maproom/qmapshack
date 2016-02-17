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
#include "gis/trk/CGisItemTrk.h"
#include "mouse/CMouseEditTrk.h"

#include <QtWidgets>

CMouseEditTrk::CMouseEditTrk(const QPointF& point, CGisDraw * gis, CCanvas * parent)
    : IMouseEditLine(IGisItem::key_t(), point, true, tr("Track"), gis, parent)
{
    startNewLine(point);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

CMouseEditTrk::CMouseEditTrk(CGisItemTrk &trk, CGisDraw * gis, CCanvas * parent)
    : IMouseEditLine(trk.getKey(), trk, true, tr("Track"), gis, parent)
    , isNewLine(false)
{
    canvas->reportStatus(key.item, tr("<b>Edit Track Points</b><br/>Select a function and a routing mode via the tool buttons. Next select a point of the line. Only points marked with a large square can be changed. The ones with a black dot are subpoints introduced by routing.<br/>"));

    // reset any focus the track might have.
    trk.setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseMove, "CMouseEditTrk");
    trk.setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseClick, "CMouseEditTrk");
    trk.looseUserFocus();

    /*
        trigger complete update of GIS components to make sure all changes to
        the originating object are reflected on the canvas
     */
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

CMouseEditTrk::~CMouseEditTrk()
{
//    canvas->reportStatus(key,"");
}

void CMouseEditTrk::mousePressEvent(QMouseEvent * e)
{
    canvas->reportStatus(key.item, "");
    IMouseEditLine::mousePressEvent(e);
}

IGisLine * CMouseEditTrk::getGisLine()
{
    return dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
}

void CMouseEditTrk::slotAbort()
{
    canvas->reportStatus(key.item,"");
    IMouseEditLine::slotAbortEx(false);
}

void CMouseEditTrk::slotCopyToOrig()
{
    canvas->reportStatus(key.item,"");

    if(!isNewLine)
    {
        int res = QMessageBox::warning(canvas, tr("Warning!"), tr("This will replace all data of the original by a simple line of coordinates. All other data will be lost permanently."), QMessageBox::Ok|QMessageBox::Abort, QMessageBox::Ok);

        if(res != QMessageBox::Ok)
        {
            return;
        }
    }
    IMouseEditLine::slotCopyToOrig();
}


void CMouseEditTrk::slotCopyToNew()
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
    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk != nullptr)
    {
        name = trk->getName();
    }

    name = QInputDialog::getText(CMainWindow::getBestWidgetForParent(), tr("Edit name..."), tr("Enter new track name."), QLineEdit::Normal, name);

    if(name.isEmpty())
    {
        return;
    }

    CMainWindow::self().getElevationAt(points);

    new CGisItemTrk(points,name, project, NOIDX);

    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}
