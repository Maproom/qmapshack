/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "overlay/COverlayCutMap.h"

#include <QPainterPath>
#include <QtWidgets>
#include <functional>

#include "canvas/IDrawContext.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "items/CItemCutMap.h"

using std::bind;

static inline qreal sqr(qreal a) { return a * a; }

static inline qreal sqrlen(const QPointF& a) { return sqr(a.x()) + sqr(a.y()); }

COverlayCutMap::COverlayCutMap(CItemCutMap* item, QStackedWidget* stackedWidget)
    : IOverlay(stackedWidget), context(item->getDrawContext()) {
  setupUi(this);

  QFileInfo fi(item->getFilename());
  shapeFilename = fi.completeBaseName() + ".shp";

  connect(toolNone, &QToolButton::clicked, this,
          bind(&COverlayCutMap::slotSetMode, this, eModeNone, std::placeholders::_1));
  connect(toolPointMove, &QToolButton::clicked, this,
          bind(&COverlayCutMap::slotSetMode, this, eModePointMove, std::placeholders::_1));
  connect(toolPointAdd, &QToolButton::clicked, this,
          bind(&COverlayCutMap::slotSetMode, this, eModePointAdd, std::placeholders::_1));
  connect(toolPointDel, &QToolButton::clicked, this,
          bind(&COverlayCutMap::slotSetMode, this, eModePointDel, std::placeholders::_1));
  connect(toolPointDelAll, &QToolButton::clicked, this, &COverlayCutMap::slotResetMask);
  connect(toolLoadShape, &QToolButton::clicked, this, &COverlayCutMap::slotLoadShape);
  connect(toolSaveShape, &QToolButton::clicked, this, &COverlayCutMap::slotSaveShape);
}

void COverlayCutMap::saveSettings(QSettings& cfg) { cfg.setValue("region", region); }

void COverlayCutMap::loadSettings(QSettings& cfg) {
  region = (cfg.value("region", QPolygonF()).value<QPolygonF>());

  updateGui();
}

bool COverlayCutMap::drawFx(QPainter& p, CCanvas::redraw_e needsRedraw) {
  if (region.isEmpty()) {
    return true;
  }

  QPolygonF shape = region;
  context->convertMap2Screen(shape);

  if (shape.size() > 2) {
    QPainterPath path;

    QRectF rectMap = context->getMapArea();
    context->convertMap2Screen(rectMap);

    path.addRect(rectMap);
    path.addPolygon(shape);

    p.setPen(QPen(Qt::darkBlue, 1));

    p.setBrush((idxFocus1 == NOIDX) ? Qt::BDiagPattern : Qt::NoBrush);
    p.drawPath(path);
  }

  QRectF dot1(0, 0, 5, 5);
  QRectF dot2(0, 0, 7, 7);
  const QPointF& pt1 = idxFocus1 != NOIDX ? shape[idxFocus1] : NOPOINTF;
  const QPointF& pt2 = idxFocus2 != NOIDX ? shape[idxFocus2] : NOPOINTF;

  // draw black dots for each point in region
  p.setPen(QPen(Qt::black, 1));
  p.setBrush(Qt::black);
  for (const QPointF& pt : std::as_const(shape)) {
    if (pt == pt1) {
      continue;
    }
    dot1.moveCenter(pt);
    p.drawRect(dot1);
  }

  // if just one dot is selected draw crosshair
  if ((idxFocus2 == NOIDX) && (idxFocus1 != NOIDX)) {
    CDraw::drawCrossHairDot(p, pt1);
  } else if ((idxFocus2 != NOIDX) && (idxFocus1 == NOIDX)) {
    CDraw::drawCrossHairDot(p, pt2);
  }
  // if both points are selected highlight segment
  else if ((idxFocus2 != NOIDX) && (idxFocus1 != NOIDX)) {
    p.setPen(QPen(Qt::red, 2));
    p.setBrush(Qt::red);

    dot2.moveCenter(pt1);
    p.drawRect(dot2);
    dot2.moveCenter(pt2);
    p.drawRect(dot2);
    p.drawLine(pt1, pt2);
  }
  return true;
}

