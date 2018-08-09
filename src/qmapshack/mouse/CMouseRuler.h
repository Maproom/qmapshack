/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CMOUSERULER_H
#define CMOUSERULER_H

#include "mouse/IMouse.h"

class CMouseRuler : public IMouse
{
    Q_OBJECT
public:
    CMouseRuler(CGisDraw * gis, CCanvas *canvas, CMouseAdapter *mouse);
    virtual ~CMouseRuler();


    void rightButtonDown(const QPoint& pos) override;
    void leftButtonDown(const QPoint& pos) override;
    void mouseMoved(const QPoint& pos) override;
    void draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect) override;

private:
    enum mode_e
    {
        eModeIdle
        ,eModePaused
        ,eModeEdit
    };

    mode_e mode = eModeEdit;
    QPolygonF line;
};

#endif //CMOUSERULER_H

