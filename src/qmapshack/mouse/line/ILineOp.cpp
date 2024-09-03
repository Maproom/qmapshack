/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "mouse/line/ILineOp.h"

#include <QtWidgets>

#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWorkspace.h"
#include "gis/GeoMath.h"
#include "gis/rte/router/CRouterSetup.h"
#include "mouse/line/IMouseEditLine.h"

struct segment_t {
  segment_t() : idx11(NOIDX), idx12(NOIDX), idx21(NOIDX) {}

  void apply(const QPolygonF& coords, const QPolygonF& pixel, QPolygonF& segCoord, QPolygonF& segPixel,
             IDrawContext* context) {
    QPointF pt1 = px1;
    QPointF pt2 = px2;

    context->convertPx2Rad(pt1);
    context->convertPx2Rad(pt2);

    if (idx11 != NOIDX && idx21 != NOIDX) {
      if (idx12 == idx21) {
        segPixel.push_back(pixel[idx12]);
        segCoord.push_back(coords[idx12]);
      } else if (idx11 < idx21) {
        for (int i = idx12; i <= idx21; i++) {
          segPixel.push_back(pixel[i]);
          segCoord.push_back(coords[i]);
        }
      } else if (idx11 > idx21) {
        for (int i = idx11; i > idx21; i--) {
          segPixel.push_back(pixel[i]);
          segCoord.push_back(coords[i]);
        }
      }
    }
  }

  qint32 idx11;
  qint32 idx12;
  qint32 idx21;

  QPointF px1;
  QPointF px2;
};

static inline qreal distance(const QPointF& pa, const QPointF& pb) {
  const qreal& dx = pa.x() - pb.x();
  const qreal& dy = pa.y() - pb.y();
  return qSqrt(dx * dx + dy * dy);
}

void GPS_Math_SubPolyline(const QPointF& pt1, const QPointF& pt2, qint32 threshold, const QPolygonF& pixel,
                          segment_t& result) {
  PJ_UV p1, p2;
  qreal dx, dy;   // delta x and y defined by p1 and p2
  qreal d_p1_p2;  // distance between p1 and p2
  qreal x, y;     // coord. (x,y) of the point on line defined by [p1,p2] close to pt
  qreal shortest1 = threshold;
  qreal shortest2 = threshold;
  qint32 idx11 = NOIDX, idx21 = NOIDX, idx12 = NOIDX;

  QPointF pt11;
  QPointF pt21;

  // find points on line closest to pt1 and pt2
  const qint32 len = pixel.size();
  for (qint32 i = 1; i < len; ++i) {
    p1.u = pixel[i - 1].x();
    p1.v = pixel[i - 1].y();
    p2.u = pixel[i].x();
    p2.v = pixel[i].y();

    dx = p2.u - p1.u;
    dy = p2.v - p1.v;
    d_p1_p2 = qSqrt(dx * dx + dy * dy);

    // find point on line closest to pt1
    // ratio u the tangent point will divide d_p1_p2
    qreal u = ((pt1.x() - p1.u) * dx + (pt1.y() - p1.v) * dy) / (d_p1_p2 * d_p1_p2);

    if (u >= 0.0 && u <= 1.0) {
      x = p1.u + u * dx;
      y = p1.v + u * dy;

      qreal distance = qSqrt((x - pt1.x()) * (x - pt1.x()) + (y - pt1.y()) * (y - pt1.y()));

      if (distance < shortest1) {
        idx11 = i - 1;
        idx12 = i;
        pt11.setX(x);
        pt11.setY(y);
        shortest1 = distance;
      }
    }

    // find point on line closest to pt2
    // ratio u the tangent point will divide d_p1_p2
    u = ((pt2.x() - p1.u) * dx + (pt2.y() - p1.v) * dy) / (d_p1_p2 * d_p1_p2);

    if (u >= 0.0 && u <= 1.0) {
      x = p1.u + u * dx;
      y = p1.v + u * dy;

      qreal distance = qSqrt((x - pt2.x()) * (x - pt2.x()) + (y - pt2.y()) * (y - pt2.y()));

      if (distance < shortest2) {
        idx21 = i - 1;
        pt21.setX(x);
        pt21.setY(y);
        shortest2 = distance;
      }
    }
  }

  // if 1st point can't be found test for distance to both ends
  if (idx11 == NOIDX) {
    QPointF px = pixel.first();
    qreal dist = distance(px, pt1);
    if (dist < (threshold << 1)) {
      idx11 = 0;
      idx12 = 1;
      pt11 = px;
    } else {
      px = pixel.last();
      qreal dist = distance(px, pt1);
      if (dist < (threshold << 1)) {
        idx11 = pixel.size() - 2;
        idx12 = pixel.size() - 1;
        pt11 = px;
      }
    }
  }

  // if 2nd point can't be found test for distance to both ends
  if (idx21 == NOIDX) {
    QPointF px = pixel.first();
    qreal dist = distance(px, pt2);

    if (dist < (threshold << 1)) {
      idx21 = 0;
      pt21 = px;
    } else {
      px = pixel.last();
      qreal dist = distance(px, pt2);
      if (dist < (threshold << 1)) {
        idx21 = pixel.size() - 2;
        pt21 = px;
      }
    }
  }

  //    qDebug() << pixel.size() << idx11 << idx12 << idx21 << pt1 << pt2 << pt11 << pt21;

  result.idx11 = idx11;
  result.idx12 = idx12;
  result.idx21 = idx21;
  result.px1 = pt11;
  result.px2 = pt21;
}

