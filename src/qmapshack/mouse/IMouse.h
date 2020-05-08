/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "canvas/CCanvas.h"
#include <QObject>
#include <QPoint>

class CGisDraw;
class QPainter;
class CMouseAdapter;

class IMouse : public QObject
{
    Q_OBJECT
public:
    explicit IMouse(CGisDraw * gis, CCanvas * canvas, CMouseAdapter *mouse);
    virtual ~IMouse();

    virtual void leftClicked(const QPoint& pos) {}
    virtual void longPress(const QPoint& pos) {}
    virtual void mouseMoved(const QPoint& pos) {}
    virtual void mouseDragged(const QPoint& start, const QPoint& last, const QPoint& end);
    virtual void leftButtonDown(const QPoint& pos) {}
    virtual void rightButtonDown(const QPoint& pos);
    virtual void doubleClicked(const QPoint& pos) {}
    virtual void dragFinished(const QPoint& pos) {}
    virtual void scaleChanged() {}
    virtual void abortStep() {}
    virtual void unfocus(){}

    virtual void draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect) = 0;

protected:
    CCanvas * canvas;
    CGisDraw * gis;
    CMouseAdapter * mouse;

public:
    /// the current mouse cursor
    /**
        Each mouse function is represented by a special cursor. The main
        widget uses this method to query the current cursor.
     */
    operator const QCursor&() const
    {
        return cursor;
    }

    CCanvas * getCanvas() const
    {
        return canvas;
    }

    void startMouseMove(const QPoint& pos);

protected:
    /// the functions mouse icon
    QCursor cursor;
};

#endif // IMOUSE_H
