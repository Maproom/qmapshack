/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2015 Christian Eichler <code@christian-eichler.de>

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

#include "canvas/CCanvas.h"
#include "helpers/CDraw.h"

#include <QDebug>
#include <QImage>
#include <QPointF>
#include <QtMath>
#include <QPainterPath>

QPen CDraw::penBorderBlue(QColor(10,10,150,220),2);
QPen CDraw::penBorderGray(Qt::lightGray,2);
QPen CDraw::penBorderBlack(QColor(0,0,0,200),2);
QBrush CDraw::brushBackWhite(QColor(255,255,255,255));
QBrush CDraw::brushBackYellow(QColor(0xff, 0xff, 0xcc, 0xE0));


QImage CDraw::createBasicArrow(const QBrush &brush, qreal scale)
{
    QImage arrow(21*scale, 16*scale, QImage::Format_ARGB32);
    arrow.fill(qRgba(0, 0, 0, 0));

    QPainter painter(&arrow);
    USE_ANTI_ALIASING(painter, true);

    // white background, same foreground as p
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(brush);

    QPointF arrowPoints[4] =
    {
        QPointF(20.0*scale,  7.0*scale), // front
        QPointF( 0.0*scale,  0.0*scale), // upper tail
        QPointF( 5.0*scale,  7.0*scale), // mid   tail
        QPointF( 0.0*scale, 15.0*scale)  // lower tail
    };
    painter.drawPolygon(arrowPoints, 4);
    painter.end();

    return arrow;
}

/**
   @brief   Calculates the square distance between two points
   @return  (int) ( (x2 - x1)^2 + (y2 - y1)^2 )
 */

static inline int pointDistanceSquare(const QPointF &p1, const QPointF &p2)
{
    return (p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y());
}

void CDraw::arrows(const QPolygonF &line, const QRectF &viewport, QPainter &p, int minPointDist, int minArrowDist, qreal scale)
{
    QImage arrow = createBasicArrow(p.brush(), scale);
    qreal xoff = qCeil(arrow.width()/2.0);
    qreal yoff = qFloor((arrow.height()-1)/2.0);

    const qreal minArrowDistSquare = minArrowDist * minArrowDist;
    const qreal minPointDistSquare = minPointDist * minPointDist;

    QPointF prevArrow;
    bool firstArrow = true;
    for(int i = 1; i < line.size(); i++)
    {
        const QPointF &pt     = line[i    ];
        const QPointF &prevPt = line[i - 1];

        // ensure there is enough space between two line points
        if( pointDistanceSquare(pt, prevPt) >= minPointDistSquare )
        {
            QPointF arrowPos = prevPt + (pt - prevPt)/2;

            if( (viewport.contains(pt) || 0 == viewport.height()) // ensure the point is visible
                && (firstArrow || pointDistanceSquare(prevArrow, arrowPos) >= minArrowDistSquare) )
            {
                p.save();

                // rotate and draw the arrow (between bullets)
                p.translate(arrowPos);
                qreal direction = ( qAtan2((pt.y() - prevPt.y()), (pt.x() - prevPt.x())) * 180.) / M_PI;
                p.rotate(direction);
                p.drawImage(-xoff, -yoff, arrow);

                p.restore();

                prevArrow  = arrowPos;
                firstArrow = false;
            }
        }
    }
}

void CDraw::text(const QString &str, QPainter &p, const QPoint &center, const QColor &color, const QFont &font)
{
    QFontMetrics fm(font);
    QRect r = fm.boundingRect(str);

    r.moveCenter(center);
    p.setFont(font);

    // draw the white `shadow`
    p.setPen(Qt::white);
    p.drawText(r.topLeft() - QPoint(-1, -1), str);
    p.drawText(r.topLeft() - QPoint( 0, -1), str);
    p.drawText(r.topLeft() - QPoint(+1, -1), str);

    p.drawText(r.topLeft() - QPoint(-1,  0), str);
    p.drawText(r.topLeft() - QPoint(+1,  0), str);

    p.drawText(r.topLeft() - QPoint(-1, +1), str);
    p.drawText(r.topLeft() - QPoint( 0, +1), str);
    p.drawText(r.topLeft() - QPoint(+1, +1), str);

    p.setPen(color);
    p.drawText(r.topLeft(), str);
}

