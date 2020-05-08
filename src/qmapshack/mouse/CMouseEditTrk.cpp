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
#include "gis/trk/CGisItemTrk.h"
#include "mouse/CMouseEditTrk.h"

#include <QtWidgets>

CMouseEditTrk::CMouseEditTrk(const QPointF& point, CGisDraw * gis, CCanvas * canvas, CMouseAdapter *mouse)
    : IMouseEditLine(IGisItem::key_t(), point, true, tr("Track"), gis, canvas, mouse)
{
    startNewLine(point);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

CMouseEditTrk::CMouseEditTrk(CGisItemTrk &trk, CGisDraw * gis, CCanvas * canvas, CMouseAdapter *mouse)
    : IMouseEditLine(trk.getKey(), trk, true, tr("Track"), gis, canvas, mouse)
    , isNewLine(false)
{
    canvas->reportStatus(key.item, tr("<b>Edit Track Points</b><br/>Select a function and a routing mode via the tool buttons. Next select a point of the line. Only points marked with a large square can be changed. The ones with a black dot are subpoints introduced by routing.<br/>") + docPanning);

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

IGisLine * CMouseEditTrk::getGisLine() const
{
    return dynamic_cast<CGisItemTrk*>(CGisWorkspace::self().getItemByKey(key));
}

void CMouseEditTrk::slotAbort()
{
    canvas->reportStatus(key.item, "");
    IMouseEditLine::slotAbortEx(false);
}

void CMouseEditTrk::slotCopyToOrig()
{
    canvas->reportStatus(key.item, "");

    if(!isNewLine)
    {
        QMessageBox::StandardButton button = QMessageBox::warning(canvas, tr("Warning!"), tr("This will replace all data of the original by a simple line of coordinates. All other data will be lost permanently."), QMessageBox::Ok | QMessageBox::Abort, QMessageBox::Ok);

        if(button != QMessageBox::Ok)
        {
            return;
        }
    }
    IMouseEditLine::slotCopyToOrig();
}


void CMouseEditTrk::slotCopyToNew()
{
    canvas->reportStatus(key.item, "");

    if(points.size() < 2)
    {
        return;
    }


    IGisProject * project = nullptr;

    QString name;
    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWorkspace::self().getItemByKey(key));
    if(trk != nullptr)
    {
        name = trk->getName();
    }

    if(!IGisItem::getNameAndProject(name, project, tr("track")))
    {
        return;
    }


    CMainWindow::self().getElevationAt(points);

    {
        QMutexLocker lock(&IGisItem::mutexItems);
        new CGisItemTrk(points, name, project, NOIDX);
    }
    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}
