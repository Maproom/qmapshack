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

#include "canvas/CCanvas.h"

#include <QtWidgets>

#include "canvas/IDrawContext.h"
#include "helpers/CDraw.h"

CCanvas::CCanvas(QWidget* parent) : QWidget(parent) {
  setFocusPolicy(Qt::WheelFocus);
  setMouseTracking(true);

  loadIndicator1 = new QMovie("://animation/loader.gif", QByteArray(), this);
  mapLoadIndicator = new QLabel(this);
  mapLoadIndicator->setMovie(loadIndicator1);
  loadIndicator1->start();
  mapLoadIndicator->show();
}

void CCanvas::setOverrideCursor(const QCursor& cursor, const QString&) {
  //    qDebug() << "setOverrideCursor" << src;
  QApplication::setOverrideCursor(cursor);
}

void CCanvas::restoreOverrideCursor(const QString& src) {
  //    qDebug() << "restoreOverrideCursor" << src;
  QApplication::restoreOverrideCursor();
}

void CCanvas::changeOverrideCursor(const QCursor& cursor, const QString& src) {
  //    qDebug() << "changeOverrideCursor" << src;
  QApplication::changeOverrideCursor(cursor);
}

void CCanvas::resizeEvent(QResizeEvent* e) {
  QMutexLocker lock(&mutex);

  needsRedraw = eRedrawAll;

  // move map loading indicator to new center of canvas
  QPoint p1(mapLoadIndicator->width() >> 1, mapLoadIndicator->height() >> 1);
  mapLoadIndicator->move(rect().center() - p1);

  emit sigChangedSize(e->size());

  QWidget::resizeEvent(e);
}

void CCanvas::paintEvent(QPaintEvent* e) {
  QPainter p;
  p.begin(this);
  USE_ANTI_ALIASING(p, true);

  // fill the background with default pattern
  p.fillRect(rect(), backColor);

  // ----- start to draw thread based content -----

  mutex.lock();
  if (!tool->drawFx(p, needsRedraw)) {
    mutex.unlock();
    slotHideLoadIndicator();
    CDraw::text(tr("No map view available."), p, rect(), Qt::black);
    mutex.lock();
  }
  mutex.unlock();
  // ----- start to draw fast content -----
  p.end();
  needsRedraw = eRedrawNone;
}

void CCanvas::mousePressEvent(QMouseEvent* e) {
  QMutexLocker lock(&mutex);
  tool->mousePressEventFx(e);
  e->accept();
}

void CCanvas::mouseMoveEvent(QMouseEvent* e) {
  QMutexLocker lock(&mutex);
  tool->mouseMoveEventFx(e);
  e->accept();
}

void CCanvas::mouseReleaseEvent(QMouseEvent* e) {
  QMutexLocker lock(&mutex);
  tool->mouseReleaseEventFx(e);
  e->accept();
}

void CCanvas::mouseDoubleClickEvent(QMouseEvent* e) {
  QMutexLocker lock(&mutex);
  tool->mouseDoubleClickEventFx(e);
  e->accept();
}

void CCanvas::wheelEvent(QWheelEvent* e) {
  QMutexLocker lock(&mutex);
  tool->wheelEventFx(e);
}

void CCanvas::enterEvent(QEnterEvent* e) {
  QMutexLocker lock(&mutex);

  tool->enterEventFx(e);
  CCanvas::setOverrideCursor(tool->getCursorFx(), "enterEvent");

  setMouseTracking(true);
}

void CCanvas::leaveEvent(QEvent* e) {
  QMutexLocker lock(&mutex);

  tool->leaveEventFx(e);

  // bad hack to stop bad number of override cursors.
  while (QApplication::overrideCursor()) {
    CCanvas::restoreOverrideCursor("leaveEvent");
  }

  setMouseTracking(false);
}

void CCanvas::keyPressEvent(QKeyEvent* e) {
  QMutexLocker lock(&mutex);
  if (!tool->keyPressEventFx(e)) {
    e->ignore();
  }
}

void CCanvas::slotTriggerCompleteUpdate(CCanvas::redraw_e flags) {
  needsRedraw = (redraw_e)(needsRedraw | flags);
  update();
}

void CCanvas::slotShowLoadIndicator() {
  QMutexLocker lock(&mutex);
  mapLoadIndicator->show();
  mapLoadIndicatorCount++;
}

void CCanvas::slotHideLoadIndicator() {
  QMutexLocker lock(&mutex);
  if (--mapLoadIndicatorCount <= 0) {
    mapLoadIndicator->hide();
    mapLoadIndicatorCount = 0;
  }
}
