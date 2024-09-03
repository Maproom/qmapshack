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

#include "mouse/line/CLineOpSelectRange.h"

#include <QtWidgets>

#include "canvas/CCanvas.h"
#include "mouse/line/CScrOptRangeLine.h"
#include "mouse/line/IMouseEditLine.h"

CLineOpSelectRange::CLineOpSelectRange(SGisLine& points, CGisDraw* gis, CCanvas* canvas, IMouseEditLine* parent)
    : ILineOp(points, gis, canvas, parent) {
  cursor = QCursor(QPixmap(":/cursors/cursorSelectRange.png"), 0, 0);
}

CLineOpSelectRange::~CLineOpSelectRange() { delete scrOptRangeLine; }

void CLineOpSelectRange::leftClick(const QPoint& pos) {
  switch (state) {
    case eStateIdle: {
      if (idxFocus != NOIDX) {
        state = eState1st;
      }
      break;
    }

    case eState1st: {
      if (idx2nd < 0 || points.size() <= idx2nd) {
        break;
      }

      qint32 d = qAbs(idxFocus - idx2nd);
      if (d < 1) {
        resetState();
        return;
      }

      scrOptRangeLine = new CScrOptRangeLine(points[idx2nd].pixel, parentHandler, canvas);
      connect(scrOptRangeLine->toolDelete, &QToolButton::clicked, this, &CLineOpSelectRange::slotDelete);
      connect(scrOptRangeLine->toolCalcRoute, &QToolButton::clicked, this, &CLineOpSelectRange::slotCalc);
      connect(scrOptRangeLine->toolDelete, &QToolButton::clicked, scrOptRangeLine.data(), &CScrOptRangeLine::hide);
      connect(scrOptRangeLine->toolCalcRoute, &QToolButton::clicked, scrOptRangeLine.data(), &CScrOptRangeLine::hide);

      if (d < 2) {
        scrOptRangeLine->toolDelete->setEnabled(false);
      }

      state = eState2nd;
      break;
    }
  }

  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpSelectRange::rightButtonDown(const QPoint& pos) {
  resetState();
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

bool CLineOpSelectRange::abortStep() {
  if (state != eStateIdle) {
    resetState();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
    return true;
  }
  return false;
}

void CLineOpSelectRange::mouseMove(const QPoint& pos) {
  ILineOp::mouseMove(pos);

  switch (state) {
    case eStateIdle: {
      // no point selected yet, find point to highlight
      idxFocus = isCloseTo(pos);
      break;
    }

    case eState1st: {
      idx2nd = isCloseTo(pos);
      if (idx2nd == NOIDX) {
        idx2nd = isCloseToLine(pos);
        if ((idx2nd != NOIDX) && ((idx2nd + 1) < points.size())) {
          idx2nd++;
        }
      }
      break;
    }
  }

  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpSelectRange::scaleChanged() {
  ILineOp::scaleChanged();
  if (state == eState2nd) {
    resetState();
  }
}

void CLineOpSelectRange::drawFg(QPainter& p) {
  if (idxFocus == NOIDX) {
    return;
  }

  switch (state) {
    case eStateIdle: {
      const IGisLine::point_t& pt = points[idxFocus];
      drawSinglePointLarge(pt.pixel, p);
      break;
    }

    case eState1st:
    case eState2nd: {
      if (idx2nd != NOIDX) {
        qint32 idx1 = qMin(idxFocus, idx2nd);
        qint32 idx2 = qMax(idxFocus, idx2nd);

        QPolygonF seg;
        for (int i = idx1; i < idx2; i++) {
          const IGisLine::point_t& point = points[i];
          seg << point.pixel;
          for (const IGisLine::subpt_t& subpt : point.subpts) {
            seg << subpt.pixel;
          }
        }

        seg << points[idx2].pixel;

        p.setPen(QPen(Qt::darkGreen, 11, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(seg);

        p.setPen(QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(seg);

        p.setPen(QPen(Qt::NoPen));
        p.setBrush(Qt::black);

        QRectF r(0, 0, 8, 8);
        for (int i = idx1; i <= idx2; i++) {
          r.moveCenter(points[i].pixel);
          p.drawRect(r);

          for (const IGisLine::subpt_t& subpt : std::as_const(points[i].subpts)) {
            p.drawEllipse(subpt.pixel, 2, 2);
          }
        }
      }

      if (!scrOptRangeLine.isNull() && eState2nd == state) {
        scrOptRangeLine->draw(p);
      }
      break;
    }
  }
}

void CLineOpSelectRange::resetState() {
  if (!scrOptRangeLine.isNull()) {
    scrOptRangeLine->deleteLater();
  }
  idxFocus = NOIDX;
  idx2nd = NOIDX;
  state = eStateIdle;
}

void CLineOpSelectRange::slotDelete() {
  qint32 idx = qMin(idxFocus, idx2nd);
  qint32 N = qAbs(idxFocus - idx2nd) - 1;

  points[idx].subpts.clear();
  points.remove(idx + 1, N);
  parentHandler->storeToHistory(points);

  resetState();
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpSelectRange::slotCalc() {
  qint32 idx = qMin(idxFocus, idx2nd);
  qint32 N = qAbs(idxFocus - idx2nd) - 1;

  points.remove(idx + 1, N);

  finalizeOperation(idx);
  parentHandler->storeToHistory(points);

  resetState();
}
