/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CSCALEWIDGET_H
#define CSCALEWIDGET_H

#include <QWidget>

class CScaleWidget : public QWidget {
  Q_OBJECT
 public:
  CScaleWidget(QWidget* parent);
  virtual ~CScaleWidget() = default;

  void setValue(qreal min, qreal scale, qreal max);

 protected:
  void paintEvent(QPaintEvent* e) override;

 private:
  void updatePixmap();

  qreal minScale = 0;
  qreal maxScale = 0;
  qreal currentScale = 0;
};

#endif  // CSCALEWIDGET_H
