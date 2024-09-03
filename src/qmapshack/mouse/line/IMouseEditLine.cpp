/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
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

#include "mouse/line/IMouseEditLine.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "gis/GeoMath.h"
#include "gis/IGisLine.h"
#include "gis/rte/router/CRouterOptimization.h"
#include "gis/trk/CGisItemTrk.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "mouse/CMouseAdapter.h"
#include "mouse/line/CLineOpAddPoint.h"
#include "mouse/line/CLineOpDeletePoint.h"
#include "mouse/line/CLineOpMovePoint.h"
#include "mouse/line/CLineOpSelectRange.h"
#include "mouse/line/CScrOptEditLine.h"
#include "units/IUnit.h"

IMouseEditLine::IMouseEditLine(const IGisItem::key_t& key, const QPointF& point, bool enableStatus, const QString& type,
                               CGisDraw* gis, CCanvas* canvas, CMouseAdapter* mouse)
    : IMouse(gis, canvas, mouse), key(key), enableStatus(enableStatus), type(type) {
  commonSetup();
  scrOptEditLine->pushSaveOrig->hide();  // hide as there is no original

  points << IGisLine::point_t(point);
  points.updatePixel(gis);

  storeToHistory(points);
}

IMouseEditLine::IMouseEditLine(const IGisItem::key_t& key, IGisLine& src, bool enableStatus, const QString& type,
                               CGisDraw* gis, CCanvas* canvas, CMouseAdapter* mouse)
    : IMouse(gis, canvas, mouse), key(key), enableStatus(enableStatus), type(type) {
  commonSetup();

  src.getPolylineFromData(points);
  points.updatePixel(gis);

  storeToHistory(points);
}

IMouseEditLine::~IMouseEditLine() {
  canvas->reportStatus("IMouseEditLine", "");
  canvas->reportStatus(key.item, "");
  canvas->reportStatus("Optimization", "");

  int mode = 0;
  if (scrOptEditLine->toolNoRoute->isChecked()) {
    mode = 0;
  } else if (scrOptEditLine->toolAutoRoute->isChecked()) {
    mode = 1;
  } else if (scrOptEditLine->toolVectorRoute->isChecked()) {
    mode = 2;
  } else if (scrOptEditLine->toolTrackRoute->isChecked()) {
    mode = 3;
  }

  SETTINGS;
  cfg.setValue("Route/drawMode", mode);

  delete scrOptEditLine;
}

void IMouseEditLine::commonSetup() {
  // create permanent line edit on screen options
  scrOptEditLine = new CScrOptEditLine(this);
  connect(scrOptEditLine->pushSaveOrig, &QPushButton::clicked, this, &IMouseEditLine::slotCopyToOrig);
  connect(scrOptEditLine->pushSaveNew, &QPushButton::clicked, this, &IMouseEditLine::slotCopyToNew);
  connect(scrOptEditLine->pushAbort, &QPushButton::clicked, this, &IMouseEditLine::slotAbort);

  connect(scrOptEditLine->toolMovePoint, &QPushButton::clicked, this, &IMouseEditLine::slotMovePoint);
  connect(scrOptEditLine->toolSelectRange, &QPushButton::clicked, this, &IMouseEditLine::slotSelectRange);
  connect(scrOptEditLine->toolAddPoint, &QPushButton::clicked, this, &IMouseEditLine::slotAddPoint);
  connect(scrOptEditLine->toolDeletePoint, &QPushButton::clicked, this, &IMouseEditLine::slotDeletePoint);

  connect(scrOptEditLine->toolNoRoute, &QPushButton::clicked, this, &IMouseEditLine::slotNoRouting);
  connect(scrOptEditLine->toolAutoRoute, &QPushButton::clicked, this, &IMouseEditLine::slotAutoRouting);
  connect(scrOptEditLine->toolVectorRoute, &QPushButton::clicked, this, &IMouseEditLine::slotVectorRouting);
  connect(scrOptEditLine->toolTrackRoute, &QPushButton::clicked, this, &IMouseEditLine::slotTrackRouting);
  connect(scrOptEditLine->pushOptimize, &QPushButton::clicked, this, &IMouseEditLine::slotOptimize);

  connect(scrOptEditLine->toolUndo, &QPushButton::clicked, this, &IMouseEditLine::slotUndo);
  connect(scrOptEditLine->toolRedo, &QPushButton::clicked, this, &IMouseEditLine::slotRedo);

  SETTINGS;
  int mode = cfg.value("Route/drawMode", 0).toInt();
  switch (mode) {
    case 0:
      scrOptEditLine->toolNoRoute->setChecked(true);
      break;

    case 1:
      scrOptEditLine->toolAutoRoute->setChecked(true);
      break;

    case 2:
      scrOptEditLine->toolVectorRoute->setChecked(true);
      break;

    case 3:
      scrOptEditLine->toolTrackRoute->setChecked(true);
      break;
  }

  slotMovePoint();
}

