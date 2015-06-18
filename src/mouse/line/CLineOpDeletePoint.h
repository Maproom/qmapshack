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

#ifndef CLINEOPDELETEPOINT_H
#define CLINEOPDELETEPOINT_H

#include "mouse/line/ILineOp.h"

class CLineOpDeletePoint : public ILineOp
{
public:
    CLineOpDeletePoint(SGisLine& points, CGisDraw *gis, CCanvas *canvas, IMouseEditLine *parent);
    virtual ~CLineOpDeletePoint();

    void mousePressEventEx(QMouseEvent * e){}
    void mouseMoveEventEx(QMouseEvent * e);
    void mouseReleaseEventEx(QMouseEvent *e);

    void draw(QPainter& p);

private:

};

#endif //CLINEOPDELETEPOINT_H

