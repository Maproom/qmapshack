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
#include "gis/CGisDraw.h"
#include "gis/CGisWorkspace.h"
#include "gis/trk/CGisItemTrk.h"
#include "misc.h"
#include "mouse/CMouseAdapter.h"
#include "mouse/CMouseRangeTrk.h"
#include "mouse/range/CScrOptRangeTool.h"

#include <QtWidgets>


CMouseRangeTrk::CMouseRangeTrk(CGisItemTrk& trk, CGisDraw* gis, CCanvas* canvas, CMouseAdapter* mouse)
    : IMouse(gis, canvas, mouse)
    , owner("CMouseRangeTrk")
{
    setObjectName(owner);
    cursor = QCursor(QPixmap("://cursors/cursorSelectRange.png"), 0, 0);
    key = trk.getKey();

    // switch to full mode to show deleted (hidden) track points, too
    trk.setMode(CGisItemTrk::eModeRange, owner);

    scrOptRangeTool = new CScrOptRangeTool(trk, this, canvas);

    const QString& msg = QString("<b>%1</b><p>%2</p><p>%3</p><p>%4</p>")
                         .arg(tr("Select Range"),
                              tr("Select a first track point with left mouse button "
                                 "and then a second one. Leave the range selection "
                                 "with a click of the right mouse button."),
                              tr("As an alternative you can use the buttons of the "
                                 "range tool to move the start and end point of a "
                                 "range. If you keep the buttons pressed the action "
                                 "will be repeated."),
                              tr("When you are done with the selection you can apply "
                                 "one of the operations of the range tool. "));

    canvas->reportStatus(key.item, msg);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

CMouseRangeTrk::~CMouseRangeTrk()
{
    canvas->reportStatus(key.item, "");

    CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(CGisWorkspace::self().getItemByKey(key));
    if(trk)
    {
        trk->setMode(CGisItemTrk::eModeNormal, owner);
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }

    delete scrOptRangeTool;
}

void CMouseRangeTrk::draw(QPainter& p, CCanvas::redraw_e, const QRect&)
{
    if(!scrOptRangeTool.isNull())
    {
        scrOptRangeTool->draw(p);
    }
}


void CMouseRangeTrk::leftButtonDown(const QPoint& pos)
{
    canvas->reportStatus(key.item, "");
}


void CMouseRangeTrk::mouseMoved(const QPoint& pos)
{
    CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(CGisWorkspace::self().getItemByKey(key));
    if(trk == nullptr)
    {
        return;
    }

    if(is_in(trk->getRangState(), {
        CGisItemTrk::eRangeStateIdle,
        CGisItemTrk::eRangeStateClicked1st,
        CGisItemTrk::eRangeStateClicked2nd,
        CGisItemTrk::eRangeStateMove1st,
        CGisItemTrk::eRangeStateMove2nd,
    }))
    {
        trk->setMouseFocusByPoint(pos, CGisItemTrk::eFocusMouseMove, owner);
        canvas->update();
    }
}

void CMouseRangeTrk::leftClicked(const QPoint& point)
{
    CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(CGisWorkspace::self().getItemByKey(key));
    if(trk == nullptr)
    {
        return;
    }

    trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseClick, owner);
    canvas->update();
}
