/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2015 Christian Eichler code@christian-eichler.de

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

#include <QImage>
#include <QPointF>
#include <QtMath>

QPen CDraw::penBorderBlue(QColor(10,10,150,220),2);
QPen CDraw::penBorderGray(Qt::lightGray,2);
QPen CDraw::penBorderBlack(QColor(0,0,0,200),2);
QBrush CDraw::brushBackWhite(QColor(255,255,255,255));
QBrush CDraw::brushBackYellow(QColor(0xff, 0xff, 0xcc, 0xE0));


QImage CDraw::createBasicArrow(const QBrush &brush)
{
    QImage arrow(21, 16, QImage::Format_ARGB32);
    arrow.fill(qRgba(0, 0, 0, 0));

    QPainter painter(&arrow);
    USE_ANTI_ALIASING(painter, true);

    // white background, same foreground as p
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(brush);

    QPointF arrowPoints[4] =
    {
        QPointF(20.0,  7.0), // front
        QPointF( 0.0,  0.0), // upper tail
        QPointF( 5.0,  7.0), // mid   tail
        QPointF( 0.0, 15.0)  // lower tail
    };
    painter.drawPolygon(arrowPoints, 4);
    painter.end();

    return arrow;
}

/**
   @brief   Calculates the distance between two points
   @return  (int) ( (x2 - x1)^2 + (y2 - y1)^2 )^0.5
 */
template<typename T>
static inline int pointDistance(T &p1, T &p2)
{
    return qSqrt(qPow(p2.x() - p1.x(), 2) + qPow(p2.y() - p1.y(), 2));
}

void CDraw::arrows(const QPolygonF &line, const QRectF &viewport, QPainter &p, int minPointDist, int minArrowDist)
{
    QImage arrow = createBasicArrow(p.brush());

    QPoint prevArrow;
    bool firstArrow = true;
    for(int i = 1; i < line.size(); i++)
    {
        const QPointF &pt     = line[i    ];
        const QPointF &prevPt = line[i - 1];

        // ensure there is enough space between two linepts
        if( pointDistance(pt, prevPt) >= minPointDist )
        {
            QPoint arrowPos((pt.x() + prevPt.x()) / 2, (pt.y() + prevPt.y()) / 2);

            if( (viewport.contains(pt) || 0 == viewport.height()) // ensure the point is visible
                && (firstArrow || pointDistance(prevArrow, arrowPos) >= minArrowDist) )
            {
                p.save();

                // rotate and draw the arrow (between bullets)
                p.translate(arrowPos);
                qreal direction = ( qAtan2((qreal)(pt.y() - prevPt.y()), (qreal)(pt.x() - prevPt.x())) * 180.) / M_PI;
                p.rotate(direction);
                p.drawImage(-11, -7, arrow);

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


void CDraw::bubble1(const QRect& rect, const QPointF &anchor, QPainter& p)
{
    // create bubble path
    QPainterPath path1;
    path1.addRoundedRect(rect,5,5);

    QPolygonF poly2;
    poly2 << anchor << (rect.bottomLeft() + QPointF(10,-5)) << (rect.bottomLeft() + QPointF(30,-5)) << anchor;
    QPainterPath path2;
    path2.addPolygon(poly2);

    path1 = path1.united(path2);

    // draw bubble
    p.setPen(CDraw::penBorderGray);
    p.setBrush(CDraw::brushBackWhite);
    p.drawPolygon(path1.toFillPolygon());
}

void CDraw::bubble2(const QWidget& widget, const QPointF &anchor, QPainter& p)
{
    QRectF rect = widget.rect();
    rect.moveTopLeft(QPoint(widget.x(), widget.y()));
    QPainterPath path1;
    path1.addRoundedRect(rect,5,5);

    QPolygonF poly2;
    poly2 << anchor << QPointF(anchor.x() - 10, rect.top() + 1) << QPointF(anchor.x() + 10, rect.top() + 1) << anchor;
    QPainterPath path2;
    path2.addPolygon(poly2);

    path1 = path1.united(path2);

    p.setPen(penBorderGray);
    p.setBrush(brushBackWhite);
    p.drawPolygon(path1.toFillPolygon());
}
