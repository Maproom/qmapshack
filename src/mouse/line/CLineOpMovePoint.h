/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CLINEOPMOVEPOINT_H
#define CLINEOPMOVEPOINT_H

#include "mouse/line/ILineOp.h"

class CLineOpMovePoint : public ILineOp
{
public:
    CLineOpMovePoint(SGisLine& points, CGisDraw *gis, CCanvas *canvas, IMouseEditLine *parent);
    virtual ~CLineOpMovePoint();

    void mousePressEventEx(QMouseEvent * e);
    void mouseMoveEventEx(QMouseEvent * e);
    void mouseReleaseEventEx(QMouseEvent * e)
    {
    }

    void drawFg(QPainter& p);

    void canvasPanned(QPointF pos);

    bool abortStep();

private:
    bool movePoint = false;
};

#endif //CLINEOPMOVEPOINT_H

