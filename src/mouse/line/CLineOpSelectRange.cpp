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

#include "canvas/CCanvas.h"
#include "mouse/line/CLineOpSelectRange.h"
#include "mouse/line/CScrOptRangeLine.h"
#include "mouse/line/IMouseEditLine.h"

#include <QtWidgets>

CLineOpSelectRange::CLineOpSelectRange(SGisLine& points, CGisDraw *gis, CCanvas * canvas, IMouseEditLine * parent)
    : ILineOp(points, gis, canvas, parent)
{
    cursor = QCursor(QPixmap(":/cursors/cursorSelectRange.png"),0,0);
}

CLineOpSelectRange::~CLineOpSelectRange()
{
}

void CLineOpSelectRange::mouseReleaseEventEx(QMouseEvent * e)
{
    if(e->button() == Qt::LeftButton)
    {
        switch(state)
        {
        case eStateIdle:
        {
            if(idxFocus != NOIDX)
            {
                state = eState1st;
            }
            break;
        }

        case eState1st:
        {
            if(idx2nd < 0 || points.size() <= idx2nd)
            {
                break;
            }

            qint32 d = qAbs(idxFocus - idx2nd);
            if(d < 1)
            {
                resetState();
                return;
            }

            scrOptRangeLine = new CScrOptRangeLine(points[idx2nd].pixel, parentHandler, canvas);
            connect(scrOptRangeLine->toolDelete,    &QToolButton::clicked, this,            &CLineOpSelectRange::slotDelete);
            connect(scrOptRangeLine->toolCalcRoute, &QToolButton::clicked, this,            &CLineOpSelectRange::slotCalc);
            connect(scrOptRangeLine->toolDelete,    &QToolButton::clicked, scrOptRangeLine.data(), &CScrOptRangeLine::hide);
            connect(scrOptRangeLine->toolCalcRoute, &QToolButton::clicked, scrOptRangeLine.data(), &CScrOptRangeLine::hide);

            if(d < 2)
            {
                scrOptRangeLine->toolDelete->setEnabled(false);
            }

            state = eState2nd;
            break;
        }
        }
    }
    else if(e->button() == Qt::RightButton)
    {
        resetState();
    }
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

bool CLineOpSelectRange::abortStep()
{
    if(state != eStateIdle)
    {
        resetState();
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
        return true;
    }
    return false;
}

void CLineOpSelectRange::mouseMoveEventEx(QMouseEvent * e)
{
    switch(state)
    {
    case eStateIdle:
    {
        // no point selected yet, find point to highlight
        idxFocus = isCloseTo(e->pos());
        break;
    }

    case eState1st:
    {
        idx2nd = isCloseTo(e->pos());
        if(idx2nd == NOIDX)
        {
            idx2nd = isCloseToLine(e->pos());
            if((idx2nd != NOIDX) && ((idx2nd + 1) < points.size()))
            {
                idx2nd++;
            }
        }
        break;
    }
    }

    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpSelectRange::wheelEvent(QWheelEvent * e)
{
    if(state == eState2nd)
    {
        resetState();
    }
}

void CLineOpSelectRange::keyPressEvent(QKeyEvent * e)
{
    if(state == eState2nd)
    {
        resetState();
    }
}


void CLineOpSelectRange::drawFg(QPainter& p)
{
    if(idxFocus == NOIDX)
    {
        return;
    }

    switch(state)
    {
    case eStateIdle:
    {
        const IGisLine::point_t& pt = points[idxFocus];
        drawSinglePointLarge(pt.pixel, p);
        break;
    }

    case eState2nd:
    {
        if(!scrOptRangeLine.isNull())
        {
            scrOptRangeLine->draw(p);
        }
    }

    case eState1st:
    {
        if(idx2nd != NOIDX)
        {
            qint32 idx1 = qMin(idxFocus, idx2nd);
            qint32 idx2 = qMax(idxFocus, idx2nd);

            QPolygonF seg;
            for(int i = idx1; i < idx2; i++)
            {
                const IGisLine::point_t& point = points[i];
                seg << point.pixel;
                for(const IGisLine::subpt_t& subpt : point.subpts)
                {
                    seg << subpt.pixel;
                }
            }

            seg << points[idx2].pixel;

            p.setPen(QPen(Qt::darkGreen, 11, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.drawPolyline(seg);

            p.setPen(QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.drawPolyline(seg);

            QRectF r(0,0,7,7);
            p.setPen(QPen(Qt::darkGreen,2));
            p.setBrush(Qt::darkGreen);
            for(const QPointF &pt : seg)
            {
                r.moveCenter(pt);
                p.drawRect(r);
            }
            p.setPen(Qt::NoPen);
            p.setBrush(Qt::black);
            for(const QPointF &pt : seg)
            {
                r.moveCenter(pt);
                p.drawRect(r);
            }
        }
        break;
    }
    }
}

void CLineOpSelectRange::resetState()
{
    if(!scrOptRangeLine.isNull())
    {
        scrOptRangeLine->deleteLater();
    }
    idxFocus = NOIDX;
    idx2nd   = NOIDX;
    state    = eStateIdle;
}

void CLineOpSelectRange::slotDelete()
{
    qint32 idx = qMin(idxFocus, idx2nd);
    qint32 N   = qAbs(idxFocus - idx2nd) - 1;

    points[idx].subpts.clear();
    points.remove(idx + 1, N);
    parentHandler->storeToHistory(points);


    resetState();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpSelectRange::slotCalc()
{
    qint32 idx = qMin(idxFocus, idx2nd);
    qint32 N   = qAbs(idxFocus - idx2nd) - 1;

    points.remove(idx + 1, N);

    finalizeOperation(idx);
    parentHandler->storeToHistory(points);

    resetState();
}
