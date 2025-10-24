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
  if (indexFocus != -1 && indexFocus < icons.size()) {
    emit sigSelectedIcon(icons[indexFocus].name);
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
        const QPixmap& icon = QPixmap(icons[index].path)
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
