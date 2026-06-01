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

#ifndef CLINEOPADDPOINT_H
#define CLINEOPADDPOINT_H

#include "mouse/line/ILineOp.h"

/**
   @brief Line operation that adds new points to the line.

   Three hover modes depending on what the cursor is nearest to:
   - Hovering a mid-segment: left-click inserts a new point into that segment.
   - Hovering the first or last point (focusIsEndpoint): left-click extends the line.
   - Dragging (isDragging): a new point is attached to the cursor until the next left-click drops it.
 */
class CLineOpAddPoint : public ILineOp {
 public:
  CLineOpAddPoint(SGisLine& points, CGisDraw* gis, CCanvas* canvas, IMouseEditLine* parent);
  virtual ~CLineOpAddPoint();

  void leftClick(const QPoint& pos) override;
  void mouseMove(const QPoint& pos) override;
  void rightButtonDown(const QPoint& pos) override;

  void drawFg(QPainter& p) override;

  /**
     @brief Enter dragging mode immediately, appending a new point at the end of the line.

     Called when creating a brand-new line so that the first click in the editor already
     has a point following the cursor.
   */
  void append();

  bool abortStep() override;

 private:
  bool isDragging = false;      /**< true while a new point is attached to the cursor */
  bool focusIsEndpoint = false; /**< true when the cursor is nearest to the first or last point */
};

#endif  // CLINEOPADDPOINT_H
