/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CMOUSERADIUSWPT_H
#define CMOUSERADIUSWPT_H

#include "gis/IGisItem.h"
#include "mouse/IMouse.h"

class CGisItemWpt;
class CGisDraw;
class CCanvas;

class CMouseRadiusWpt : public IMouse
{
    Q_OBJECT
public:
    CMouseRadiusWpt(CGisItemWpt& wpt, CGisDraw * gis, CCanvas * canvas, CMouseAdapter * mouse);
    virtual ~CMouseRadiusWpt();

    void draw(QPainter& p,  CCanvas::redraw_e needsRedraw, const QRect &rect) override;

    void leftClicked(const QPoint& point) override;
    void mouseMoved(const QPoint &pos) override;
    void mouseDragged(const QPoint& start, const QPoint& last, const QPoint& end) override;
    void rightButtonDown(const QPoint& point) override;
    void scaleChanged() override;

private:
    const IGisItem::key_t key;
    const QPointF wptPosition;
    const bool avoid;
    bool initial;
    QPointF startPos;
    qreal dist;
};

#endif //CMOUSERADIUSWPT_H

