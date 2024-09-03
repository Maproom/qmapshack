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

#include "grid/CGrid.h"

#include <QtGui>
#include <QtWidgets>

#include "CMainWindow.h"
#include "helpers/CDraw.h"
#include "map/CMapDraw.h"

CGrid::CGrid(CMapDraw* map) : QObject(map), map(map) {}

CGrid::~CGrid() {}

void CGrid::convertPos2Str(const QPointF& pos, QString& info, bool simple) {
  if (!proj.isValid()) {
    return;
  }

  QPointF pt = pos;

  pt *= DEG_TO_RAD;
  proj.transform(pt, PJ_FWD);

  if (proj.isTarLatLong()) {
    QString lat, lng;
    pt *= RAD_TO_DEG;
    lat = pt.y() < 0 ? "S" : "N";
    lng = pt.x() < 0 ? "W" : "E";
    if (simple) {
      info += tr("%1 %2 ").arg(pt.y(), 0, 'f', 6).arg(pt.x(), 0, 'f', 6);
    } else {
      info += tr("%1%2%5 %3%4%5 ")
                  .arg(lat)
                  .arg(qAbs(pt.y()), 0, 'f', 6)
                  .arg(lng)
                  .arg(qAbs(pt.x()), 0, 'f', 6)
                  .arg(QChar('\260'));
    }
  } else {
    if (simple) {
      info += tr("%1m, %2m ").arg(pt.y(), 0, 'f', 0).arg(pt.x(), 0, 'f', 0);
    } else {
      info += tr("N %1m, E %2m ").arg(pt.y(), 0, 'f', 0).arg(pt.x(), 0, 'f', 0);
    }
  }
}

void CGrid::saveConfig(QSettings& cfg) {
  cfg.setValue("grid/color", color.name());
  cfg.setValue("grid/proj", proj.getProjTar());
}

void CGrid::loadConfig(QSettings& cfg) {
  color = QColor(cfg.value("grid/color", color.name()).toString());
  setProjAndColor(cfg.value("grid/proj", "EPSG:4326").toString(), color);
}

void CGrid::setProjAndColor(const QString& projStr, const QColor& c) {
  color = c;
  proj.init("EPSG:4326", projStr.toLatin1());
  if (!proj.isValid()) {
    QMessageBox::warning(CMainWindow::self().getBestWidgetForParent(), tr("Grid Projection..."),
                         tr("Failed to setup grid projection. Please configure a valid projection."), QMessageBox::Ok);

    QTimer::singleShot(1000, &CMainWindow::self(), &CMainWindow::slotSetupGrid);
  }
}

void CGrid::findGridSpace(qreal min, qreal max, qreal& xSpace, qreal& ySpace) {
  qreal dX = qAbs(min - max) / 10;
  if (dX < M_PI / 180000) {
    xSpace = 5 * M_PI / 1800000;
    ySpace = 5 * M_PI / 1800000;
  } else if (dX < M_PI / 18000) {
    xSpace = 5 * M_PI / 180000;
    ySpace = 5 * M_PI / 180000;
  } else if (dX < M_PI / 1800) {
    xSpace = 5 * M_PI / 18000;
    ySpace = 5 * M_PI / 18000;
  } else if (dX < M_PI / 180) {
    xSpace = 5 * M_PI / 1800;
    ySpace = 5 * M_PI / 1800;
  } else if (dX < M_PI / 18) {
    xSpace = 5 * M_PI / 180;
    ySpace = 5 * M_PI / 180;
  } else if (dX < M_PI / 1.8) {
    xSpace = 5 * M_PI / 180;
    ySpace = 5 * M_PI / 180;
  }

  else if (dX < 3000) {
    xSpace = 1000;
    ySpace = 1000;
  } else if (dX < 7000) {
    xSpace = 5000;
    ySpace = 5000;
  } else if (dX < 30000) {
    xSpace = 10000;
    ySpace = 10000;
  } else if (dX < 70000) {
    xSpace = 50000;
    ySpace = 50000;
  } else if (dX < 300000) {
    xSpace = 100000;
    ySpace = 100000;
  } else if (dX < 700000) {
    xSpace = 500000;
    ySpace = 500000;
  } else if (dX < 3000000) {
    xSpace = 1000000;
    ySpace = 1000000;
  } else if (dX < 7000000) {
    xSpace = 5000000;
    ySpace = 5000000;
  } else if (dX < 30000000) {
    xSpace = 10000000;
    ySpace = 10000000;
  } else if (dX < 70000000) {
    xSpace = 50000000;
    ySpace = 50000000;
  }
}

