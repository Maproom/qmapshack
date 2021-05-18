/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>
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

#ifndef ILINEOP_H
#define ILINEOP_H

#include "gis/IGisLine.h"
#include <QBrush>
#include <QCursor>
#include <QObject>
#include <QPen>
#include <QRect>

class QMouseEvent;
class QPinchGesture;
class CCanvas;
class QPainter;
class IMouseEditLine;

class ILineOp : public QObject
{
    Q_OBJECT
public:
    ILineOp(SGisLine& points, CGisDraw* gis, CCanvas* canvas, IMouseEditLine* parent);
    virtual ~ILineOp();

    virtual void leftClick(const QPoint& pos) = 0;
    virtual void mouseMove(const QPoint& pos);
    virtual void leftButtonDown(const QPoint& pos);
    virtual void rightButtonDown(const QPoint& pos) {}
    virtual void scaleChanged();

    virtual void drawFg(QPainter& p) = 0;
    virtual void drawBg(QPainter& p);

    const QCursor& getCursor() const
    {
        return cursor;
    }

    /**
       @brief   (try to) abort a step in the current operation

       Method called, if a step in the current operation should be aborted, such as
       adding or moving a(n already) selected waypoint.
       This dummy implementation does not do anything and therefore always returns `false`.

       @return  `true` if a step in the current operation was  successfully, `false` otherwise
     */
    virtual bool abortStep()
    {
        return false;
    }

    void updateStatus();
    void showRoutingErrorMessage(const QString& msg) const;

protected slots:
    void slotTimeoutRouting();

protected:
    virtual void cancelDelayedRouting();
    virtual void startDelayedRouting();
    virtual void finalizeOperation(qint32 idx);
    qint32 isCloseTo(const QPoint& pos) const;
    qint32 isCloseToLine(const QPoint& pos) const;

    void drawSinglePointSmall(const QPointF& pt, QPainter& p);
    void drawSinglePointLarge(const QPointF& pt, QPainter& p);
    void drawLeadLine(const QPolygonF& line, QPainter& p) const;
    void updateLeadLines(qint32 idx);

    void startMouseMove(const QPointF& point);

    IMouseEditLine* parentHandler;
    SGisLine& points;
    CCanvas* canvas;
    CGisDraw* gis;

    QCursor cursor;

    qint32 idxFocus = NOIDX;

    QPoint lastPos;
    QPoint firstPos;

    QRect rectPoint {0, 0, 9, 9};
    const QPen penBgPoint {Qt::white, 4};
    const QPen penFgPoint {Qt::red, 2};
    const QBrush brushBgPoint {Qt::white};
    const QBrush brushFgPoint {Qt::red};

    QPolygonF leadLineCoord1;
    QPolygonF leadLineCoord2;
    QPolygonF leadLinePixel1;
    QPolygonF leadLinePixel2;

    QPolygonF subLineCoord1;
    QPolygonF subLineCoord2;
    QPolygonF subLinePixel1;
    QPolygonF subLinePixel2;

private:
    void tryRouting(IGisLine::point_t& pt1, IGisLine::point_t& pt2) const;

    QTimer* timerRouting;
    QTime buttonPressTime;

    bool ignoreClick = false;
    bool mapMove = false;
    bool mouseDidMove = false;
};

#endif //ILINEOP_H