void CDraw::text(const QString &str, QPainter &p, const QRect &r, const QColor &color)
{
    p.setPen(Qt::white);
    p.setFont(CMainWindow::self().getMapFont());

    // draw the white `shadow`
    p.drawText(r.adjusted(-1, -1, -1, -1), Qt::AlignCenter, str);
    p.drawText(r.adjusted( 0, -1,  0, -1), Qt::AlignCenter, str);
    p.drawText(r.adjusted(+1, -1, +1, -1), Qt::AlignCenter, str);

    p.drawText(r.adjusted(-1,  0, -1,  0), Qt::AlignCenter, str);
    p.drawText(r.adjusted(+1,  0, +1,  0), Qt::AlignCenter, str);

    p.drawText(r.adjusted(-1, +1, -1, +1), Qt::AlignCenter, str);
    p.drawText(r.adjusted( 0, +1,  0, +1), Qt::AlignCenter, str);
    p.drawText(r.adjusted(+1, +1, +1, +1), Qt::AlignCenter, str);

    p.setPen(color);
    p.drawText(r, Qt::AlignCenter, str);
}

QPoint CDraw::bubble(QPainter &p, const QRect &contentRect, const QPoint &pointerPos, int pointerBaseWidth, float pointerBasePos)
{
    QPainterPath bubblePath;
    bubblePath.addRoundedRect(contentRect, RECT_RADIUS, RECT_RADIUS);

    // draw the arrow
    int pointerBaseCenterX = (pointerBasePos <= 1)
                             ? contentRect.left() + (pointerBasePos * contentRect.width())
                             : contentRect.left() + (int) pointerBasePos;

    int pointerHeight = 0;
    if(pointerPos.y() < contentRect.top())
    {
        pointerHeight = contentRect.top() - pointerPos.y() + 1;
    }
    else if(pointerPos.y() > contentRect.bottom())
    {
        pointerHeight = contentRect.bottom() - pointerPos.y() - 1;
    }
    else
    {
        qDebug() << "cannot calculate pointerHeight/pointerBaseCenterX due to invalid parameters";
    }

    if(0 != pointerHeight)
    {
        QPolygonF pointerPoly;
        pointerPoly << pointerPos
                    << QPointF(pointerBaseCenterX - pointerBaseWidth / 2, pointerPos.y() + pointerHeight)
                    << QPointF(pointerBaseCenterX + pointerBaseWidth / 2, pointerPos.y() + pointerHeight)
                    << pointerPos;

        QPainterPath pointerPath;
        pointerPath.addPolygon(pointerPoly);

        bubblePath = bubblePath.united(pointerPath);
    }

    p.setPen  (CDraw::penBorderGray);
    p.setBrush(CDraw::brushBackWhite);

    p.drawPolygon(bubblePath.toFillPolygon());

    return contentRect.topLeft();
}

void CDraw::drawCrossHairDot(QPainter& p, const QPointF& pt)
{
    USE_ANTI_ALIASING(p, false);
    p.setBrush(Qt::NoBrush);
    QRectF dot2(0,0,7,7);
    p.setPen(QPen(Qt::white, 3));
    p.drawLine(pt.x(), pt.y() + 3, pt.x(), pt.y() + 20);
    p.drawLine(pt.x(), pt.y() - 3, pt.x(), pt.y() - 20);
    p.drawLine(pt.x() - 3, pt.y(), pt.x() - +20, pt.y());
    p.drawLine(pt.x() + 3, pt.y(), pt.x() + 20, pt.y());
    p.setPen(QPen(Qt::red, 1));
    p.drawLine(pt.x(), pt.y() + 3, pt.x(), pt.y() + 20);
    p.drawLine(pt.x(), pt.y() - 3, pt.x(), pt.y() - 20);
    p.drawLine(pt.x() - 3, pt.y(), pt.x() - +20, pt.y());
    p.drawLine(pt.x() + 3, pt.y(), pt.x() + 20, pt.y());

    dot2.moveCenter(pt);
    p.setPen(QPen(Qt::white, 3));
    p.drawRect(dot2);
    p.setPen(QPen(Qt::red, 1));
    p.drawRect(dot2);
    USE_ANTI_ALIASING(p, true);
}

void CDraw::drawRectangle(QPainter& p, const QRectF& rect, const QPen& pen, const QBrush& brush)
{
    p.setBrush(brush);
    p.setPen(QPen(Qt::white, pen.width() + 2));
    p.drawRect(rect);
    p.setPen(pen);
    p.drawRect(rect);
}

void CDraw::drawRectangle(QPainter& p, const QRectF& rect, const Qt::GlobalColor& pen, const Qt::GlobalColor& brush)
{
    drawRectangle(p,rect, QPen(pen), QBrush(brush));
}
