/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler <oliver.eichler@gmx.de>
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

#ifndef IMOUSESELECT_H
#define IMOUSESELECT_H

#include "canvas/CCanvas.h"
#include "mouse/IMouse.h"

class CGisDraw;

class IMouseSelect : public IMouse
{
    Q_OBJECT
public:
    IMouseSelect(CGisDraw* gis, CCanvas* canvas, CMouseAdapter* mouse);
    virtual ~IMouseSelect();

    void draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect& rect) override;

    void mouseMoved(const QPoint& pos) override;
    void mouseDragged(const QPoint& start, const QPoint& last, const QPoint& end) override;
    void leftButtonDown(const QPoint& pos) override;
    void dragFinished(const QPoint& pos) override;

protected:
    void rectRad2Px(const QRectF& rectSrc, QRectF& rectTar) const;
    void rectRad2Deg(const QRectF& rectSrc, QRectF& rectTar) const;
    void placeScrOpt();

    QPointF offset;
    QPointF posInitial;

    QRectF rectSelection;
    QRectF rectTopLeft     {0, 0, 20, 20};
    QRectF rectTopRight    {0, 0, 20, 20};
    QRectF rectBottomLeft  {0, 0, 20, 20};
    QRectF rectBottomRight {0, 0, 20, 20};

    enum state_e
    {
        eStateIdle
        , eStateInitial
        , eStateMap
        , eStateMapMoving
        , eStateResize
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

    QPointer<IScrOpt> scrOpt;
};

#endif //IMOUSESELECT_H

