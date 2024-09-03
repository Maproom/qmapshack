/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "mouse/CMouseRuler.h"

#include <QtWidgets>
#include <functional>

#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWorkspace.h"
#include "gis/GeoMath.h"
#include "gis/IGisItem.h"
#include "gis/IGisLine.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CDraw.h"
#include "mouse/CScrOptRuler.h"

CMouseRuler::CMouseRuler(CGisDraw* gis, CCanvas* canvas, CMouseAdapter* mouse) : IMouse(gis, canvas, mouse) {
  cursor = QCursor(QPixmap(":/cursors/cursorRuler.png"), 0, 0);

  scrOptRuler = new CScrOptRuler(this, canvas);
  connect(scrOptRuler->toolUndo, &QToolButton::clicked, this, &CMouseRuler::slotUndo);
  connect(scrOptRuler->toolRedo, &QToolButton::clicked, this, &CMouseRuler::slotRedo);
  connect(scrOptRuler->toolReset, &QToolButton::clicked, this, &CMouseRuler::slotReset);
  connect(scrOptRuler->toolShowTable, &QToolButton::clicked, this, [this]() { updateStatus(ruler); });
  connect(scrOptRuler->toolShowPrecision, &QToolButton::clicked, this, [this]() { updateStatus(ruler); });
  connect(scrOptRuler->toolToWpt, &QToolButton::clicked, this, &CMouseRuler::slotToWpt);
  connect(scrOptRuler->toolToTrk, &QToolButton::clicked, this, &CMouseRuler::slotToTrk);
  connect(scrOptRuler->toolToRte, &QToolButton::clicked, this, &CMouseRuler::slotToRte);
  connect(scrOptRuler->toolToArea, &QToolButton::clicked, this, &CMouseRuler::slotToArea);

  const QString msg = "<b>" + tr("Distance Ruler") + "</b><br/>" +
                      tr("Use the left mouse button to add points. "
                         "A right click will pause updating the ruler. "
                         "Another left click will continue to add points. ");

  canvas->reportStatus("CMouseRuler", msg);
}

CMouseRuler::~CMouseRuler() {
  canvas->reportStatus("CMouseRuler", "");
  delete scrOptRuler;
}

void CMouseRuler::meter2whatever(qreal meter, QString& val, QString& unit) {
  if (scrOptRuler->toolShowPrecision->isChecked()) {
    IUnit::self().meter2base(meter, val, unit);
  } else {
    IUnit::self().meter2distance(meter, val, unit);
  }
}

void CMouseRuler::storeToHistory(const QPolygonF& line) {
  // crop history if necessary
  if (idxHistory != NOIDX) {
    while (history.size() > (idxHistory + 1)) {
      history.pop_back();
    }
  }

  history << line;
  idxHistory = history.size() - 1;

  scrOptRuler->toolRedo->setEnabled(false);
  scrOptRuler->toolUndo->setEnabled(idxHistory > 0);
  scrOptRuler->toolReset->setEnabled(true);
  scrOptRuler->toolToWpt->setEnabled(true);
  scrOptRuler->toolToTrk->setEnabled(true);
  scrOptRuler->toolToRte->setEnabled(true);
  scrOptRuler->toolToArea->setEnabled(true);
}

