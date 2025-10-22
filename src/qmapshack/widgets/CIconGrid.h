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

#ifndef CICONGRID_H
#define CICONGRID_H

#include <QScrollArea>
#include <QWidget>

#include "helpers/CWptIconManager.h"

class CIconGrid : public QWidget {
  Q_OBJECT
 public:
  CIconGrid(QScrollArea *parent);

  void updateIconList(const QString &filter, const QString &category);

  static constexpr int kTileSize = 64;
  static constexpr int kVisibleRows = 5;
  static constexpr int kVisibleCols = 8;

 signals:
  void sigIconName(const QString &name);
  void sigSelectedIcon(const QString &name);

 protected:
  void mouseMoveEvent(QMouseEvent *e) override;
  void mousePressEvent(QMouseEvent *e) override;
  void resizeEvent(QResizeEvent *e) override;
  void paintEvent(QPaintEvent *e) override;

 private:
  void setIndexFocus(int i);

  QMap<QString, CWptIconManager::icon_t> icons;

  int indexFocus = -1;
  int rows = -1;
  int cols = -1;

  QScrollArea *scrollArea;
};

#endif  // CICONGRID_H
