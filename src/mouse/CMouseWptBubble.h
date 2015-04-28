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

#ifndef CMOUSEWPTBUBBLE_H
#define CMOUSEWPTBUBBLE_H

#include "gis/IGisItem.h"
#include "mouse/IMouse.h"

class CGisItemWpt;
class CGisDraw;
class CCanvas;


class CMouseWptBubble : public IMouse
{
    Q_OBJECT
public:
    CMouseWptBubble(const IGisItem::key_t& key, CGisDraw * gis, CCanvas * parent);
    virtual ~CMouseWptBubble();

    virtual void draw(QPainter& p, bool needsRedraw, const QRect &rect);
    virtual void mousePressEvent(QMouseEvent * e);
    virtual void mouseMoveEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void wheelEvent(QWheelEvent * e);

private:
    const IGisItem::key_t& key;
};

#endif //CMOUSEWPTBUBBLE_H