void CMouseRuler::slotUndo() {
  if (idxHistory > 0) {
    idxHistory--;
  }

  ruler = history[idxHistory];

  scrOptRuler->toolRedo->setEnabled(true);
  scrOptRuler->toolUndo->setEnabled(idxHistory > 0);

  updateStatus(ruler);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CMouseRuler::slotRedo() {
  if (idxHistory < (history.size() - 1)) {
    idxHistory++;
  }

  ruler = history[idxHistory];

  scrOptRuler->toolRedo->setEnabled(idxHistory < (history.size() - 1));
  scrOptRuler->toolUndo->setEnabled(true);

  updateStatus(ruler);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CMouseRuler::slotReset() {
  ruler.clear();
  idxHistory = NOIDX;
  history.clear();
  scrOptRuler->toolRedo->setEnabled(false);
  scrOptRuler->toolUndo->setEnabled(false);
  scrOptRuler->toolReset->setEnabled(false);
  scrOptRuler->toolToWpt->setEnabled(false);
  scrOptRuler->toolToTrk->setEnabled(false);
  scrOptRuler->toolToRte->setEnabled(false);
  scrOptRuler->toolToArea->setEnabled(false);

  updateStatus(ruler);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CMouseRuler::slotToWpt() {
  QString icon;
  QString name;

  if (!CGisItemWpt::getIconAndName(icon, name)) {
    return;
  }

  IGisProject* project = CGisWorkspace::self().selectProject(false);
  if (nullptr == project) {
    return;
  }

  bool first = true;
  QMutexLocker lock(&IGisItem::mutexItems);
  for (const QPointF& pt : std::as_const(ruler)) {
    if (!first) {
      // update name after the first one
      name = CGisItemWpt::getLastName(name);
    } else {
      first = false;
    }
    qreal ele = canvas->getElevationAt(pt);
    CGisItemWpt* wpt = new CGisItemWpt(pt * RAD_TO_DEG, ele, QDateTime::currentDateTimeUtc(), name, icon, project);
    wpt->updateDecoration(CGisItemWpt::eMarkChanged, CGisItemWpt::eMarkNone);
  }
}

void CMouseRuler::slotToTrk() {
  QString name;
  IGisProject* project = nullptr;

  if (!IGisItem::getNameAndProject(name, project, tr("track"))) {
    return;
  }

  QMutexLocker lock(&IGisItem::mutexItems);
  new CGisItemTrk(SGisLine(ruler), name, project, NOIDX);
}

void CMouseRuler::slotToRte() {
  QString name;
  IGisProject* project = nullptr;

  if (!IGisItem::getNameAndProject(name, project, tr("route"))) {
    return;
  }
  QMutexLocker lock(&IGisItem::mutexItems);
  new CGisItemRte(SGisLine(ruler), name, project, NOIDX);
}

void CMouseRuler::slotToArea() {
  QString name;
  IGisProject* project = nullptr;

  if (!IGisItem::getNameAndProject(name, project, tr("area"))) {
    return;
  }
  QMutexLocker lock(&IGisItem::mutexItems);
  new CGisItemOvlArea(SGisLine(ruler), name, project, NOIDX);
}

void CMouseRuler::rightButtonDown(const QPoint& pos) {
  if (mode == eModeEdit) {
    mode = eModePaused;
    ruler.pop_back();
  }

  updateStatus(ruler);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CMouseRuler::leftClicked(const QPoint& pos) {
  mode = eModeEdit;

  QPointF coord = pos;
  gis->convertPx2Rad(coord);

  if (ruler.isEmpty()) {
    ruler << coord;
  }

  storeToHistory(ruler);
  updateStatus(ruler);

  ruler << coord;
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CMouseRuler::mouseMoved(const QPoint& pos) {
  if (ruler.isEmpty() || mode != eModeEdit) {
    return;
  }

  QPointF coord = pos;
  gis->convertPx2Rad(coord);

  qreal stepDistance = scrOptRuler->comboStepDist->currentData().toDouble();
  qreal stepAngle = scrOptRuler->comboStepAngle->currentData().toDouble();

  if ((stepDistance || stepAngle) && (ruler.count() > 1)) {
    const QPointF& coord0 = ruler[ruler.count() - 2];

    qreal a1, a2;
    qreal d = GPS_Math_Distance(coord0.x(), coord0.y(), coord.x(), coord.y(), a1, a2);

    if (stepDistance) {
      d = qRound(d / stepDistance) * stepDistance;
    }

    if (stepAngle) {
      a1 = qRound(a1 / stepAngle) * stepAngle;
    }

    coord = GPS_Math_Wpt_Projection(coord0, d, a1 * DEG_TO_RAD);
  }

  ruler.last() = coord;

  updateStatus(ruler);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CMouseRuler::updateStatus(const QPolygonF& line) {
  const int N = line.size();
  if ((N < 2) || !scrOptRuler->toolShowTable->isChecked()) {
    canvas->reportStatus("CMouseRuler", "");
    return;
  }

  qreal totalDistance = 0;
  qreal totalAscent = 0;
  qreal totalDescent = 0;

  QString msg = "<b>" + tr("Distance Ruler") + "</b>";
  msg += "<table border=1 cellspacing=0 cellpadding=2 width=100%>";
  msg += "<tr><th>#</th><th>" + tr("Distance") + "</th><th>" + tr("Ascent") + "</th><th>" + tr("Descent") +
         "</th><th>" + tr("Slope") + "</th><th>" + tr("Course") + "</th></tr>";

  for (int n = 1; n < N; n++) {
    QPointF pt1 = ruler[n - 1];
    QPointF pt2 = ruler[n];

    qreal a1, a2;
    qreal d = qRound(GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt2.y(), a1, a2));

    if (d == 0) {
      continue;
    }

    totalDistance += d;

    msg += QString("<tr><td>%1</td>").arg(n);

    QString val;
    QString unit;

    meter2whatever(d, val, unit);

    msg += "<td align=right>" + val + unit + "</td>";

    qreal ele1 = qRound(canvas->getElevationAt(pt1));
    qreal ele2 = qRound(canvas->getElevationAt(pt2));

    if ((ele1 != NOFLOAT) && (ele2 != NOFLOAT)) {
      int delta = ele2 - ele1;

      IUnit::self().meter2elevation(qAbs(delta), val, unit);
      if (delta >= 0) {
        totalAscent += delta;
        msg += QString("<td align=right>%1%2</td><td align=right></td>").arg(val, unit);
      } else {
        totalDescent -= delta;
        msg += QString("<td align=right></td><td align=right>%1%2</td>").arg(val, unit);
      }
      qreal slope = IUnit::slopeConvert(IUnit::eSlopePercent, delta / d * 100.0);
      IUnit::self().slope2string(slope, val, unit);
      msg += QString("<td align=right>%1%2</td>").arg(val, unit);
    } else {
      msg += "<td></td><td></td>";
    }

    val = QString::asprintf("%1.1f", (a1 < 0) ? (a1 + 360) : a1);
    msg += QString("<td align=right>%1°</td>").arg(val);

    msg += "</tr>";
  }

  if (N > 2) {
    QString val, unit;
    msg += "<tr><td><b>" + tr("Sum") + "</b></td>";
    meter2whatever(totalDistance, val, unit);
    msg += "<td align=right><b>" + val + unit + "</b></td>";
    IUnit::self().meter2elevation(totalAscent, val, unit);
    msg += "<td align=right><b>" + val + unit + "</b></td>";
    IUnit::self().meter2elevation(totalDescent, val, unit);
    msg += "<td align=right><b>" + val + unit + "</b></td></tr>";
  }

  msg += "</table>";
  canvas->reportStatus("CMouseRuler", msg);
}

void CMouseRuler::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect& rect) {
  const int N = ruler.size();

  if (N < 1) {
    return;
  }

  QPolygonF line = ruler;
  gis->convertRad2Px(line);

  if (N >= 2) {
    p.setPen(QPen(Qt::white, 5));
    p.drawPolyline(line);

    p.setPen(QPen(Qt::blue, 3));
    p.drawPolyline(line);

    QRectF r(0, 0, 50, 50);
    for (int n = 1; n < N; n++) {
      QPointF pt1 = ruler[n - 1];
      QPointF pt2 = ruler[n];

      qreal a1, a2;
      qreal d = GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt2.y(), a1, a2);

      if (d == 0) {
        continue;
      }

      if ((n > 1) && scrOptRuler->toolShowAngle->isChecked()) {
        r.moveCenter(line[n - 1]);

        QLineF seg1(line[n - 2], line[n - 1]);
        QLineF seg2(line[n - 1], line[n]);

        qreal angleStart = seg2.angle();
        qreal angleSpan = (seg2.angleTo(seg1) - 180);

        p.setPen(Qt::black);
        p.drawArc(r, angleStart * 16, angleSpan * 16);

        CDraw::text(QString("%1°").arg(qAbs(qRound(angleSpan))), p, line[n - 1], Qt::black);
      }

      QString val;
      QString unit;

      meter2whatever(d, val, unit);
      QString str = val + unit;

      qreal ele1 = canvas->getElevationAt(pt1);
      qreal ele2 = canvas->getElevationAt(pt2);

      if ((ele1 != NOFLOAT) && (ele2 != NOFLOAT) && scrOptRuler->toolShowAscent->isChecked()) {
        int delta = qRound(ele2 - ele1);

        IUnit::self().meter2elevation(qAbs(delta), val, unit);
        str += QString(", %1 %2%3").arg(delta > 0 ? QChar(0x2197) : QChar(0x2198)).arg(val, unit);
      }

      if (scrOptRuler->toolShowCourse->isChecked()) {
        val = QString::asprintf("%1.1f", (a1 < 0) ? (a1 + 360) : a1);
        str += QString(", %1°").arg(val);
      }

      QLineF seg(line[n - 1], line[n]);
      p.save();
      p.translate(seg.center().toPoint());
      p.rotate(a1 + ((a1 > 0) ? -90 : 90));
      CDraw::text(str, p, QPoint(0, 0), Qt::black);
      p.restore();
    }
  }

  p.setPen(QPen(Qt::white, 5));
  p.drawEllipse(line.first(), 2, 2);

  p.setPen(QPen(Qt::blue, 3));
  p.drawEllipse(line.first(), 2, 2);

  p.setPen(QPen(Qt::white, 5));
  p.drawEllipse(line.last(), 2, 2);

  p.setPen(QPen(Qt::blue, 3));
  p.drawEllipse(line.last(), 2, 2);
}
