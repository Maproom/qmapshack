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

#ifndef CGRIDPOINT_H
#define CGRIDPOINT_H

#include "canvas/CCanvas.h"
#include "units/IUnit.h"

class CItemRefMap;
class IDrawContext;

class CGridPoint
{
public:
    CGridPoint();
    virtual ~CGridPoint() = default;

    void registerItem(CItemRefMap * item);

    bool drawFx(QPainter& p, CCanvas::redraw_e needsRedraw);
    void mouseMoveEventFx(QMouseEvent *e);
    void mouseReleaseEventFx(QMouseEvent *e);
    void leaveEventFx(QEvent *e);
    QCursor getCursorFx();

    const QPointF& getPoint() const
    {
        return ptPoint;
    }

    void setPoint(const QPointF& pt)
    {
        ptPoint = pt;
        state   = pt == NOPOINTF ? eStateNotSet : eStateSet;
    }

private:
    enum state_e
    {
        eStateNotSet
        , eStateSet
        , eStateHighlight
        , eStateMove
    };

    state_e state                   = eStateNotSet;
    CItemRefMap * item              = nullptr;
    const IDrawContext * context    = nullptr;
    QPointF ptPoint                 = NOPOINTF;
    QPointF ptFocus1                = NOPOINTF;
};

#endif //CGRIDPOINT_H

