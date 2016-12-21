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
    Q_OBJECT
public:
    CMouseRangeTrk(CGisItemTrk& trk, CGisDraw * gis, CCanvas * parent);
    virtual ~CMouseRangeTrk();

    void draw(QPainter& p, CCanvas::redraw_e, const QRect &) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private slots:
    void slotHidePoints();
    void slotShowPoints();
    void slotActivity(quint32 flags);
    void slotCopy();

private:
    void resetState();
    IGisItem::key_t key;

    enum state_e
    {
        eStateIdle
        ,eStateSelectRange
        ,eStateRangeSelected
    };

    bool moveMap    = false;
    bool mapMoved   = false;
    state_e state   = eStateIdle;
    QPointF anchor  = NOPOINTF;

    QPoint lastPoint;

    QPointer<CScrOptRangeTrk> scrOptRange;
};

#endif //CMOUSERANGETRK_H

