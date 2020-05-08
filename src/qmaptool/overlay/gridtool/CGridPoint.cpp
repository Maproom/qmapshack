/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "canvas/IDrawContext.h"
#include "helpers/CDraw.h"
#include "items/CItemRefMap.h"
#include "overlay/gridtool/CGridPoint.h"

#include <QtWidgets>

CGridPoint::CGridPoint()
{
}

void CGridPoint::registerItem(CItemRefMap * item)
{
    this->item = item;

    if(item != nullptr)
    {
        context = item->getDrawContext();
        if(context == nullptr)
        {
            this->item = nullptr;
        }
    }
    else
    {
        context = nullptr;
    }
}

bool CGridPoint::drawFx(QPainter& p, CCanvas::redraw_e needsRedraw)
{
    if(ptFocus1 != NOPOINTF)
    {
        QPointF pt = ptFocus1;
        context->convertMap2Screen(pt);
        CDraw::drawCrossHairDot(p, pt);
    }

    if(ptPoint != NOPOINTF)
    {
        QPointF pt = ptPoint;
        context->convertMap2Screen(pt);

        QRectF dot1(0,0,7,7);
        dot1.moveCenter(pt);

        if(state == eStateHighlight)
        {
            p.setPen(QPen(QColor("#ffaa00"), 2));
            p.setBrush(QColor("#ffaa00"));
        }
        else
        {
            p.setPen(QPen(Qt::white, 1));
            p.setBrush(QColor("#ffaa00"));
        }

        p.drawRect(dot1);
    }

    return true;
}

void CGridPoint::mouseMoveEventFx(QMouseEvent *e)
{
    QPointF pt = e->pos();


    switch(state)
    {
    case eStateMove:
    case eStateNotSet:
        context->convertScreen2Map(pt);
        ptFocus1 = pt;
        break;

    case eStateSet:
    {
        QPointF point = ptPoint;
        context->convertMap2Screen(point);
        if((point - pt).manhattanLength() < 30)
        {
            state = eStateHighlight;
        }
        break;
    }

    case eStateHighlight:
    {
        QPointF point = ptPoint;
        context->convertMap2Screen(point);
        if((point - pt).manhattanLength() >= 30)
        {
            state = eStateSet;
        }
        break;
    }
    }

    context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
}

void CGridPoint::mouseReleaseEventFx(QMouseEvent *e)
{
    switch(state)
    {
    case eStateMove:
    case eStateNotSet:
        ptPoint     = ptFocus1;
        ptFocus1    = NOPOINTF;
        state       = eStateSet;
        CCanvas::restoreOverrideCursor("CGridPoint::mouseReleaseEventFx");
        break;

    case eStateSet:
        break;

    case eStateHighlight:
        ptFocus1    = ptPoint;
        ptPoint     = NOPOINTF;
        state       = eStateMove;
        CCanvas::setOverrideCursor(Qt::BlankCursor, "CGridPoint::mouseReleaseEventFx");
        break;
    }

    context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
}

void CGridPoint::leaveEventFx(QEvent *e)
{
    ptFocus1 = NOPOINTF;

    switch(state)
    {
    case eStateNotSet:
    case eStateSet:
        break;

    case eStateHighlight:
    case eStateMove:
        state = eStateSet;
        break;
    }

    context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
}

QCursor CGridPoint::getCursorFx()
{
    switch(state)
    {
    case eStateNotSet:
        return Qt::BlankCursor;

    case eStateSet:
        return Qt::ArrowCursor;

    case eStateHighlight:
        return Qt::ArrowCursor;

    case eStateMove:
        return Qt::BlankCursor;
    }

    return Qt::ArrowCursor;
}
