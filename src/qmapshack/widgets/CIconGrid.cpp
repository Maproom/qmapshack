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

#include "helpers/CDraw.h"

CIconGrid::CIconGrid(QScrollArea *parent) : QWidget(parent) { setMouseTracking(true); }

void CIconGrid::updateIconList(const QList<CWptIconManager::icon_t> &visibleIcons) {
  icons = visibleIcons;
  setIndexFocus(-1);
  QCoreApplication::postEvent(this, new QResizeEvent(size(), QSize()));
}

int CIconGrid::indexAt(const QPoint &pos) const {
  if (cols < 1 || pos.x() < 0 || pos.y() < 0 || pos.x() >= cols * kTileSize) {
    return -1;
  }
  const int index = (pos.y() / kTileSize) * cols + (pos.x() / kTileSize);
  return (index < icons.size()) ? index : -1;
}

QString CIconGrid::iconAt(const QPoint &pos) const {
  const int index = indexAt(pos);
  return (index == -1) ? QString() : icons[index].name;
}

QRect CIconGrid::rectOfIcon(const QString &name) const {
  if (cols < 1) {
    return {};
  }
  for (int i = 0; i < icons.size(); i++) {
    if (icons[i].name == name) {
      return QRect((i % cols) * kTileSize, (i / cols) * kTileSize, kTileSize, kTileSize);
    }
  }
  return {};
}

void CIconGrid::mouseMoveEvent(QMouseEvent *e) {
  const int newIndex = indexAt(e->pos());
  if (newIndex != indexFocus) {
    setIndexFocus(newIndex);
  }
}

void CIconGrid::mousePressEvent(QMouseEvent *e) {
  // The point, not indexFocus: a click without a move before it has no focus.
  const int index = indexAt(e->pos());
  if (index != -1) {
    setIndexFocus(index);
    emit sigSelectedIcon(icons[index].name);
  }
}

void CIconGrid::resizeEvent(QResizeEvent *e) {
  cols = e->size().width() / kTileSize;
  rows = ceil(icons.size() / float(cols));
  setIndexFocus(-1);
  setMinimumWidth(cols * kTileSize);
  setMinimumHeight(rows * kTileSize);
  repaint();
}

void CIconGrid::paintEvent(QPaintEvent *e) {
  QPainter p(this);
  USE_ANTI_ALIASING(p, true);
  p.setPen(Qt::NoPen);
  p.setBrush(Qt::gray);
  p.drawRect(rect());

  p.setPen(Qt::NoPen);
  p.setBrush(Qt::white);
  const QRect rectTile(0, 0, kTileSize, kTileSize);

  const int nIcons = icons.size();
  for (int m = 0; m < rows; m++) {
    for (int n = 0; n < cols; n++) {
      const int index = m * cols + n;
      if (index < nIcons) {
        p.save();
        p.translate(n * kTileSize, m * kTileSize);
        if (index == indexFocus) {
          p.setBrush(Qt::lightGray);
        }
        p.drawRect(rectTile);
        const QPixmap &icon = QPixmap(icons[index].path)
                                  .scaled(kTileSize / 2, kTileSize / 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QRect rectIcon = icon.rect();
        rectIcon.moveCenter(rectTile.center());
        p.drawPixmap(rectIcon, icon);
        p.restore();
      }
    }
  }
}

void CIconGrid::setIndexFocus(int newIndex) {
  if (newIndex != -1 && newIndex < icons.size()) {
    emit sigIconName(icons[newIndex].name);
  } else {
    emit sigIconName("");
  }
  indexFocus = newIndex;
  repaint();
}