ILineOp::ILineOp(SGisLine& points, CGisDraw* gis, CCanvas* canvas, IMouseEditLine* parent)
    : QObject(parent), parentHandler(parent), points(points), canvas(canvas), gis(gis) {
  timerRouting = new QTimer(this);
  timerRouting->setSingleShot(true);
  timerRouting->setInterval(400);
  connect(timerRouting, &QTimer::timeout, this, &ILineOp::slotTimeoutRouting);
}

ILineOp::~ILineOp() { canvas->reportStatus("Routino", QString()); }

void ILineOp::cancelDelayedRouting() { timerRouting->stop(); }

void ILineOp::startDelayedRouting() {
  if (parentHandler->useAutoRouting()) {
    timerRouting->start();
  } else if (parentHandler->useVectorRouting() || parentHandler->useTrackRouting()) {
    slotTimeoutRouting();
  }
}

void ILineOp::slotTimeoutRouting() {
  timerRouting->stop();
  finalizeOperation(idxFocus);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void ILineOp::drawBg(QPainter& p) {
  drawLeadLine(leadLinePixel1, p);
  drawLeadLine(leadLinePixel2, p);
}

void ILineOp::drawSinglePointSmall(const QPointF& pt, QPainter& p) {
  QRect r(0, 0, 3, 3);
  r.moveCenter(pt.toPoint());

  p.setPen(QPen(Qt::white, 2));
  p.setBrush(Qt::white);
  p.drawRect(r);

  p.setPen(Qt::black);
  p.setBrush(Qt::black);
  p.drawRect(r);
}

void ILineOp::drawSinglePointLarge(const QPointF& pt, QPainter& p) {
  rectPoint.moveCenter(pt.toPoint());

  p.setPen(penBgPoint);
  p.setBrush(brushBgPoint);
  p.drawRect(rectPoint);

  p.setPen(penFgPoint);
  p.setBrush(brushFgPoint);
  p.drawRect(rectPoint);
}

void ILineOp::drawLeadLine(const QPolygonF& line, QPainter& p) const {
  p.setPen(QPen(Qt::yellow, 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  p.drawPolyline(line);
}

void ILineOp::mouseMove(const QPoint& pos) { updateLeadLines(idxFocus); }

void ILineOp::leftButtonDown(const QPoint& pos) {
  timerRouting->stop();
  showRoutingErrorMessage(QString());
}

void ILineOp::scaleChanged() { timerRouting->stop(); }

void ILineOp::startMouseMove(const QPointF& point) {
  //    // as long the mouse is not taken as moving
  //    // to not trigger on-the-fly-routing
  parentHandler->startMouseMove(point.toPoint());
  timerRouting->stop();
}

void ILineOp::updateLeadLines(qint32 idx) {
  leadLinePixel1.clear();
  leadLinePixel2.clear();
  subLinePixel1.clear();
  subLinePixel2.clear();

  if ((parentHandler->useVectorRouting() || parentHandler->useTrackRouting()) && (idx != NOIDX)) {
    leadLineCoord1.clear();
    leadLineCoord2.clear();
    subLineCoord1.clear();
    subLineCoord2.clear();

    if (idx > 0) {
      const IGisLine::point_t& pt1 = points[idx - 1];
      const IGisLine::point_t& pt2 = points[idx];

      bool res = parentHandler->useVectorRouting()
                     ? canvas->findPolylineCloseBy(pt2.pixel, pt2.pixel, 10, leadLineCoord1)
                     : CGisWorkspace::self().findPolylineCloseBy(pt2.pixel, pt2.pixel, 10, leadLineCoord1);
      if (res) {
        leadLinePixel1 = leadLineCoord1;
        gis->convertRad2Px(leadLinePixel1);

        segment_t result;
        GPS_Math_SubPolyline(pt1.pixel, pt2.pixel, 10, leadLinePixel1, result);
        result.apply(leadLineCoord1, leadLinePixel1, subLineCoord1, subLinePixel1, gis);
      }
    }

    if (idx < points.size() - 1) {
      const IGisLine::point_t& pt1 = points[idx];
      const IGisLine::point_t& pt2 = points[idx + 1];

      bool res = parentHandler->useVectorRouting()
                     ? canvas->findPolylineCloseBy(pt1.pixel, pt1.pixel, 10, leadLineCoord2)
                     : CGisWorkspace::self().findPolylineCloseBy(pt1.pixel, pt1.pixel, 10, leadLineCoord2);

      if (res) {
        leadLinePixel2 = leadLineCoord2;
        gis->convertRad2Px(leadLinePixel2);

        segment_t result;
        GPS_Math_SubPolyline(pt1.pixel, pt2.pixel, 10, leadLinePixel2, result);
        result.apply(leadLineCoord2, leadLinePixel2, subLineCoord2, subLinePixel2, gis);
      }
    }
  }
}

void ILineOp::showRoutingErrorMessage(const QString& msg) const {
  if (msg.isEmpty()) {
    canvas->reportStatus("Routino", QString());
  } else {
    canvas->reportStatus("Routino",
                         QString("<span style='color: red;'><b>%1</b><br />%2</span>").arg(tr("Routing"), msg));
  }
}

void ILineOp::tryRouting(IGisLine::point_t& pt1, IGisLine::point_t& pt2) const {
  QPolygonF subs;

  try {
    if (CRouterSetup::self().calcRoute(pt1.coord, pt2.coord, subs) >= 0) {
      pt1.subpts.clear();
      for (const QPointF& sub : std::as_const(subs)) {
        pt1.subpts << IGisLine::subpt_t(sub);
      }
    }
    showRoutingErrorMessage(QString());
  } catch (const QString& msg) {
    showRoutingErrorMessage(msg);
  }
  // that is a workaround for canvas loosing mouse tracking caused by CProgressDialog being modal:
  canvas->setMouseTracking(true);
}

void ILineOp::finalizeOperation(qint32 idx) {
  if (idx == NOIDX) {
    return;
  }

  if (parentHandler->useAutoRouting()) {
    CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
    if (idx > 0) {
      tryRouting(points[idx - 1], points[idx]);
    }
    if (idx < (points.size() - 1)) {
      tryRouting(points[idx], points[idx + 1]);
    }
  } else if (parentHandler->useVectorRouting() || parentHandler->useTrackRouting()) {
    if (idx > 0) {
      IGisLine::point_t& pt1 = points[idx - 1];
      pt1.subpts.clear();
      for (const QPointF& pt : std::as_const(subLineCoord1)) {
        pt1.subpts << IGisLine::subpt_t(pt);
      }
    }

    if (idx < (points.size() - 1)) {
      IGisLine::point_t& pt1 = points[idx];
      pt1.subpts.clear();
      for (const QPointF& pt : std::as_const(subLineCoord2)) {
        pt1.subpts << IGisLine::subpt_t(pt);
      }
    }
  }

  // need to move the mouse away by some pixels to trigger next routing event
  startMouseMove(points[idx].pixel);

  parentHandler->updateStatus();
}

qint32 ILineOp::isCloseTo(const QPoint& pos) const {
  qint32 min = 30;
  qint32 idx = NOIDX;
  const int N = points.size();
  for (int i = 0; i < N; i++) {
    const IGisLine::point_t& pt = points[i];

    qint32 d = (pos - pt.pixel).manhattanLength();
    if (d < min) {
      min = d;
      idx = i;
    }
  }

  return idx;
}

qint32 ILineOp::isCloseToLine(const QPoint& pos) const {
  qint32 idx = NOIDX;
  qreal dist = 60;

  for (int i = 0; i < points.size() - 1; i++) {
    QPolygonF line;
    const IGisLine::point_t& pt1 = points[i];
    const IGisLine::point_t& pt2 = points[i + 1];

    if (pt1.subpts.isEmpty()) {
      line << pt1.pixel << pt2.pixel;
    } else {
      line << pt1.pixel;
      for (const IGisLine::subpt_t& pt : pt1.subpts) {
        line << pt.pixel;
      }
      line << pt2.pixel;
    }

    qreal d = GPS_Math_DistPointPolyline(line, pos);
    if (d < dist) {
      dist = d;
      idx = i;
    }
  }

  return idx;
}
