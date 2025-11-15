/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "mouse/IScrOpt.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "gis/CGisWorkspace.h"
#include "mouse/IMouse.h"

IScrOpt::IScrOpt(IMouse* mouse) : QWidget(mouse == nullptr ? nullptr : mouse->getCanvas()), mouse(mouse) {
  backgroundColor = palette().color(QPalette::Window);
  setAttribute(Qt::WA_DeleteOnClose);
  setFocusPolicy(Qt::WheelFocus);
}

IScrOpt::~IScrOpt() {
  if (hasFocus() && !mouse.isNull()) {
    CCanvas::setOverrideCursor(*mouse, "IScrOpt::~IScrOpt");
  }
  CGisWorkspace::self().slotWksItemSelectionReset();
}

void IScrOpt::moveTo(const QPoint& anchor) {
  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  if (canvas == nullptr) {
    move(anchor + QPoint(-width() / 2, SCR_OPT_OFFSET));
    return;
  }

  qint32 xmin = 0;
  qint32 xmax = canvas->width() - width();
  qint32 ymax = canvas->height() - height() - SCR_OPT_OFFSET;

  QPoint pt = anchor + QPoint(-width() / 2, SCR_OPT_OFFSET);
  pt.rx() = qMax(xmin, pt.x());
  pt.rx() = qMin(xmax, pt.x());
  pt.ry() = qMin(ymax, pt.y());

  move(pt);
}

void IScrOpt::mouseMove(const QPoint& pos) { mousePos = pos; }

void IScrOpt::enterEvent(QEnterEvent* e) {
  QWidget::enterEvent(e);
  CCanvas::restoreOverrideCursor("IScrOpt::enterEvent");
}

void IScrOpt::leaveEvent(QEvent* e) {
  QWidget::leaveEvent(e);
  if (!mouse.isNull()) {
    CCanvas::setOverrideCursor(*mouse, "IScrOpt::leaveEvent");
  }
}

void IScrOpt::slotLinkActivated(const QString& link) {
  if (link.startsWith("http")) {
    QDesktopServices::openUrl(QUrl(link));
  }
}