void IMouseEditLine::abortStep() {
  // at first try to abort a step within the current operation (px. stop adding a new waypoint)
  if (!lineOp->abortStep()) {
    // if within operation nothing can be aborted, then abort the whole operation
    // this equals clicking the `abort` button
    slotAbortEx(true);
  }
}

bool IMouseEditLine::useAutoRouting() const { return scrOptEditLine->toolAutoRoute->isChecked(); }

bool IMouseEditLine::useVectorRouting() const { return scrOptEditLine->toolVectorRoute->isChecked(); }

bool IMouseEditLine::useTrackRouting() const { return scrOptEditLine->toolTrackRoute->isChecked(); }

void IMouseEditLine::drawLine(const QPolygonF& l, const QColor color, int width, QPainter& p) {
  p.setPen(QPen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  p.drawPolyline(l);
}

void IMouseEditLine::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect& rect) {
  if (needsRedraw & (CCanvas::eRedrawMouse | CCanvas::eRedrawGis)) {
    points.updatePixel(gis);

    pixelLine.clear();
    pixelPts.clear();
    pixelSubs.clear();

    for (const IGisLine::point_t& pt : std::as_const(points)) {
      pixelLine << pt.pixel;
      pixelPts << pt.pixel;

      for (const IGisLine::subpt_t& sub : pt.subpts) {
        pixelLine << sub.pixel;
        pixelSubs << sub.pixel;
      }
    }
  }

  if (pixelPts.isEmpty()) {
    return;
  }

  lineOp->drawBg(p);

  drawLine(pixelLine, Qt::white, 7, p);

  // draw magenta arrows (with white background)
  p.setBrush(Qt::magenta);
  CDraw::arrows(pixelLine, QRectF(), p, 10, 120, 2.0);

  p.setPen(Qt::NoPen);
  p.setBrush(Qt::white);
  QRect r1(0, 0, 9, 9);
  for (const QPointF& pt : std::as_const(pixelPts)) {
    r1.moveCenter(pt.toPoint());
    p.drawRect(r1);
  }

  drawLine(pixelLine, Qt::magenta, 5, p);

  p.setPen(Qt::NoPen);
  p.setBrush(Qt::black);
  QRect r2(0, 0, 7, 7);
  for (const QPointF& pt : std::as_const(pixelPts)) {
    r2.moveCenter(pt.toPoint());
    p.drawRect(r2);
  }

  for (const QPointF& pt : std::as_const(pixelSubs)) {
    p.drawEllipse(pt, 2, 2);
  }

  QRect r3(0, 0, 9, 9);
  p.setBrush(Qt::NoBrush);

  p.setPen(QPen(Qt::yellow, 2));
  r3.moveCenter(pixelPts.first().toPoint());
  p.drawRect(r3);

  p.setPen(QPen(Qt::green, 2));
  r3.moveCenter(pixelPts.last().toPoint());
  p.drawRect(r3);

  lineOp->drawFg(p);
}

void IMouseEditLine::startNewLine(const QPointF& point) {
  scrOptEditLine->toolAddPoint->setChecked(true);
  slotAddPoint();

  CLineOpAddPoint* lineOpAddPoint = dynamic_cast<CLineOpAddPoint*>(lineOp);
  if (lineOpAddPoint) {
    lineOpAddPoint->append();
  }

  canvas->reportStatus(key.item, tr("<b>New Line</b><br/>Move the mouse and use the left mouse button to drop points. "
                                    "When done use the right mouse button to stop.<br/>") +
                                     docPanning);
}

void IMouseEditLine::leftButtonDown(const QPoint& pos) {
  canvas->reportStatus(key.item, "");
  lineOp->leftButtonDown(pos);
}

void IMouseEditLine::rightButtonDown(const QPoint& pos) {
  canvas->reportStatus(key.item, "");
  lineOp->rightButtonDown(pos);
}