void COverlayCutMap::mouseMoveEventFx(QMouseEvent* e) {
  QPointF pt = e->pos();
  context->convertScreen2Map(pt);

  switch (mode) {
    case eModePointMove:
      mouseMovePointMove(pt);
      break;

    case eModePointAdd:
      mouseMovePointAdd(pt);
      break;

    case eModePointDel:
      mouseMovePointDel(pt);
      break;

    default:
      return;
  }

  updateGui();
  context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
}

void COverlayCutMap::mouseReleaseEventFx(QMouseEvent* e) {
  QPointF pt = e->pos();
  context->convertScreen2Map(pt);

  Qt::MouseButton button = e->button();

  switch (mode) {
    case eModePointMove:
      mouseReleasePointMove(pt, button);
      break;

    case eModePointAdd:
      mouseReleasePointAdd(pt, button);
      break;

    case eModePointDel:
      mouseReleasePointDel(pt, button);
      break;

    default:
      return;
  }

  updateGui();
  context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
}

QCursor COverlayCutMap::getCursorFx() {
  switch (mode) {
    case eModePointAdd:
      return QCursor(QPixmap(":/cursors/cursorPointAdd.png"), 0, 0);

    case eModePointDel:
      return QCursor(QPixmap(":/cursors/cursorPointDel.png"), 0, 0);

    case eModePointMove:
      return QCursor(QPixmap(":/cursors/cursorPointMove.png"), 0, 0);
  }

  return Qt::ArrowCursor;
}

void COverlayCutMap::slotSetMode(mode_e m, bool on) {
  if (on) {
    mode = m;
  }
}

