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

#include "mouse/CMouseEditTrk.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/CGisWidget.h"
#include "canvas/CCanvas.h"

#include <QtWidgets>

CMouseEditTrk::CMouseEditTrk(const QPointF& point, CGisDraw * gis, CCanvas * parent)
    : IMouseEditLine(point, gis, parent)
{
    cursor = cursor1 = QCursor(QPixmap(":/cursors/cursorMoveLine.png"),0,0);
}

CMouseEditTrk::CMouseEditTrk(CGisItemTrk &trk, CGisDraw * gis, CCanvas * parent)
    : IMouseEditLine(trk, gis, parent)
{
    cursor = cursor1 = QCursor(QPixmap(":/cursors/cursorMoveLine.png"),0,0);
    key         = trk.getKey();

    // reset any focus the track might have.
    trk.setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseMove);
    trk.setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseClick);
    if(trk.hasUserFocus())
    {
        trk.gainUserFocus(false);
    }

    canvas->reportStatus(key.item, tr("<b>Edit Track Points</b><br/>Select a track point for more options.<br/>"));
    /*
        trigger complete update of GIS components to make sure all changes to
        the originating object are reflected on the canvas
    */
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
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
    IMouseEditLine::slotAbort();
}

void CMouseEditTrk::slotCopyToOrig()
{
    canvas->reportStatus(key.item,"");
    IMouseEditLine::slotCopyToOrig();
}


void CMouseEditTrk::slotCopyToNew()
{
    canvas->reportStatus(key.item,"");

    if(coords1.size() < 2)
    {
        return;
    }

    IGisProject * project = CGisWidget::self().selectProject();
    if(project == 0)
    {
        return;
    }

    /// @todo make this independent from track
    QString name;
    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk != 0)
    {
        name = trk->getName();
    }

    name = QInputDialog::getText(0, QObject::tr("Edit name..."), QObject::tr("Enter new track name."), QLineEdit::Normal, name);

    if(name.isEmpty())
    {
        return;
    }

    new CGisItemTrk(coords1,name, project, -1);

    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);            
}
