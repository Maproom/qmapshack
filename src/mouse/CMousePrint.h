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

#ifndef CMOUSEPRINT_H
#define CMOUSEPRINT_H

#include "mouse/IMouse.h"

class CGisDraw;
class CCanvas;

class CMousePrint : public IMouse
{
    Q_OBJECT
public:
    CMousePrint(CGisDraw * gis, CCanvas * parent);
    virtual ~CMousePrint();

    void draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect) override;
    void mousePressEvent(QMouseEvent * e) override;
    void mouseMoveEvent(QMouseEvent * e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent * e) override;

private:
    QPoint lastPos;
    QPointF offset;
    QPointF posInitial;

    QRectF rectSelection;
    QRectF rectTopLeft {0,0,20,20};
    QRectF rectTopRight {0,0,20,20};
    QRectF rectBottomLeft {0,0,20,20};
    QRectF rectBottomRight {0,0,20,20};
    QRectF rectPrintButton {0,0,48,48};
    QRectF rectImageButton {0,0,48,48};

    enum state_e
    {
        eStateIdle
        ,eStateInitial
        ,eStateMap
        ,eStateMapMoving
        ,eStateResize
    };

    state_e state = eStateIdle;

    enum corner_e
    {
        eCornerNone
        , eCornerTopLeft
        , eCornerTopRight
        , eCornerBottomLeft
        , eCornerBottomRight
        , eCornerPrint
        , eCornerImage
    };

    corner_e corner = eCornerNone;
};

#endif //CMOUSEPRINT_H