void COverlayCutMap::slotResetMask() {
  int res = QMessageBox::question(this, tr("Delete mask..."), tr("Are you sure to delete complete mask?"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
  if (res != QMessageBox::Yes) {
    return;
  }

  mouseReset();
  region.clear();
  storeToHistory(region);
  updateGui();
}

void COverlayCutMap::slotSaveShape() {
  SETTINGS;
  QString path = cfg.value("Path/shapeInput", QDir::homePath()).toString();

  QString filename = QFileDialog::getSaveFileName(&CMainWindow::self(), tr("Save mask..."), path + "/" + shapeFilename,
                                                  "Shape file (*.shp)");
  if (filename.isEmpty()) {
    return;
  }
  cfg.setValue("Path/shapeInput", QFileInfo(shapeFilename).absolutePath());

  if (region.isEmpty()) {
    return;
  }
  shapeFilename = filename;
  saveShape(filename);
}

void COverlayCutMap::saveShape(const QString& filename) {
  const QPolygonF& line = region;

  QFile file(filename);
  file.open(QIODevice::WriteOnly);
  QTextStream out(&file);

  out << "id,WKT" << Qt::endl;
  out << "1,\"POLYGON((";

  bool first = true;
  for (const QPointF& pt : line) {
    QPointF pt1 = pt;
    context->convertMap2Proj(pt1);

    if (!first) {
      out << ", ";
    }
    first = false;

    out << qRound(pt1.x()) << " " << qRound(pt1.y());
  }
  out << "))\"" << Qt::endl;
}

void COverlayCutMap::slotLoadShape() {
  SETTINGS;
  QString path = cfg.value("Path/shapeInput", QDir::homePath()).toString();

  QString filename = QFileDialog::getOpenFileName(&CMainWindow::self(), tr("Load mask..."), path, "Shape file (*.shp)");
  if (filename.isEmpty()) {
    return;
  }
  cfg.setValue("Path/shapeInput", QFileInfo(filename).absolutePath());

  QFile file(filename);
  file.open(QIODevice::ReadOnly);
  QTextStream in(&file);

  QString line = in.readLine().simplified();
  if (line != "id,WKT") {
    QMessageBox::warning(&CMainWindow::self(), tr("Failed..."), tr("Not a shape file."), QMessageBox::Abort);
    return;
  }

  while (!in.atEnd()) {
    QString line = in.readLine().simplified();
    if (line.startsWith("1,\"POLYGON((")) {
      region.clear();

      line = line.mid(12);
      QTextStream in2(&line, QIODevice::ReadOnly);

      while (!in2.atEnd()) {
        qreal x, y;
        in2 >> x >> y;

        QPointF pt(x, y);
        context->convertProj2Map(pt);

        region << pt;

        QString str;
        in2 >> str;
        if (str != ",") {
          break;
        }
      }
    }
  }

  updateGui();
  context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
  emit sigChanged();
}

void COverlayCutMap::mouseReset() {
  addPoint = false;
  movePoint = false;
  idxFocus1 = NOIDX;
  idxFocus1 = NOIDX;
}

void COverlayCutMap::abortStep() {
  if (addPoint) {
    restoreFromHistory(region);
    addPoint = false;
    idxFocus1 = NOIDX;
    idxFocus2 = NOIDX;
  } else if (movePoint) {
    restoreFromHistory(region);
    movePoint = false;
    idxFocus1 = NOIDX;
  }

  context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
}

void COverlayCutMap::restoreFromHistory(QPolygonF& line) {
  line = history[idxHistory];
  emit sigChanged();
}

void COverlayCutMap::storeToHistory(const QPolygonF& line) {
  // crop history if necessary
  if (idxHistory != NOIDX) {
    while (history.size() > (idxHistory + 1)) {
      history.pop_back();
    }
  }

  history << line;
  idxHistory = history.size() - 1;

  emit sigChanged();
}

void COverlayCutMap::isCloseTo(QPointF pt, QPolygonF line, qint32& idx) const {
  idx = NOIDX;

  context->convertMap2Screen(pt);
  context->convertMap2Screen(line);

  if (line.isEmpty()) {
    return;
  }

  qint32 min = 30;
  const int N = region.size();
  for (int i = 0; i < N; i++) {
    qint32 d = (pt - line[i]).manhattanLength();
    if (d < min) {
      min = d;
      idx = i;
    }
  }
}

void COverlayCutMap::isCloseToLine(QPointF pt, QPolygonF line, qint32& idx1, qint32& idx2) const {
  idx1 = NOIDX;
  idx2 = NOIDX;

  context->convertMap2Screen(pt);
  context->convertMap2Screen(line);

  const int count = line.size();
  if (count < 2) {
    return;
  }

  QPointF b = line[0];
  QPointF dbq = b - pt;
  qreal dist = 30 * 30;

  for (qint32 i = 1; i < count; ++i) {
    const QPointF a = b;
    const QPointF daq = dbq;
    b = line[i];
    dbq = b - pt;

    const QPointF dab = a - b;

    const qreal inv_sqrlen = 1. / sqrlen(dab);
    const qreal t = (dab.x() * daq.x() + dab.y() * daq.y()) * inv_sqrlen;
    if (t < 0.) {
      continue;
    }
    qreal current_dist;
    if (t <= 1.) {
      current_dist = sqr(dab.x() * dbq.y() - dab.y() * dbq.x()) * inv_sqrlen;
    } else  // t>1.
    {
      current_dist = sqrlen(dbq);
    }

    if (current_dist < dist) {
      dist = current_dist;
      idx1 = i - 1;
      idx2 = i;
    }
  }
}

void COverlayCutMap::mouseMovePointAdd(const QPointF& pt) {
  if (region.isEmpty()) {
    return;
  }

  if (addPoint) {
    region[idxFocus1] = pt;
  } else {
    // find line segment close to cursor
    isCloseToLine(pt, region, idxFocus1, idxFocus2);
  }
}

void COverlayCutMap::mouseReleasePointAdd(const QPointF& pt, Qt::MouseButton button) {
  if (button == Qt::LeftButton) {
    if (addPoint) {
      // if isPoint is true the line has been appended/prepended
      // in this case go on with adding another point
      if (idxFocus1 == (region.size() - 1)) {
        idxFocus1++;
      }
      // store current state of line to undo/redo history
      storeToHistory(region);
      region.insert(idxFocus1, pt);
    } else if (idxFocus1 != NOIDX) {
      // add a new point to line segment
      idxFocus2 = NOIDX;
      idxFocus1++;
      region.insert(idxFocus1, pt);
      addPoint = true;
      CCanvas::setOverrideCursor(Qt::BlankCursor, "COverlayCutMap::mouseReleasePointAdd");
    } else if (region.isEmpty()) {
      region.append(pt);
      storeToHistory(region);
      region.append(pt);
      region.append(pt);
      idxFocus1 = 1;
      addPoint = true;
      CCanvas::setOverrideCursor(Qt::BlankCursor, "COverlayCutMap::mouseReleasePointAdd");
    }
  } else if (button == Qt::RightButton) {
    CCanvas::restoreOverrideCursor("COverlayCutMap::mouseReleasePointAdd");
    if (!addPoint) {
      toolNone->click();
    }
    abortStep();
  }
}

void COverlayCutMap::mouseMovePointDel(const QPointF& pt) { isCloseTo(pt, region, idxFocus1); }

void COverlayCutMap::mouseReleasePointDel(const QPointF& pt, Qt::MouseButton button) {
  if ((idxFocus1 != NOIDX) && (button == Qt::LeftButton)) {
    // special case first or last point.
    if ((idxFocus1 == 0) || (idxFocus1 == (region.size() - 1))) {
      if (region.size() > 2) {
        // more than one point set
        region.pop_back();
        region.pop_front();
      } else {
        // remove last point
        region.clear();
      }

      // close polygon area by appending the first point
      if (!region.isEmpty()) {
        region << region.first();
      }
    } else {
      region.remove(idxFocus1);
    }
    storeToHistory(region);
  } else if (button == Qt::RightButton) {
    CCanvas::restoreOverrideCursor("COverlayCutMap::mouseReleasePointDel");
    toolNone->click();
  }
  idxFocus1 = NOIDX;
}

void COverlayCutMap::mouseMovePointMove(const QPointF& pt) {
  if (movePoint) {
    if ((idxFocus1 == 0) || (idxFocus1 == (region.size() - 1))) {
      region.first() = pt;
      region.last() = pt;
    } else {
      region[idxFocus1] = pt;
    }
  } else {
    // no point selected yet, find point to highlight
    isCloseTo(pt, region, idxFocus1);
  }
}

void COverlayCutMap::mouseReleasePointMove(const QPointF& pt, Qt::MouseButton button) {
  if (button == Qt::LeftButton) {
    if (movePoint) {
      // terminate moving the point
      movePoint = false;
      // store new state of line to undo/redo history
      storeToHistory(region);
      CCanvas::restoreOverrideCursor("COverlayCutMap::mouseReleasePointAdd");
    } else if (idxFocus1 != NOIDX) {
      if ((idxFocus1 == 0) || (idxFocus1 == (region.size() - 1))) {
        region.first() = pt;
        region.last() = pt;
      } else {
        region[idxFocus1] = pt;
      }

      movePoint = true;
      CCanvas::setOverrideCursor(Qt::BlankCursor, "COverlayCutMap::mouseReleasePointMove");
    }
  } else if (button == Qt::RightButton) {
    CCanvas::restoreOverrideCursor("COverlayCutMap::mouseReleasePointMove");
    if (!movePoint) {
      toolNone->click();
    }
    abortStep();
  }
}

void COverlayCutMap::updateGui() {
  bool enable = !region.isEmpty();
  toolPointMove->setEnabled(enable);
  toolPointDel->setEnabled(enable);
  toolPointDelAll->setEnabled(enable);
  toolSaveShape->setEnabled(enable);
}
