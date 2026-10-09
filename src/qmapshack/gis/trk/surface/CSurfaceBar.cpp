/**********************************************************************************************
    Copyright (C) 2026 Gert Pellin <gert@pellin.be>

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

#include "gis/trk/surface/CSurfaceBar.h"

#include <QHelpEvent>
#include <QPainter>
#include <QPainterPath>
#include <QToolTip>

CSurfaceBar::CSurfaceBar(QWidget* parent) : QWidget(parent) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void CSurfaceBar::setParts(const QList<part_t>& newParts) {
  parts = newParts;
  total = 0;
  for (const part_t& part : std::as_const(parts)) {
    total += part.value;
  }
  update();
}

QSize CSurfaceBar::sizeHint() const { return QSize(200, fontMetrics().height()); }

QSize CSurfaceBar::minimumSizeHint() const { return QSize(50, fontMetrics().height()); }

void CSurfaceBar::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  const QRectF r = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);

  QPainterPath clip;
  clip.addRoundedRect(r, 3, 3);
  p.setClipPath(clip);

  qreal x = r.left();
  for (const part_t& part : std::as_const(parts)) {
    if (total <= 0 || part.value <= 0) {
      continue;
    }
    const qreal w = r.width() * part.value / total;
    p.fillRect(QRectF(x, r.top(), w, r.height()), part.color);
    x += w;
  }

  p.setClipping(false);
  p.setPen(QPen(palette().color(QPalette::Mid), 1));
  p.setBrush(Qt::NoBrush);
  p.drawRoundedRect(r, 3, 3);
}

bool CSurfaceBar::event(QEvent* e) {
  if (e->type() == QEvent::ToolTip && total > 0) {
    QHelpEvent* help = static_cast<QHelpEvent*>(e);
    qreal x = 0;
    for (const part_t& part : std::as_const(parts)) {
      x += width() * part.value / total;
      if (help->pos().x() <= x) {
        QToolTip::showText(help->globalPos(), part.text, this);
        return true;
      }
    }
  }
  return QWidget::event(e);
}
