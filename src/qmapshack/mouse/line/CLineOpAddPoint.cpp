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

#include "mouse/line/CLineOpAddPoint.h"

#include <QtWidgets>

#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "mouse/line/IMouseEditLine.h"

CLineOpAddPoint::CLineOpAddPoint(SGisLine& points, CGisDraw* gis, CCanvas* canvas, IMouseEditLine* parent)
    : ILineOp(points, gis, canvas, parent) {
  cursor = QCursor(QPixmap(":/cursors/cursorPointAdd.png"), 0, 0);
}

CLineOpAddPoint::~CLineOpAddPoint() {}

void CLineOpAddPoint::append() {
  idxFocus = points.size();
  points.insert(idxFocus, IGisLine::point_t(points.last()));
  isDragging = true;
  focusIsEndpoint = true;
  startMouseMove(points.last().pixel);
}

bool CLineOpAddPoint::abortStep() {
  if (isDragging) {
    cancelDelayedRouting();
    parentHandler->restoreFromHistory(points);

    isDragging = false;
    idxFocus = NOIDX;

    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);

    return true;
  }
  return false;
}

void CLineOpAddPoint::leftClick(const QPoint& pos) {
  if (idxFocus == NOIDX || isRouting) {
    return;
  }

  if (isDragging) {
    QPointF coord = pos;
    gis->convertPx2Rad(coord);

    if (!runRoutingAndPin(coord)) {
      return;
    }

    if (focusIsEndpoint) {
      if (idxFocus == (points.size() - 1)) {
        idxFocus++;
      }
      parentHandler->storeToHistory(points);
      points.insert(idxFocus, IGisLine::point_t(coord));
    } else {
      parentHandler->storeToHistory(points);
      isDragging = false;
      idxFocus = NOIDX;
    }
  } else if (focusIsEndpoint) {
    /** idxFocus points at the last existing point; advance past it so the new point is appended. */
    if (idxFocus == (points.size() - 1)) {
      idxFocus++;
    }
    QPointF coord = pos;
    gis->convertPx2Rad(coord);
    points.insert(idxFocus, IGisLine::point_t(coord));
    isDragging = true;
  } else if (idxFocus != NOIDX) {
    points[idxFocus].subpts.clear();
    QPointF coord = pos;
    gis->convertPx2Rad(coord);
    /** idxFocus is the segment start; advance to insert the new point after it, not before. */
    idxFocus++;
    points.insert(idxFocus, IGisLine::point_t(coord));
    isDragging = true;
  }
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpAddPoint::mouseMove(const QPoint& pos) {
  ILineOp::mouseMove(pos);
  if (isDragging) {
    if (isRouting) {
      return;
    }
    QPointF coord = pos;
    gis->convertPx2Rad(coord);

    IGisLine::point_t& pt = points[idxFocus];
    pt.coord = coord;
    pt.subpts.clear();
    if (idxFocus > 0) {
      points[idxFocus - 1].subpts.clear();
    }

    startDelayedRouting();
  } else {
    focusIsEndpoint = false;
    idxFocus = isCloseToLine(pos);
    if (idxFocus == NOIDX) {
      idxFocus = isCloseTo(pos);
      if ((idxFocus == 0) || (idxFocus == (points.size() - 1))) {
        focusIsEndpoint = true;
      }
    }
  }
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpAddPoint::rightButtonDown(const QPoint& pos) {
  abortStep();
  idxFocus = NOIDX;
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpAddPoint::drawFg(QPainter& p) {
  if (idxFocus == NOIDX) {
    return;
  }

  if (isDragging) {
    const IGisLine::point_t& pt = points[idxFocus];
    drawSinglePointSmall(pt.pixel, p);
  } else if (focusIsEndpoint) {
    const IGisLine::point_t& pt = points[idxFocus];
    drawSinglePointLarge(pt.pixel, p);
  } else if (idxFocus < (points.size() - 1)) {
    QPolygonF line;
    const IGisLine::point_t& pt1 = points[idxFocus];
    const IGisLine::point_t& pt2 = points[idxFocus + 1];

    if (pt1.subpts.isEmpty()) {
      line << pt1.pixel << pt2.pixel;
    } else {
      line << pt1.pixel;
      for (const IGisLine::subpt_t& pt : pt1.subpts) {
        line << pt.pixel;
      }
      line << pt2.pixel;
    }

    p.setPen(penBgPoint);
    p.setBrush(brushBgPoint);

    rectPoint.moveCenter(pt1.pixel.toPoint());
    p.drawRect(rectPoint);
    rectPoint.moveCenter(pt2.pixel.toPoint());
    p.drawRect(rectPoint);

    p.setPen(QPen(Qt::white, 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(line);

    p.setPen(penFgPoint);
    p.setBrush(brushFgPoint);

    rectPoint.moveCenter(pt1.pixel.toPoint());
    p.drawRect(rectPoint);
    rectPoint.moveCenter(pt2.pixel.toPoint());
    p.drawRect(rectPoint);

    p.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(line);
  }
}
