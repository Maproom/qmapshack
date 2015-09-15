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

#include "gis/CGisDraw.h"
#include "mouse/CMousePrint.h"
#include "print/CPrintDialog.h"

#include <QtWidgets>

CMousePrint::CMousePrint(CGisDraw *gis, CCanvas *parent)
    : IMouse(gis, parent)
    , rectTopLeft(0,0,20,20)
    , rectTopRight(0,0,20,20)
    , rectBottomLeft(0,0,20,20)
    , rectBottomRight(0,0,20,20)
    , rectPrintButton(0,0,48,48)
    , rectImageButton(0,0,48,48)
    , state(eStateIdle)
    , corner(eCornerNone)
{
    cursor      = QCursor(QPixmap("://cursors/cursorSave.png"),0,0);

    canvas->reportStatus("CMousePrint", tr("<b>Save(Print) Map</b><br/>Select a rectangular area on the map. Use the left mouse button and move the mouse. Abort with a right click. Adjust the selection by point-click-move on the corners. Save/print the selection by a left click on the disc/printer icon in the center of the selection."));
}

CMousePrint::~CMousePrint()
{
    canvas->reportStatus("CMousePrint", "");
}

void CMousePrint::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect)
{
    if(rectSelection.isNull())
    {
        return;
    }

    QPointF pt1 = rectSelection.topLeft();
    QPointF pt2 = rectSelection.bottomRight();

    gis->convertRad2Px(pt1);
    gis->convertRad2Px(pt2);

    QRectF rectSel(pt1,pt2);
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

    if(rectSel.width() > 100 && rectSel.height() > 50)
    {
        rectPrintButton.moveCenter(rectSel.center() + QPointF(30,0));
        p.setPen(corner == eCornerPrint ? QPen(Qt::red,3) : QPen(Qt::darkBlue,2));
        p.setBrush(Qt::white);
        p.drawRect(rectPrintButton.adjusted(-3,-3,3,3));
        p.drawPixmap(rectPrintButton.topLeft(), QPixmap("://icons/48x48/Print.png"));

        rectImageButton.moveCenter(rectSel.center() - QPointF(30,0));
        p.setPen(corner == eCornerImage ? QPen(Qt::red,3) : QPen(Qt::darkBlue,2));
        p.setBrush(Qt::white);
        p.drawRect(rectImageButton.adjusted(-3,-3,3,3));
        p.drawPixmap(rectImageButton.topLeft(), QPixmap("://icons/48x48/Save.png"));
    }
}

void CMousePrint::mousePressEvent(QMouseEvent * e)
{
    e->accept();

    canvas->reportStatus("CMousePrint", "");

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
                if(corner == eCornerPrint)
                {
                    CPrintDialog dlg(CPrintDialog::eTypePrint, rectSelection, canvas);
                    dlg.exec();
                    canvas->resetMouse();
                }
                else if(corner == eCornerImage)
                {
                    CPrintDialog dlg(CPrintDialog::eTypeImage, rectSelection, canvas);
                    dlg.exec();
                    canvas->resetMouse();
                }
                else
                {
                    state = eStateResize;
                }
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

void CMousePrint::mouseMoveEvent(QMouseEvent * e)
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
        else if(rectPrintButton.contains(pos))
        {
            corner = eCornerPrint;
        }
        else if(rectImageButton.contains(pos))
        {
            corner = eCornerImage;
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

void CMousePrint::mouseReleaseEvent(QMouseEvent *e)
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

void CMousePrint::wheelEvent(QWheelEvent * e)
{
}


