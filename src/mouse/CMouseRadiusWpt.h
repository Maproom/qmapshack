/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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
    CMouseRadiusWpt(CGisItemWpt& wpt, CGisDraw * gis, CCanvas * parent);
    virtual ~CMouseRadiusWpt();

    void draw(QPainter& p,  CCanvas::redraw_e needsRedraw, const QRect &rect) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void afterMouseLostEvent(QMouseEvent *e) override;

private:
    const IGisItem::key_t key;
    const QPointF wptPosition;
    const bool avoid;
    qreal   dist;

    bool mapMove    = false;
    bool mapDidMove   = false;

    QPoint lastPoint;
};

#endif //CMOUSERADIUSWPT_H

