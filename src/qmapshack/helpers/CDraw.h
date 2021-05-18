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

#ifndef CPAINTER_H
#define CPAINTER_H

#include <QPainter>
#include <QPolygonF>
#include <QRectF>

#include "CMainWindow.h"
inline void USE_ANTI_ALIASING(QPainter& p, bool useAntiAliasing)
{
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing, useAntiAliasing);
}

#define RECT_RADIUS 3
#define PAINT_ROUNDED_RECT(p, r) p.drawRoundedRect(r, RECT_RADIUS, RECT_RADIUS)


class CDraw
{
public:

    static QPen penBorderBlue;
    static QPen penBorderGray;
    static QPen penBorderBlack;
    static QPen penBorderRed;
    static QBrush brushBackWhite;
    static QBrush brushBackYellow;
    static QBrush brushBackSemiBlue;

    /**
       @brief Draw arrows along a line

       An arrow is drawn if all the following requirements are met:
     * the position the new arrow would have been drawn is within viewport
          OR
          `viewport.height() == 0`
     * the two points have a distance of at least `minPointDist`
     * the (potential) position of the new arrow has at least a distance of `minArrowDist` from the previous arrow

       @param line          The line to draw the arrows along
       @param viewport      Restrict drawing of arrows to this viewport (no limitation is applied if `viewport.height() == 0`)
       @param minPointDist  The minimum distance of two points (in px)
       @param minArrowDist  The minimum distance of two consecutive arrows (in px)
     */
    static void arrows(const QPolygonF& line, const QRectF& viewport, QPainter& p, int minPointDist, int minArrowDist, qreal scale);
    static void nogos(const QPolygonF& line, const QRectF& viewport, QPainter& p, int minNogoDist);

    static void text(const QString& str, QPainter& p, const QPointF& center, const QColor& color, const QFont& font = CMainWindow::self().getMapFont());
    static void text(const QString& str, QPainter& p, const QPoint& center, const QColor& color, const QFont& font = CMainWindow::self().getMapFont());
    static void text(const QString& str, QPainter& p, const QRect& r, const QColor& color);

    /**
       @brief Draw a cartoon bubble

       `pointerBasePos` denotes the position of the pointer's base, where 0 is `at the very left of the content`, and 1 is `at the very right`.
       Be careful with small values (near 0) or large values (near 1) for pointerBasePos, this might lead to incorrect drawing,
       especially if pointerBaseWidth is large.
       If is larger than 1, a value in pixels is assumed.

       @param p                 An active QPainter
       @param contentRect       The area the actual content will be in
       @param pointerPos        The position of the pointer's head
       @param background        The color of the background
       @param pointerBaseWidth  The width of the pointer
       @param pointerBasePos    The (relative) location of the pointer (in percent / pixels)
       @param pen               The border color

       @return Top left corner of content rectangle.
     */
    static QPoint bubble(QPainter& p, const QRect& contentRect, const QPoint& pointerPos, const QColor& background,
                         int pointerBaseWidth, float pointerBasePos, const QPen& pen = penBorderGray);

    /**
       @brief Draw a cartoon bubble

       Shift the pointerBasePos below pointerPos.

       @param p                 An active QPainter
       @param contentRect       The area the actual content will be in
       @param pointerPos        The position of the pointer's head
       @param background        The color of the background

       @return Top left corner of content rectangle.
     */
    static QPoint bubble(QPainter& p, const QRect& contentRect, const QPoint& pointerPos, const QColor& background);


    static bool doesOverlap(const QList<QRectF>& blockedAreas, const QRectF& rect);

    /**
       @brief   Creates a new arrow using the brush specified
       @return  A QImage containing the arrow
     */
    static QImage createBasicArrow(const QBrush& brush, qreal scale);

    static void number(int num, int size, QPainter& p, const QPointF& center, const QColor& color);

    static QPixmap number(int num, const QColor& color);
};

#endif // CPAINTER_H

