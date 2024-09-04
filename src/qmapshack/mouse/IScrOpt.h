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

#ifndef ISCROPT_H
#define ISCROPT_H

#include <QMouseEvent>
#include <QPixmap>
#include <QPointer>
#include <QRect>
#include <QSemaphore>
#include <QWidget>

class IMouse;

#define SCR_OPT_OFFSET 15

class IScrOpt : public QWidget {
 public:
  IScrOpt(IMouse* mouse);
  virtual ~IScrOpt();

  void setOrigin(const QPoint& pos) { origin = pos; }

  const QPoint& getOrigin() const { return origin; }

  virtual void draw(QPainter& p) = 0;
  virtual void mouseMove(const QPoint& pos);

  bool isNotLocked() const { return semaphore.available(); }

 protected slots:
  void slotLinkActivated(const QString& link);

 protected:
  void enterEvent(QEnterEvent* e) override;
  void leaveEvent(QEvent* e) override;
  // block mouse actions to hit the canvas
  void mousePressEvent(QMouseEvent* e) override { e->accept(); }
  // block mouse actions to hit the canvas
  void mouseMoveEvent(QMouseEvent* e) override { e->accept(); }

  void moveTo(const QPoint& anchor);

  QPoint origin;
  QPoint mousePos;

  QPointer<IMouse> mouse;

  friend class CScrOptSemaphoreLocker;
  QSemaphore semaphore{1};

  QColor backgroundColor;
};

#endif  // ISCROPT_H
