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

#ifndef CMOUSEMOVEWPT_H
#define CMOUSEMOVEWPT_H

#include "gis/IGisItem.h"
#include "mouse/IMouse.h"

#include <QPixmap>

class CCanvas;
class CGisItemWpt;
class CGisDraw;

class CMouseMoveWpt : public IMouse
{
public:
    CMouseMoveWpt(CGisItemWpt& wpt, CGisDraw * gis, CCanvas * parent);
    virtual ~CMouseMoveWpt();

    void draw(QPainter& p,  CCanvas::redraw_e needsRedraw, const QRect &rect) override;
    void mousePressEvent  (QMouseEvent *e) override;
    void mouseMoveEvent   (QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void wheelEvent       (QWheelEvent *e) override;

protected slots:
    void slotPanCanvas() override;

private:
    IGisItem::key_t key;
    QPointF origPos;
    QPointF newPos;
    QPointF focus;
    QPixmap icon;
};

#endif //CMOUSEMOVEWPT_H

