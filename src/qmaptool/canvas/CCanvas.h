/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CCANVAS_H
#define CCANVAS_H

#include <QLabel>
#include <QMovie>
#include <QMutex>
#include <QPointer>
#include <QWidget>

class ITool;
class CMainWindow;
class QStackedWidget;

class CCanvas : public QWidget {
  Q_OBJECT
 public:
  CCanvas(QWidget* parent);
  virtual ~CCanvas() = default;

  static void setOverrideCursor(const QCursor& cursor, const QString&);
  static void restoreOverrideCursor(const QString& src);
  static void changeOverrideCursor(const QCursor& cursor, const QString& src);

  enum redraw_e { eRedrawNone = 0, eRedrawMap = 0x01, eRedrawOverlay = 0x08, eRedrawAll = 0xFFFFFFFF };

  void setToolInterface(ITool* t) { tool = t; }

 signals:
  void sigChangedSize(const QSize& size);

 public slots:
  void slotTriggerCompleteUpdate(CCanvas::redraw_e flags);

  void slotShowLoadIndicator();
  void slotHideLoadIndicator();

 protected:
  void resizeEvent(QResizeEvent* e) override;
  void paintEvent(QPaintEvent* e) override;
  void mousePressEvent(QMouseEvent* e) override;
  void mouseMoveEvent(QMouseEvent* e) override;
  void mouseReleaseEvent(QMouseEvent* e) override;
  void mouseDoubleClickEvent(QMouseEvent* e) override;
  void wheelEvent(QWheelEvent* e) override;
  void enterEvent(QEnterEvent* e) override;
  void leaveEvent(QEvent* e) override;
  void keyPressEvent(QKeyEvent* e) override;

 private:
  mutable QRecursiveMutex mutex;

  QColor backColor = 0xFFFFFFBF;      //< the background color used in case of missing map tiles
  redraw_e needsRedraw = eRedrawAll;  //< set true to initiate a complete redraw of the screen content

  /// load indicator for maps
  QMovie* loadIndicator1;
  QLabel* mapLoadIndicator;
  qint32 mapLoadIndicatorCount = 0;

  ITool* tool = nullptr;
};

#endif  // CCANVAS_H
