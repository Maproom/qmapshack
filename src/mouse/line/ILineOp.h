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

#ifndef ILINEOP_H
#define ILINEOP_H

#include "gis/IGisLine.h"
#include <QCursor>
#include <QObject>

class QMouseEvent;
class CCanvas;
class QPainter;
class IMouseEditLine;

class ILineOp : public QObject
{
public:
    ILineOp(SGisLine &points, CGisDraw * gis, CCanvas * canvas, IMouseEditLine * parent);
    virtual ~ILineOp();

    virtual void mousePressEvent(QMouseEvent * e);
    virtual void mouseMoveEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

    virtual void draw(QPainter& p) = 0;

    const QCursor& getCursor()
    {
        return cursor;
    }

protected:
    virtual void finalizeOperation(qint32 idx);
    qint32 isCloseTo(const QPoint& pos);
    qint32 isCloseToLine(const QPoint& pos);


    IMouseEditLine * parentHandler;
    SGisLine& points;
    CCanvas * canvas;
    CGisDraw * gis;

    QCursor cursor;

    bool mapMove;
    bool mapDidMove;

    QPoint lastPos;
};

#endif //ILINEOP_H

