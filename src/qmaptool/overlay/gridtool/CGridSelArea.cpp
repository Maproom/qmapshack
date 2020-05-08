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
#include "overlay/gridtool/CGridSelArea.h"

#include <QtWidgets>

CGridSelArea::CGridSelArea(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    labelHelp->setText(tr("Select the area to be covered by the calculated reference points. Simply grab "
                          "the corners of the selection rectangle with a left click and place them where "
                          "you want with a second click."));
}


void CGridSelArea::registerItem(CItemRefMap * item)
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

void CGridSelArea::saveSettings(QSettings& cfg)
{
    cfg.setValue("area", area);
}

void CGridSelArea::loadSettings(QSettings& cfg)
{
    area = cfg.value("area", QRectF()).toRectF();
    emit sigChanged();
}

bool CGridSelArea::drawFx(QPainter& p, CCanvas::redraw_e needsRedraw)
{
    if(area.isEmpty())
    {
        return false;
    }

    QRectF rect = area;
    context->convertMap2Screen(rect);

    rectTopLeft.moveTopLeft(rect.topLeft());
    rectTopRight.moveTopRight(rect.topRight());
    rectBottomLeft.moveBottomLeft(rect.bottomLeft());
    rectBottomRight.moveBottomRight(rect.bottomRight());

    CDraw::drawRectangle(p, rectTopLeft, Qt::black, Qt::lightGray);
    CDraw::drawRectangle(p, rectTopRight, Qt::black, Qt::lightGray);
    CDraw::drawRectangle(p, rectBottomLeft, Qt::black, Qt::lightGray);
    CDraw::drawRectangle(p, rectBottomRight, Qt::black, Qt::lightGray);

    p.setBrush(Qt::red);
    switch(corner)
    {
    case eCornerTopLeft:
        CDraw::drawRectangle(p, rectTopLeft, Qt::black, Qt::red);
        break;

    case eCornerTopRight:
        CDraw::drawRectangle(p, rectTopRight, Qt::black, Qt::red);
        break;

    case eCornerBottomLeft:
        CDraw::drawRectangle(p, rectBottomLeft, Qt::black, Qt::red);
        break;

    case eCornerBottomRight:
        CDraw::drawRectangle(p, rectBottomRight, Qt::black, Qt::red);
        break;
    }

    CDraw::drawRectangle(p, rect, QPen(Qt::black), Qt::NoBrush);

    return true;
}

void CGridSelArea::mouseMoveEventFx(QMouseEvent *e)
{
    switch(state)
    {
    case eStateIdle:
    {
        corner_e _corner = corner;
        QPoint pos = e->pos();
        if(rectTopLeft.contains(pos))
        {
            offset = pos - rectTopLeft.topLeft();
            corner = eCornerTopLeft;
        }
        else if(rectTopRight.contains(pos))
        {
            offset = pos - rectTopRight.topRight();
            corner = eCornerTopRight;
        }
        else if(rectBottomLeft.contains(pos))
        {
            offset = pos - rectBottomLeft.bottomLeft();
            corner = eCornerBottomLeft;
        }
        else if(rectBottomRight.contains(pos))
        {
            offset = pos - rectBottomRight.bottomRight();
            corner = eCornerBottomRight;
        }
        else
        {
            corner = eCornerNone;
        }

        if(corner != _corner)
        {
            context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
        }

        break;
    }

    case eStateMove:
    {
        QPointF pos = e->pos() - offset;
        context->convertScreen2Map(pos);

        switch(corner)
        {
        case eCornerTopLeft:
            area.setTopLeft(pos);
            break;

        case eCornerTopRight:
            area.setTopRight(pos);
            break;

        case eCornerBottomLeft:
            area.setBottomLeft(pos);
            break;

        case eCornerBottomRight:
            area.setBottomRight(pos);
            break;
        }

        emit sigChanged();
        context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
        break;
    }
    }
}

void CGridSelArea::mouseReleaseEventFx(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        switch(state)
        {
        case eStateIdle:
        {
            if(corner != eCornerNone)
            {
                areaSave = area;
                state    = eStateMove;
            }
            break;
        }

        case eStateMove:
        {
            corner = eCornerNone;
            state  = eStateIdle;
            break;
        }
        }
    }
    else
    {
        area   = areaSave;
        state  = eStateIdle;
        corner = eCornerNone;
    }
    context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
    emit sigChanged();
}

void CGridSelArea::leaveEventFx(QEvent *e)
{
}

QCursor CGridSelArea::getCursorFx()
{
    return Qt::ArrowCursor;
}


void CGridSelArea::slotSetArea(const QRectF& rect)
{
    area = rect;
    context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
    emit sigChanged();
}

void CGridSelArea::slotReset()
{
    area    = QRectF();
    state   = eStateIdle;
    corner  = eCornerNone;
    context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
    emit sigChanged();
}
