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

#ifndef IMOUSE_H
#define IMOUSE_H

#include <QCursor>
#include <QObject>
#include <QPointer>

class QMouseEvent;
class QWheelEvent;
class CCanvas;
class QTimer;
class CGisDraw;


class IMouse : public QObject
{
    Q_OBJECT
public:
    IMouse(CGisDraw * gis, CCanvas * canvas);
    virtual ~IMouse();

    enum type_e
    {
        eNormal
    };

    virtual void draw(QPainter& p, bool needsRedraw, const QRect &rect) = 0;
    virtual void mousePressEvent(QMouseEvent * e) = 0;
    virtual void mouseMoveEvent(QMouseEvent * e) = 0;
    virtual void mouseReleaseEvent(QMouseEvent *e) = 0;
    virtual void mouseDoubleClickEvent(QMouseEvent *e)
    {
    }
    virtual void wheelEvent(QWheelEvent * e) = 0;
    /// the current mouse cursor
    /**
        Each mouse function is represented by a special cursor. The main
        widget uses this method to query the current cursor.
     */
    operator const QCursor&()
    {
        return( cursor);
    }
    CCanvas * getCanvas()
    {
        return( canvas);
    }

    virtual void setMouseTracking(bool enabled);

protected slots:
    virtual void slotPanCanvas();

protected:
    void panCanvas(const QPoint& pos);
    /// the functions mouse icon
    QCursor cursor;

    QPointer<CGisDraw> gis;

    QPointer<CCanvas>  canvas;
    // the current point reported by the mouse events
    QPoint point;

    QTimer * timer;
};

#endif //IMOUSE_H

