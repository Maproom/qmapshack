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
#include <QScrollBar>

CIconGrid::CIconGrid(QScrollArea *parent) : QWidget(parent), scrollArea(parent) { setMouseTracking(true); }

void CIconGrid::updateIconList(const QString &filter, const QString &category) {
  QMap<QString, CWptIconManager::icon_t> availableIcons;

  // filter by category
  if (category.isEmpty()) {
    availableIcons = CWptIconManager::self().getWptIcons();
  } else {
    const QMap<QString, CWptIconManager::icon_t> &icons = CWptIconManager::self().getWptIcons();
    const QStringList &keys = icons.keys();
    for (const QString &key : keys) {
      if (icons[key].categories.contains(category)) {
        availableIcons[key] = icons[key];
      }
    }
  }

  // filter by name and tags
  if (filter.isEmpty()) {
    icons = availableIcons;
  } else {
    icons.clear();
    const QList<QString> &keys = availableIcons.keys();
    for (const QString &key : keys) {
      const CWptIconManager::icon_t &icon = availableIcons[key];

      if (key.contains(filter, Qt::CaseInsensitive)) {
        icons[key] = icon;
      } else {
        const QStringList &tags = icon.tags;
        if (!tags.filter(filter, Qt::CaseInsensitive).isEmpty()) {
          icons[key] = icon;
        }
      }
    }
  }

  setIndexFocus(-1);
  scrollArea->verticalScrollBar()->setValue(0);
  QCoreApplication::postEvent(this, new QResizeEvent(size(), QSize()));
}

void CIconGrid::mouseMoveEvent(QMouseEvent *e) {
  const QPoint &pos = e->pos();
  const int n = pos.x() / kTileSize;
  const int m = pos.y() / kTileSize;
  const int newIndex = (pos.x() < cols * kTileSize) ? m * cols + n : -1;

  if (newIndex != indexFocus) {
    setIndexFocus(newIndex);
  }
}

void CIconGrid::mousePressEvent(QMouseEvent *e) {
  const QList<QString> &names = icons.keys();
  if (indexFocus != -1 && indexFocus < names.size()) {
    emit sigSelectedIcon(names[indexFocus]);
  }
}

void CIconGrid::resizeEvent(QResizeEvent *e) {
  cols = e->size().width() / kTileSize;
  rows = ceil(icons.size() / float(cols));
  setIndexFocus(-1);
  setMinimumHeight(rows * kTileSize);
  repaint();
}

void CIconGrid::paintEvent(QPaintEvent *e) {
  QPainter p(this);
  p.setPen(Qt::NoPen);
  p.setBrush(Qt::gray);
  p.drawRect(rect());

  p.setPen(Qt::NoPen);
  p.setBrush(Qt::white);
  const QRect r(0, 0, kTileSize, kTileSize);
  const QList<CWptIconManager::icon_t> &values = icons.values();
  const int nIcons = values.size();
  for (int m = 0; m < rows; m++) {
    for (int n = 0; n < cols; n++) {
      const int index = m * cols + n;
      if (index < nIcons) {
        p.save();
        p.translate(n * kTileSize, m * kTileSize);
        if (index == indexFocus) {
          p.setBrush(Qt::lightGray);
        }
        p.drawRect(r);
        p.drawPixmap(kTileSize / 4, kTileSize / 4, QPixmap(values[index].path).scaled(kTileSize / 2, kTileSize / 2));
        p.restore();
      }
    }
  }
}

void CIconGrid::setIndexFocus(int newIndex) {
  const QList<QString> &names = icons.keys();
  if (newIndex != -1 && newIndex < names.size()) {
    emit sigIconName(names[newIndex]);
  } else {
    emit sigIconName("");
  }
  indexFocus = newIndex;
  repaint();
}