bool CGrid::calcIntersection(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4, qreal& x,
                             qreal& y) {
  qreal ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

  x = x1 + ua * (x2 - x1);
  y = y1 + ua * (y2 - y1);

  qreal d12 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
  qreal d1x = (x1 - x) * (x1 - x) + (y1 - y) * (y1 - y);
  qreal d2x = (x2 - x) * (x2 - x) + (y2 - y) * (y2 - y);
  qreal d34 = (x4 - x3) * (x4 - x3) + (y4 - y3) * (y4 - y3);
  qreal d3x = (x3 - x) * (x3 - x) + (y3 - y) * (y3 - y);
  qreal d4x = (x4 - x) * (x4 - x) + (y4 - y) * (y4 - y);

  return (d12 >= d1x) && (d12 >= d2x) && (d34 >= d3x) && (d34 >= d4x);
}

struct val_t {
  val_t(qint32 pos, qreal val) : pos(pos), val(val) {}
  qint32 pos;
  qreal val;
};

void CGrid::draw(QPainter& p, const QRect& rect) {
  if (!proj.isValid() || !CMainWindow::self().isGridVisible()) {
    return;
  }

  QPointF topLeft = rect.topLeft();
  QPointF topRight = rect.topRight();
  QPointF btmLeft = rect.bottomLeft();
  QPointF btmRight = rect.bottomRight();

  map->convertPx2Rad(topLeft);
  map->convertPx2Rad(topRight);
  map->convertPx2Rad(btmLeft);
  map->convertPx2Rad(btmRight);

  proj.transform(topLeft, PJ_FWD);
  proj.transform(topRight, PJ_FWD);
  proj.transform(btmLeft, PJ_FWD);
  proj.transform(btmRight, PJ_FWD);

  //    qDebug() << "---";
  //    qDebug() << "topLeft " << topLeft.u  << topLeft.v;
  //    qDebug() << "topRight" << topRight.u << topRight.v;
  //    qDebug() << "btmLeft " << btmLeft.u  << btmLeft.v;
  //    qDebug() << "btmRight" << btmRight.u << btmRight.v;

  //    qDebug() << topLeft.u - topRight.u;
  //    qDebug() << btmLeft.u - btmRight.u;

  //    qDebug() << topLeft.v  - btmLeft.v;
  //    qDebug() << topRight.v - btmRight.v;

  qreal topMax = qMax(topLeft.y(), topRight.y());
  qreal btmMin = qMin(btmLeft.y(), btmRight.y());
  qreal leftMin = qMin(topLeft.x(), btmLeft.x());
  qreal rightMax = qMax(topRight.x(), btmRight.x());

  qreal xGridSpace = 1000;
  qreal yGridSpace = 1000;
  findGridSpace(leftMin, rightMax, xGridSpace, yGridSpace);

  qreal xStart = qFloor(leftMin / xGridSpace) * xGridSpace;
  qreal yStart = qCeil(topMax / yGridSpace) * yGridSpace;

  qreal x = xStart - xGridSpace;
  qreal y = yStart + yGridSpace;

  if (proj.isTarLatLong()) {
    if (y > (85 * DEG_TO_RAD)) {
      y = (85 * DEG_TO_RAD);
    }
    if (btmMin < -(85 * DEG_TO_RAD - yGridSpace)) {
      btmMin = -(85 * DEG_TO_RAD - yGridSpace);
    }

    if (x > rightMax) {
      if (qAbs(x) > qAbs(rightMax)) {
        xStart = x = -180 * DEG_TO_RAD;
      }
      if (qAbs(x) < qAbs(rightMax)) {
        rightMax = 180 * DEG_TO_RAD;
      }
    }
  }

  QList<val_t> horzTopTicks;
  QList<val_t> horzBtmTicks;
  QList<val_t> vertLftTicks;
  QList<val_t> vertRgtTicks;

  p.save();
  p.setBrush(Qt::NoBrush);
  p.setPen(QPen(color, 1));
  USE_ANTI_ALIASING(p, false);

  qreal h = rect.height();
  qreal w = rect.width();

  while (y > btmMin) {
    while (x < rightMax) {
      QPointF p1(x, y);
      QPointF p2(x + xGridSpace, y);
      QPointF p3(x + xGridSpace, y - yGridSpace);
      QPointF p4(x, y - yGridSpace);

      qreal xVal = p1.x();
      qreal yVal = p1.y();

      proj.transform(p1, PJ_INV);
      proj.transform(p2, PJ_INV);
      proj.transform(p3, PJ_INV);
      proj.transform(p4, PJ_INV);

      //            qDebug() << (p1 * RAD_TO_DEG) << (p2 * RAD_TO_DEG) << (p3 * RAD_TO_DEG) << (p4 * RAD_TO_DEG);

      map->convertRad2Px(p1);
      map->convertRad2Px(p2);
      map->convertRad2Px(p3);
      map->convertRad2Px(p4);

      qreal xx, yy;
      if (calcIntersection(0, 0, w, 0, p1.x(), p1.y(), p4.x(), p4.y(), xx, yy)) {
        horzTopTicks << val_t(xx, xVal);
      }
      if (calcIntersection(0, h, w, h, p1.x(), p1.y(), p4.x(), p4.y(), xx, yy)) {
        horzBtmTicks << val_t(xx, xVal);
      }
      if (calcIntersection(0, 0, 0, h, p1.x(), p1.y(), p2.x(), p2.y(), xx, yy)) {
        vertLftTicks << val_t(yy, yVal);
      }
      if (calcIntersection(w, 0, w, h, p1.x(), p1.y(), p2.x(), p2.y(), xx, yy)) {
        vertRgtTicks << val_t(yy, yVal);
      }

      p.drawLine(p1, p2);
      p.drawLine(p2, p3);
      p.drawLine(p3, p4);
      p.drawLine(p4, p1);

      x += xGridSpace;
    }
    x = xStart;
    y -= yGridSpace;
  }
  USE_ANTI_ALIASING(p, true);
  p.restore();

  QColor textColor;
  textColor.setHsv(color.hslHue(), color.hsvSaturation(), (color.value() > 128 ? color.value() - 128 : 0));

  if (proj.isTarLatLong()) {
    QFontMetrics fm(CMainWindow::self().getMapFont());
    int yoff = fm.height() + fm.ascent();
    int xoff = fm.horizontalAdvance("XX.XXXX") >> 1;

    for (const val_t& val : std::as_const(horzTopTicks)) {
      CDraw::text(qAbs(val.val) < 1.e-5 ? "0" : QString("%1%2").arg(val.val * RAD_TO_DEG).arg(QChar(0260)), p,
                  QPoint(val.pos, yoff), textColor);
    }

    for (const val_t& val : std::as_const(horzBtmTicks)) {
      CDraw::text(qAbs(val.val) < 1.e-5 ? "0" : QString("%1%2").arg(val.val * RAD_TO_DEG).arg(QChar(0260)), p,
                  QPoint(val.pos, h), textColor);
    }

    for (const val_t& val : std::as_const(vertLftTicks)) {
      CDraw::text(qAbs(val.val) < 1.e-5 ? "0" : QString("%1%2").arg(val.val * RAD_TO_DEG).arg(QChar(0260)), p,
                  QPoint(xoff, val.pos), textColor);
    }

    for (const val_t& val : std::as_const(vertRgtTicks)) {
      CDraw::text(qAbs(val.val) < 1.e-5 ? "0" : QString("%1%2").arg(val.val * RAD_TO_DEG).arg(QChar(0260)), p,
                  QPoint(w - xoff, val.pos), textColor);
    }
  } else {
    QFontMetrics fm(CMainWindow::self().getMapFont());
    int yoff = fm.height() + fm.ascent();
    int xoff = fm.horizontalAdvance("XXXX") >> 1;

    for (const val_t& val : std::as_const(horzTopTicks)) {
      CDraw::text(QString("%1").arg(qint32(val.val / 1000)), p, QPoint(val.pos, yoff), textColor);
    }

    for (const val_t& val : std::as_const(horzBtmTicks)) {
      CDraw::text(QString("%1").arg(qint32(val.val / 1000)), p, QPoint(val.pos, h), textColor);
    }

    for (const val_t& val : std::as_const(vertLftTicks)) {
      CDraw::text(QString("%1").arg(qint32(val.val / 1000)), p, QPoint(xoff, val.pos), textColor);
    }

    for (const val_t& val : std::as_const(vertRgtTicks)) {
      CDraw::text(QString("%1").arg(qint32(val.val / 1000)), p, QPoint(w - xoff, val.pos), textColor);
    }
  }
}
