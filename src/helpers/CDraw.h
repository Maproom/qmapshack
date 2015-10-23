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

#ifndef CPAINTER_H
#define CPAINTER_H

#include <QPolygonF>
#include <QRectF>
#include <QPainter>

#include "CMainWindow.h"

class CDraw
{
public:
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
    static void arrows(const QPolygonF &line, const QRectF &viewport, QPainter &p, int minPointDist, int minArrowDist);

    static void text(const QString& str, QPainter &p, const QPoint &center, const QColor &color, const QFont &font = CMainWindow::self().getMapFont());
    static void text(const QString& str, QPainter &p, const QRect  &r,      const QColor &color);

private:
    /**
       @brief   Creates a new arrow using the brush specified
       @return  A QImage containing the arrow
     */
    static QImage createBasicArrow(const QBrush &brush);
};

#endif // CPAINTER_H

