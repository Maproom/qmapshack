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

#ifndef CMOUSESELECT_H
#define CMOUSESELECT_H

#include "gis/IGisItem.h"
#include "mouse/IMouse.h"

class CGisDraw;
class CCanvas;
class CScrOptSelect;

class CMouseSelect : public IMouse
{
public:
    CMouseSelect(CGisDraw * gis, CCanvas * parent);
    virtual ~CMouseSelect();

    void draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect) override;
    void mousePressEvent(QMouseEvent * e) override;
    void mouseMoveEvent(QMouseEvent * e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent * e) override;

private slots:
    void slotModeSwitch(IGisItem::selection_e mode, bool checked);
    void slotCopy() const;
    void slotDelete() const;

private:
    void rectRad2Px(const QRectF& rectSrc, QRectF& rectTar) const;
    void placeScrOpt();
    void findItems();

    QPoint lastPos;
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

    QPointer<CScrOptSelect> scrOptSelect;

    IGisItem::selection_e modeSelection = IGisItem::eSelectionIntersect;
    IGisItem::selection_e modeLastSel   = IGisItem::eSelectionNone;

    QList<IGisItem::key_t> itemKeys;

    QRectF rectLastSel;
};

#endif //CMOUSESELECT_H

