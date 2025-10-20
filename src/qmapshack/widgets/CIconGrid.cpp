/**********************************************************************************************
    Copyright (C) 2025 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CIconGrid.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>

CIconGrid::CIconGrid(QScrollArea *parent) : QWidget(parent) { setMouseTracking(true); }

void CIconGrid::updateIconList(const QString &filter) {
  const QMap<QString, CWptIconManager::icon_t> &availableIcons = CWptIconManager::self().getWptIcons();

  if (filter.isEmpty()) {
    icons = availableIcons;
  } else {
    icons.clear();
    const QList<QString> &keys = availableIcons.keys();
    for (const QString &key : keys) {
      if (key.contains(filter, Qt::CaseInsensitive)) {
        icons[key] = availableIcons[key];
      }
    }
  }
  indexFocus = -1;
  repaint();
}

void CIconGrid::mouseMoveEvent(QMouseEvent *e) {
  const QPoint &pos = e->pos();
  const int n = pos.x() / tileSize;
  const int m = pos.y() / tileSize;
  const int newIndex = (pos.x() < cols * tileSize) ? m * cols + n : -1;

  if (newIndex != indexFocus) {
    const QList<QString> &names = icons.keys();
    if (newIndex != -1 && newIndex < names.size()) {
      emit sigIconName(names[newIndex]);
    } else {
      emit sigIconName("");
    }
    indexFocus = newIndex;
    repaint();
  }
}

void CIconGrid::resizeEvent(QResizeEvent *e) {
  cols = e->size().width() / tileSize;
  rows = ceil(icons.size() / float(cols));
  setMinimumHeight(rows * tileSize);
  indexFocus = -1;
}

void CIconGrid::paintEvent(QPaintEvent *e) {
  QPainter p(this);
  p.setPen(Qt::NoPen);
  p.setBrush(Qt::lightGray);
  p.drawRect(rect());

  p.setPen(Qt::black);
  p.setBrush(Qt::white);
  const QRect r(0, 0, tileSize, tileSize);
  const QList<CWptIconManager::icon_t> &values = icons.values();
  const int nIcons = values.size();
  for (int m = 0; m < rows; m++) {
    for (int n = 0; n < cols; n++) {
      const int index = m * cols + n;
      if (index < nIcons) {
        p.save();
        p.translate(n * tileSize, m * tileSize);
        if (index == indexFocus) {
          p.setBrush(Qt::lightGray);
        }
        p.drawRect(r);
        p.drawPixmap(16, 16, QPixmap(values[index].path).scaled(32, 32));
        p.restore();
      }
    }
  }
}
