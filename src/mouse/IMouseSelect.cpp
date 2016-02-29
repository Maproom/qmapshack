/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/CGisDraw.h"
#include "mouse/IMouseSelect.h"
#include "mouse/IScrOpt.h"

#include <QtWidgets>

IMouseSelect::IMouseSelect(CGisDraw *gis, CCanvas *canvas)
    : IMouse(gis, canvas)
{
}

IMouseSelect::~IMouseSelect()
{
    canvas->reportStatus("IMouseSelect", "");
    delete scrOpt;
}

void IMouseSelect::rectRad2Px(const QRectF& rectSrc, QRectF& rectTar) const
{
    QPointF pt1 = rectSrc.topLeft();
    QPointF pt2 = rectSrc.bottomRight();

    gis->convertRad2Px(pt1);
    gis->convertRad2Px(pt2);

    rectTar = QRectF(pt1,pt2);
}

void IMouseSelect::placeScrOpt()
{
    if(scrOpt.isNull())
    {
        return;
    }

    if((state == eStateMap) || (state == eStateMapMoving))
    {
        QRectF rectSel;
        rectRad2Px(rectSelection, rectSel);
        scrOpt->move(rectSel.topRight().toPoint());
        scrOpt->show();
    }
    else
    {
        scrOpt->hide();
    }
}


void IMouseSelect::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect)
{
    if(rectSelection.isNull())
    {
        return;
    }

    QRectF rectSel;
    rectRad2Px(rectSelection, rectSel);

    QRectF rectScr = canvas->rect();

    rectTopLeft.moveTopLeft(rectSel.topLeft());
    rectTopRight.moveTopRight(rectSel.topRight());
    rectBottomLeft.moveBottomLeft(rectSel.bottomLeft());
    rectBottomRight.moveBottomRight(rectSel.bottomRight());


    QPainterPath path;
    path.addRect(rectScr);
    path.addRect(rectSel);

    p.setPen(Qt::black);
    p.setBrush(QColor(0,0,0,128));
    p.drawPath(path);

    p.setBrush(Qt::lightGray);
    p.drawRect(rectTopLeft);
    p.drawRect(rectTopRight);
    p.drawRect(rectBottomLeft);
    p.drawRect(rectBottomRight);

    p.setBrush(Qt::red);
    switch(corner)
    {
    case eCornerTopLeft:
        p.drawRect(rectTopLeft);
        break;

    case eCornerTopRight:
        p.drawRect(rectTopRight);
        break;

    case eCornerBottomLeft:
        p.drawRect(rectBottomLeft);
        break;

    case eCornerBottomRight:
        p.drawRect(rectBottomRight);
        break;
    }

    placeScrOpt();
}

void IMouseSelect::mousePressEvent(QMouseEvent * e)
{
    e->accept();

    canvas->reportStatus("IMouseSelect", "");

    if(e->button() == Qt::RightButton)
    {
        canvas->resetMouse();
        canvas->update();
    }
    else if(e->button() == Qt::LeftButton)
    {
        switch(state)
        {
        case eStateIdle:
        {
            QPointF pos = e->pos();
            gis->convertPx2Rad(pos);
            rectSelection.setTopLeft(pos);
            rectSelection.setBottomRight(pos);
            posInitial = pos;
            state = eStateInitial;
            break;
        }

        case eStateMap:
        {
            if(corner != eCornerNone)
            {
                state = eStateResize;
            }
            else
            {
                lastPos = e->pos();
                state = eStateMapMoving;
            }
            break;
        }
        }
    }
}


void IMouseSelect::mouseMoveEvent(QMouseEvent * e)
{
    e->accept();

    switch(state)
    {
    case eStateInitial:
    {
        QPointF pos = e->pos();
        gis->convertPx2Rad(pos);

        if(pos.x() < posInitial.x())
        {
            rectSelection.setLeft(pos.x());
        }
        else
        {
            rectSelection.setRight(pos.x());
        }

        if(pos.y() < posInitial.y())
        {
            rectSelection.setBottom(pos.y());
        }
        else
        {
            rectSelection.setTop(pos.y());
        }

        canvas->update();
        break;
    }

    case eStateMap:
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
            canvas->update();
        }
        break;
    }

    case eStateMapMoving:
    {
        QPoint pos = e->pos();

        if(pos != lastPos)
        {
            QPoint delta = pos - lastPos;
            canvas->moveMap(delta);
            lastPos     = pos;
        }
        break;
    }

    case eStateResize:
    {
        QPointF pos = e->pos() - offset;
        gis->convertPx2Rad(pos);
        switch(corner)
        {
        case eCornerTopLeft:
            rectSelection.setTopLeft(pos);
            break;

        case eCornerTopRight:
            rectSelection.setTopRight(pos);
            break;

        case eCornerBottomLeft:
            rectSelection.setBottomLeft(pos);
            break;

        case eCornerBottomRight:
            rectSelection.setBottomRight(pos);
            break;
        }

        canvas->update();
        break;
    }
    }
}

void IMouseSelect::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();

    if(!rectSelection.isNull())
    {
        QPointF pt1 = rectSelection.topLeft();
        QPointF pt2 = rectSelection.bottomRight();

        gis->convertRad2Px(pt1);
        gis->convertRad2Px(pt2);

        QRectF rectSel(pt1,pt2);

        if(rectSel.width() < 40 || rectSel.height() < 40)
        {
            rectSelection = QRectF();
        }
    }

    state = rectSelection.isNull() ? eStateIdle : eStateMap;

    canvas->update();
}

void IMouseSelect::wheelEvent(QWheelEvent * e)
{
}

