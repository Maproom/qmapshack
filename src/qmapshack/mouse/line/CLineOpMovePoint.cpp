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

#include "mouse/line/CLineOpMovePoint.h"

#include <QtWidgets>

#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "mouse/line/IMouseEditLine.h"
#include "units/IUnit.h"

CLineOpMovePoint::CLineOpMovePoint(SGisLine& points, CGisDraw* gis, CCanvas* canvas, IMouseEditLine* parent)
    : ILineOp(points, gis, canvas, parent) {
  cursor = QCursor(QPixmap(":/cursors/cursorPointMove.png"), 0, 0);
}

CLineOpMovePoint::~CLineOpMovePoint() {}

void CLineOpMovePoint::leftClick(const QPoint& pos) {
  if (isDragging) {
    if (isRouting) {
      return;
    }

    QPointF coord = pos;
    gis->convertPx2Rad(coord);

    if (!runRoutingAndPin(coord)) {
      return;
    }

    isDragging = false;
    parentHandler->storeToHistory(points);
  } else if (idxFocus != NOIDX) {
    QPointF coord = pos;
    gis->convertPx2Rad(coord);

    IGisLine::point_t& pt = points[idxFocus];
    pt.coord = coord;
    pt.subpts.clear();
    if (idxFocus != 0) {
      points[idxFocus - 1].subpts.clear();
    }

    isDragging = true;
  }

  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpMovePoint::rightButtonDown(const QPoint& pos) {
  abortStep();
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

bool CLineOpMovePoint::abortStep() {
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

void CLineOpMovePoint::mouseMove(const QPoint& pos) {
  ILineOp::mouseMove(pos);

  if (isDragging) {
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
    idxFocus = isCloseTo(pos);
  }
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpMovePoint::drawFg(QPainter& p) {
  if (idxFocus == NOIDX) {
    return;
  }

  const IGisLine::point_t& pt = points[idxFocus];
  if (isDragging) {
    drawSinglePointSmall(pt.pixel, p);
  } else {
    drawSinglePointLarge(pt.pixel, p);
  }
}