void IMouseEditLine::mouseMoved(const QPoint& pos) {
  lineOp->mouseMove(pos);

  canvas->displayInfo(pos);
  canvas->update();
}

void IMouseEditLine::leftClicked(const QPoint& pos) { lineOp->leftClick(pos); }

void IMouseEditLine::scaleChanged() { lineOp->scaleChanged(); }

void IMouseEditLine::slotDeletePoint() {
  canvas->reportStatus(
      key.item,
      tr("<b>Delete Point</b><br/>Move the mouse close to a point and press the left button to delete it.<br/>") +
          docPanning);
  delete lineOp;
  lineOp = new CLineOpDeletePoint(points, gis, canvas, this);
  changeCursor();
}

void IMouseEditLine::slotSelectRange() {
  canvas->reportStatus(
      key.item, tr("<b>Select Range of Points</b><br/>Left click on first point to start selection. Left click second "
                   "point to complete selection and choose from options. Use the right mouse button to cancel.<br/>") +
                    docPanning);
  delete lineOp;
  lineOp = new CLineOpSelectRange(points, gis, canvas, this);
  changeCursor();
}

void IMouseEditLine::slotMovePoint() {
  canvas->reportStatus(key.item, tr("<b>Move Point</b><br/>Move the mouse close to a point and press the left button "
                                    "to make it stick to the cursor. Move the mouse to move the point. Drop the point "
                                    "by a left click. Use the right mouse button to cancel.<br/>") +
                                     docPanning);
  delete lineOp;
  lineOp = new CLineOpMovePoint(points, gis, canvas, this);
  changeCursor();
}

void IMouseEditLine::slotAddPoint() {
  canvas->reportStatus(key.item, tr("<b>Add Point</b><br/>Move the mouse close to a line segment and press the left "
                                    "button to add a point. The point will stick to the cursor and you can move it.  "
                                    "Drop the point by a left click. Use the right mouse button to cancel.<br/>") +
                                     docPanning);
  delete lineOp;
  lineOp = new CLineOpAddPoint(points, gis, canvas, this);
  changeCursor();
}

void IMouseEditLine::slotNoRouting() {
  canvas->reportStatus(key.item, tr("<b>No Routing</b><br/>All points will be connected with a straight line.<br/>"));
  canvas->reportStatus("Routino", QString());
}

void IMouseEditLine::slotAutoRouting() {
  canvas->reportStatus(key.item, tr("<b>Auto Routing</b><br/>The current router setup is used to derive a route "
                                    "between points. <b>Note:</b> The selected router must be able to route "
                                    "on-the-fly. Offline routers usually can do, online routers can't.<br/>"));
}

void IMouseEditLine::slotVectorRouting() {
  canvas->reportStatus(
      key.item, tr("<b>Vector Routing</b><br/>Connect points with a line from a loaded vector map if possible.<br/>"));
  canvas->reportStatus("Routino", QString());
}

void IMouseEditLine::slotTrackRouting() {
  canvas->reportStatus(key.item,
                       tr("<b>Track Routing</b><br/>Connect points with a line from a loaded track if possible.<br/>"));
  canvas->reportStatus("Routino", QString());
}

void IMouseEditLine::slotOptimize() {
  canvas->reportStatus(key.item, "");
  canvas->reportStatus("Optimization", QString("<b>%1</b><br/>").arg(tr("Started Optimization.")));

  int response = 0;
  try {
    response = optimizer.optimize(points);
  } catch (const QString& msg) {
    response = -1;
    lineOp->showRoutingErrorMessage(msg);
  }

  if (response == -1) {
    canvas->reportStatus("Optimization", QString("<b>%1</b><br/><b>%2</b> %3<br/>")
                                             .arg(tr("Optimization failed."), tr("Note:"),
                                                  tr("The selected router must be able to route on-the-fly. Offline "
                                                     "routers usually can do, online routers can't.")));
  } else {
    canvas->reportStatus("Optimization", QString("<b>%1</b><br/>").arg(tr("Optimization successful.")));

    points.updatePixel(gis);

    storeToHistory(points);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
  }
  updateStatus();
}

void IMouseEditLine::changeCursor() {
  cursor = lineOp->getCursor();
  if (QApplication::overrideCursor() != 0) {
    CCanvas::changeOverrideCursor(cursor, "IMouseEditLine::changeCursor");
  }
}

