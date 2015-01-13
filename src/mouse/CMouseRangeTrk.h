/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CMOUSERANGETRK_H
#define CMOUSERANGETRK_H

#include "gis/IGisItem.h"
#include "mouse/IMouse.h"
#include <QPointer>

class CGisItemTrk;
class CGisDraw;
class CCanvas;
class CScrOptRangeTrk;

class CMouseRangeTrk : public IMouse
{
    Q_OBJECT;
public:
    CMouseRangeTrk(CGisItemTrk& trk, CGisDraw * gis, CCanvas * parent);
    virtual ~CMouseRangeTrk();

    void draw(QPainter& p,  bool needsRedraw, const QRect &rect);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent * e);

private slots:
    void slotHidePoints();
    void slotShowPoints();
    void slotCopy();

private:
    IGisItem::key_t key;

    enum state_e
    {
        eStateIdle
        ,eStateMoveMap
        ,eStateSelectRange
        ,eStateRangeSelected
    };

    state_e state;

    QPointF anchor;

    QPoint lastPoint;

    QPointer<CScrOptRangeTrk> scrOptRange;
};

#endif //CMOUSERANGETRK_H

