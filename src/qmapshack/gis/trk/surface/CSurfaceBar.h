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

#ifndef CSURFACEBAR_H
#define CSURFACEBAR_H

#include <QColor>
#include <QList>
#include <QWidget>

/// a horizontal bar of colored parts, e.g. the shares of the surface classes
class CSurfaceBar : public QWidget {
  Q_OBJECT
 public:
  explicit CSurfaceBar(QWidget* parent);
  ~CSurfaceBar() override = default;

  struct part_t {
    QColor color;
    qreal value;
    QString text;  ///< tooltip
  };
  void setParts(const QList<part_t>& parts);

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 protected:
  void paintEvent(QPaintEvent* e) override;
  bool event(QEvent* e) override;

 private:
  QList<part_t> parts;
  qreal total = 0;
};

#endif  // CSURFACEBAR_H