void IMouseEditLine::slotAbortEx(bool showMB) {
  // ask if the current operation should be aborted (only in case there are things to be saved)
  bool doAbort = (idxHistory == 0) || !showMB;
  if (!doAbort) {
    doAbort = (QMessageBox::Yes ==
               QMessageBox::question(nullptr, "Abort",
                                     "Do you really want to abort?\nAny modifications done will be discarded.",
                                     QMessageBox::Yes | QMessageBox::No));
  }

  if (doAbort) {
    canvas->resetMouse();
    canvas->update();
  }
}

void IMouseEditLine::slotCopyToOrig() {
  QMutexLocker lock(&IGisItem::mutexItems);

  IGisLine* line = getGisLine();
  if (line != nullptr) {
    CMainWindow::self().getElevationAt(points);
    line->setDataFromPolyline(points);
  }

  canvas->resetMouse();
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void IMouseEditLine::restoreFromHistory(SGisLine& line) {
  line = history[idxHistory];
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);

  updateStatus();
}

void IMouseEditLine::storeToHistory(const SGisLine& line) {
  // crop history if necessary
  if (idxHistory != NOIDX) {
    while (history.size() > (idxHistory + 1)) {
      history.pop_back();
    }
  }

  history << line;
  idxHistory = history.size() - 1;

  scrOptEditLine->toolRedo->setEnabled(false);
  scrOptEditLine->toolUndo->setEnabled(idxHistory > 0);

  updateStatus();
}

void IMouseEditLine::slotUndo() {
  if (lineOp != nullptr) {
    lineOp->abortStep();
  }

  if (idxHistory > 0) {
    idxHistory--;
  }

  points = history[idxHistory];

  scrOptEditLine->toolRedo->setEnabled(true);
  scrOptEditLine->toolUndo->setEnabled(idxHistory > 0);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
  updateStatus();
}

void IMouseEditLine::slotRedo() {
  // abort operation
  if (lineOp != nullptr) {
    lineOp->abortStep();
  }

  if (idxHistory < (history.size() - 1)) {
    idxHistory++;
  }

  points = history[idxHistory];

  scrOptEditLine->toolRedo->setEnabled(idxHistory < (history.size() - 1));
  scrOptEditLine->toolUndo->setEnabled(true);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
  updateStatus();
}

void IMouseEditLine::updateStatus() {
  if (!enableStatus || points.isEmpty()) {
    canvas->reportStatus("IMouseEditLine", QString());
    return;
  }

  canvas->getElevationAt(points);

  qreal asc = 0;
  qreal dsc = 0;
  qreal dist = 0;

  qreal lastEle = points[0].ele;
  QPointF lastPos = points[0].coord;

  for (const IGisLine::point_t& pt1 : std::as_const(points)) {
    qreal delta = pt1.ele - lastEle;
    if (qAbs(delta) > ASCENT_THRESHOLD) {
      if (delta > 0) {
        asc += delta;
      }
      if (delta < 0) {
        dsc -= delta;
      }
      lastEle = pt1.ele;
    }

    dist += GPS_Math_Distance(lastPos.x(), lastPos.y(), pt1.coord.x(), pt1.coord.y());
    lastPos = pt1.coord;

    for (const IGisLine::subpt_t& pt : pt1.subpts) {
      delta = pt.ele - lastEle;
      if (qAbs(delta) > ASCENT_THRESHOLD) {
        if (delta > 0) {
          asc += delta;
        }
        if (delta < 0) {
          dsc -= delta;
        }
        lastEle = pt.ele;
      }

      dist += GPS_Math_Distance(lastPos.x(), lastPos.y(), pt.coord.x(), pt.coord.y());
      lastPos = pt.coord;
    }
  }

  QString msg, val, unit;

  msg += tr("<b>%1 Metrics</b>").arg(type);
  msg += "<table>";
  IUnit::self().meter2distance(dist, val, unit);
  msg += "<tr><td>" + tr("Distance:") + "</td><td>" + QString("&nbsp;%1 %2").arg(val, unit) + "</td></tr>";
  IUnit::self().meter2elevation(asc, val, unit);
  msg += "<tr><td>" + tr("Ascent:") + "</td><td>" + QString("&nbsp;%1 %2").arg(val, unit) + "</td></tr>";
  IUnit::self().meter2elevation(dsc, val, unit);
  msg += "<tr><td>" + tr("Descent:") + "</td><td>" + QString("&nbsp;%1 %2").arg(val, unit) + "</td></tr>";
  msg += "</table>";

  canvas->reportStatus("IMouseEditLine", msg);
}
