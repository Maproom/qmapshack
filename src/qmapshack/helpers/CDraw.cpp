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
#include <QPainterPath>
#include <QPointF>
#include <QtMath>

QPen CDraw::penBorderBlue(QColor(10, 10, 150, 220), 2);
QPen CDraw::penBorderGray(Qt::lightGray, 2);
QPen CDraw::penBorderBlack(QColor(0, 0, 0, 200), 2);
QPen CDraw::penBorderRed(Qt::red, 2);
QBrush CDraw::brushBackWhite(QColor(255, 255, 255, 255));
QBrush CDraw::brushBackYellow(QColor(0xff, 0xff, 0xcc, 0xE0));
QBrush CDraw::brushBackSemiBlue(QColor(127, 127, 255, 127));


QImage CDraw::createBasicArrow(const QBrush &brush, qreal scale)
{
    QImage arrow(20 * scale, 15 * scale, QImage::Format_ARGB32);
    arrow.fill(qRgba(0, 0, 0, 0));

    QPainter painter(&arrow);
    USE_ANTI_ALIASING(painter, true);

    // white background, same foreground as p
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(brush);

//    *..............
//    .             .
//    .   *         *
//    .             .
//    *..............

    QPointF arrowPoints[5] =
    {
        QPointF(19.0 * scale,  7.0 * scale), // front
        QPointF( 0.0 * scale,  0.0 * scale), // upper tail
        QPointF( 5.0 * scale,  7.0 * scale), // mid   tail
        QPointF( 0.0 * scale, 14.0 * scale), // lower tail
        QPointF(19.0 * scale,  7.0 * scale)  // front
    };
    painter.drawPolygon(arrowPoints, 5);
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
    const QImage& arrow = createBasicArrow(p.brush(), qMin(qMax(1.0, scale / 3), 3.0));
    qreal xoff = qCeil(arrow.width() / 2.0);
    qreal yoff = qFloor((arrow.height() - 1) / 2.0);

    const qreal minArrowDistSquare = minArrowDist * minArrowDist;
    const qreal minPointDistSquare = minPointDist * minPointDist;

    QPointF prevArrow;
    bool firstArrow = true;
    for(int i = 1; i < line.size(); i++)
    {
        const QPointF &pt     = line[i    ];
        const QPointF &prevPt = line[i - 1];

        // ensure there is enough space between two linepts
        if( pointDistanceSquare(pt, prevPt) >= minPointDistSquare )
        {
            QPointF arrowPos = prevPt + (pt - prevPt) / 2;

            if( (viewport.contains(arrowPos) || 0 == viewport.height()) // ensure the point is visible
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

void CDraw::nogos(const QPolygonF &line, const QRectF &viewport, QPainter &p, int minNogoDist)
{
    static const QImage nogo = QImage("://icons/48x48/NoGo.png").scaled(14, 14, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    double l = minNogoDist / 3;

    for(int i = 1; i < line.size(); i++)
    {
        const QPointF &pt     = line[i    ];
        const QPointF &prevPt = line[i - 1];

        const double dist = sqrt(pointDistanceSquare(pt, prevPt));

        if ( l <= dist )
        {
            const QPointF line = pt - prevPt;
            do
            {
                const QPointF nogoPos = prevPt + line * l / dist;

                if( (viewport.contains(nogoPos) || 0 == viewport.height())) // ensure the point is visible
                {
                    p.save();

                    p.translate(nogoPos);
                    p.drawImage(-7, -7, nogo);

                    p.restore();
                }
                l += minNogoDist;
            }
            while ( l <= dist );
        }
        l -= dist;
    }
}

void CDraw::text(const QString &str, QPainter &p, const QPointF &center, const QColor &color, const QFont &font)
{
    text(str, p, center.toPoint(), color, font);
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

QPoint CDraw::bubble(QPainter &p, const QRect &contentRect, const QPoint &pointerPos, const QColor& background)
{
    qint32 pointerBasePos = qMax(0, pointerPos.x() - contentRect.left());
    return CDraw::bubble(p, contentRect, pointerPos, background, 20, pointerBasePos);
}

QPoint CDraw::bubble(QPainter &p, const QRect &contentRect, const QPoint &pointerPos, const QColor& background,
                     int pointerBaseWidth, float pointerBasePos, const QPen &pen)
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

    p.setPen  (pen);
    p.setBrush(background);
    p.drawPolygon(bubblePath.toFillPolygon());

    return contentRect.topLeft();
}

bool CDraw::doesOverlap(const QList<QRectF>& blockedAreas, const QRectF& rect)
{
    for(const QRectF& r : blockedAreas)
    {
        if(r.intersects(rect))
        {
            return true;
        }
    }
    return false;
}


void CDraw::number(int num, int size, QPainter& p, const QPointF& center, const QColor &color)
{
    const qreal size_2 = (size - 1) / 2.0;

    p.setPen(QPen(Qt::white, 2));
    p.setBrush(color);
    p.drawEllipse(center, size_2, size_2);

    const QString& s = QString::number(num);
    QRectF r = p.boundingRect(QRectF(), s);
    r.moveCenter(center);

    p.setPen(Qt::white);
    p.setBrush(Qt::white);
    p.drawText(r, s);
}

QPixmap CDraw::number(int num, const QColor &color)
{
    const QFont& f  = CMainWindow::self().getMapFont();
    const int pointSize = f.pointSize();
    const int size = (pointSize + (f.bold() ? 3 : 2)) * 2;

    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);

    p.setFont(f);
    USE_ANTI_ALIASING(p, true);
    p.translate(1, 1);

    CDraw::number(num, size, p, {(size - 2) / 2.0, (size - 2) / 2.0}, color);

    return pixmap;
}
