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

#include "map/CMapRMAP.h"

#include <QtGui>
#include <QtWidgets>

#include "CMainWindow.h"
#include "helpers/CDraw.h"
#include "map/CMapDraw.h"
#include "units/IUnit.h"

CMapRMAP::CMapRMAP(const QString& filename, CMapDraw* parent) : IMap(eFeatVisibility, parent), filename(filename) {
  qDebug() << "------------------------------";
  qDebug() << "RMAP: try to open" << filename;

  QFile file(filename);
  file.open(QIODevice::ReadOnly);
  //    qDebug() << file.errorString();

  QDataStream stream(&file);
  stream.setByteOrder(QDataStream::LittleEndian);

  QByteArray charbuf(20, 0);
  stream.readRawData(charbuf.data(), 19);

  if ("CompeGPSRasterImage" != QString(charbuf.data())) {
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("This is not a TwoNav RMAP file."),
                         QMessageBox::Abort, QMessageBox::Abort);
    return;
  }

  quint32 tag1, tag2, tmp32;
  stream >> tag1 >> tag2 >> tmp32;

  if (tag1 != 10 || tag2 != 7) {
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Unknown sub-format."),
                         QMessageBox::Abort, QMessageBox::Abort);
    return;
  }

  stream >> xsize_px >> ysize_px;
  stream >> tmp32 >> tmp32;
  stream >> tileSizeX >> tileSizeY;

  ysize_px = -ysize_px;

  quint64 mapDataOffset;
  stream >> mapDataOffset;
  stream >> tmp32;

  qint32 nZoomLevels;
  stream >> nZoomLevels;

  for (int i = 0; i < nZoomLevels; i++) {
    level_t level;
    stream >> level.offsetLevel;
    levels << level;
  }

  for (int i = 0; i < levels.size(); i++) {
    level_t& level = levels[i];
    file.seek(level.offsetLevel);

    stream >> level.width;
    stream >> level.height;
    stream >> level.xTiles;
    stream >> level.yTiles;

    for (int j = 0; j < (level.xTiles * level.yTiles); j++) {
      quint64 offset;
      stream >> offset;
      level.offsetJpegs << offset;
    }
  }

  file.seek(mapDataOffset);
  stream >> tmp32 >> tmp32;

  charbuf.resize(tmp32 + 1);
  charbuf.fill(0);
  stream.readRawData(charbuf.data(), tmp32);

  QPoint p0;
  QPoint p1;
  QPoint p2;
  QPoint p3;
  QPointF c0;
  QPointF c1;
  QPointF c2;
  QPointF c3;

  bool pointsAreLongLat = true;
  QString projection;
  QString datum;
  const QStringList& lines = QString(charbuf).split("\r\n");

  for (const QString& line : lines) {
    //        qDebug() << line;
    if (line.startsWith("Version=")) {
      if (line.split("=")[1] != "2") {
        QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Unknown version."),
                             QMessageBox::Abort, QMessageBox::Abort);
        return;
      }
    } else if (line.startsWith("Projection=")) {
      projection = line.split("=")[1];
    } else if (line.startsWith("Datum=")) {
      datum = line.split("=")[1];
    } else if (line.startsWith("P0=")) {
      QStringList vals = line.split("=")[1].split(",");
      if (vals.size() < 5) {
        QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                             tr("Failed to read reference point."), QMessageBox::Abort, QMessageBox::Abort);
        return;
      }

      p0 = QPoint(vals[0].toInt(), vals[1].toInt());
      if (vals[2] == "A") {
        c0.rx() = vals[3].toDouble() * DEG_TO_RAD;
        c0.ry() = vals[4].toDouble() * DEG_TO_RAD;
      } else {
        c0.rx() = vals[3].toDouble();
        c0.ry() = vals[4].toDouble();
      }
    } else if (line.startsWith("P1=")) {
      QStringList vals = line.split("=")[1].split(",");
      if (vals.size() < 5) {
        QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                             tr("Failed to read reference point."), QMessageBox::Abort, QMessageBox::Abort);
        return;
      }

      p1 = QPoint(vals[0].toInt(), vals[1].toInt());
      if (vals[2] == "A") {
        c1.rx() = vals[3].toDouble() * DEG_TO_RAD;
        c1.ry() = vals[4].toDouble() * DEG_TO_RAD;
      } else {
        pointsAreLongLat = false;
        c1.rx() = vals[3].toDouble();
        c1.ry() = vals[4].toDouble();
      }
    } else if (line.startsWith("P2=")) {
      QStringList vals = line.split("=")[1].split(",");
      if (vals.size() < 5) {
        QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                             tr("Failed to read reference point."), QMessageBox::Abort, QMessageBox::Abort);
        return;
      }

      p2 = QPoint(vals[0].toInt(), vals[1].toInt());
      if (vals[2] == "A") {
        c2.rx() = vals[3].toDouble() * DEG_TO_RAD;
        c2.ry() = vals[4].toDouble() * DEG_TO_RAD;
      } else {
        pointsAreLongLat = false;
        c2.rx() = vals[3].toDouble();
        c2.ry() = vals[4].toDouble();
      }
    } else if (line.startsWith("P3=")) {
      QStringList vals = line.split("=")[1].split(",");
      if (vals.size() < 5) {
        QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                             tr("Failed to read reference point."), QMessageBox::Abort, QMessageBox::Abort);
        return;
      }

      p3 = QPoint(vals[0].toInt(), vals[1].toInt());
      if (vals[2] == "A") {
        c3.rx() = vals[3].toDouble() * DEG_TO_RAD;
        c3.ry() = vals[4].toDouble() * DEG_TO_RAD;
      } else {
        pointsAreLongLat = false;
        c3.rx() = vals[3].toDouble();
        c3.ry() = vals[4].toDouble();
      }
    } else {
      //            qDebug() << line;
    }
    // qDebug() << line;
  }

  if (!projection.isEmpty() && !datum.isEmpty()) {
    if (!setProjection(projection, datum)) {
      QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                           tr("Unknown projection and datum (%1%2).").arg(projection, datum), QMessageBox::Abort,
                           QMessageBox::Abort);
      return;
    }
  }

  if (!proj.isSrcLatLong()) {
    if (pointsAreLongLat) {
      proj.transform(c0, PJ_INV);
      proj.transform(c1, PJ_INV);
      proj.transform(c2, PJ_INV);
      proj.transform(c3, PJ_INV);
    }

    //        qDebug() << c0.x() << c0.ry();
    //        qDebug() << c1.x() << c1.ry();
    //        qDebug() << c2.x() << c2.ry();
    //        qDebug() << c3.x() << c3.ry();

    xref1 = NOFLOAT;
    yref1 = -NOFLOAT;
    xref2 = -NOFLOAT;
    yref2 = NOFLOAT;
  } else {
    xref1 = 180 * DEG_TO_RAD;
    yref1 = -90 * DEG_TO_RAD;
    xref2 = -180 * DEG_TO_RAD;
    yref2 = 90 * DEG_TO_RAD;
  }

  xref1 = qMin(xref1, c0.x());
  xref2 = qMax(xref2, c0.x());
  xref1 = qMin(xref1, c1.x());
  xref2 = qMax(xref2, c1.x());
  xref1 = qMin(xref1, c2.x());
  xref2 = qMax(xref2, c2.x());

  yref1 = qMax(yref1, c0.y());
  yref2 = qMin(yref2, c0.y());
  yref1 = qMax(yref1, c1.y());
  yref2 = qMin(yref2, c1.y());
  yref1 = qMax(yref1, c2.y());
  yref2 = qMin(yref2, c2.y());

  scale.rx() = (xref2 - xref1) / xsize_px;
  scale.ry() = (yref2 - yref1) / ysize_px;

  qreal widthL0 = levels[0].width;
  qreal heightL0 = levels[0].height;

  for (int i = 0; i < levels.size(); i++) {
    level_t& level = levels[i];

    level.xscale = scale.x() * widthL0 / level.width;
    level.yscale = scale.y() * heightL0 / level.height;

    // qDebug() << i << level.xscale << level.yscale;
  }

  isActivated = true;

  //    qDebug() << "xref1:" << xref1 << "yref1:" << yref1;
  //    qDebug() << "xref2:" << xref2 << "yref2:" << yref2;
  //    qDebug() << "map  width:" << xsize_px << "height:" << ysize_px;
  //    qDebug() << "tile width:" << tileSizeX << "height:" << tileSizeY;
  //    qDebug() << "scale x:  " << scale.x() << "y:" << scale.y();
}

bool CMapRMAP::setProjection(const QString& projection, const QString& datum) {
  QString projstr;
  if (projection.startsWith("0,UTM")) {
    QStringList vals = projection.split(",");
    int zone = vals[2].toInt();
    bool isSouth = vals[3] != "N";

    projstr += QString("+proj=utm +zone=%1 %2").arg(zone).arg(isSouth ? "+south" : "");
  }
  if (projection.startsWith("1,")) {
    projstr += "+proj=longlat";
  } else if (projection.startsWith("2,")) {
    projstr += "+proj=merc";
  } else if (projection.startsWith("114,")) {
    projstr += "+proj=tmerc +lat_0=0 +lon_0=12 +k=1 +x_0=4500000 +y_0=0";
  } else if (projection.startsWith("117,")) {
    projstr += "+proj=tmerc +lat_0=0 +lon_0=9  +k=1 +x_0=3500000 +y_0=0";
  }

  if (datum == "WGS 84") {
    projstr += " +datum=WGS84 +units=m +no_defs";
  } else if (datum == "Potsdam Rauenberg DHDN") {
    projstr += " +ellps=bessel +towgs84=606,23,413,0,0,0,0 +units=m +no_defs";
  }

  proj.init(projstr.toLatin1(), "EPSG:4326");
  if (!proj.isValid()) {
    return false;
  }

  qDebug() << "rmap:" << proj.getProjSrc();

  return true;
}

CMapRMAP::level_t& CMapRMAP::findBestLevel(const QPointF& s) {
  int i = levels.size() - 1;
  if (s.x() < levels[0].xscale) {
    return levels[0];
  }
  if (s.x() > levels[i].xscale) {
    return levels[i];
  }

  int j = 0;
  qreal dsx = NOFLOAT;
  for (; j < levels.size(); j++) {
    level_t& level = levels[j];
    if (qAbs(level.xscale - s.x()) < dsx) {
      i = j;
      dsx = qAbs(level.xscale - s.x());
    }
  }

  return levels[i];
}

void CMapRMAP::draw(IDrawContext::buffer_t& buf) /* override */
{
  if (map->needsRedraw()) {
    return;
  }

  // convert top left buffer corner
  // into buffer's coordinate system
  QPointF pp = buf.ref1;
  map->convertRad2Px(pp);

  // find best level for buffer's scale factor derived from base scale and zoom factor
  QPointF bufferScale = buf.scale * buf.zoomFactor;

  if (isOutOfScale(bufferScale)) {
    return;
  }

  level_t& level = findBestLevel(bufferScale);

  // convert top left and bottom right point of buffer to local coord. system
  QPointF p1 = buf.ref1;
  QPointF p2 = buf.ref3;

  convertRad2M(p1);
  convertRad2M(p2);

  // find indices into tile buffer
  int idxx1 = qFloor((p1.x() - xref1) / (level.xscale * tileSizeX));
  int idxy1 = qFloor((p1.y() - yref1) / (level.yscale * tileSizeY));
  int idxx2 = qCeil((p2.x() - xref1) / (level.xscale * tileSizeX));
  int idxy2 = qCeil((p2.y() - yref1) / (level.yscale * tileSizeY));

  idxx1 = qMax(0, idxx1);
  idxx1 = qMin(idxx1, level.xTiles);

  idxx2 = qMax(0, idxx2);
  idxx2 = qMin(idxx2, level.xTiles);

  idxy1 = qMax(0, idxy1);
  idxy1 = qMin(idxy1, level.yTiles);

  idxy2 = qMax(0, idxy2);
  idxy2 = qMin(idxy2, level.yTiles);

  // ----- start drawing -----
  QPainter p(&buf.image);
  USE_ANTI_ALIASING(p, true);
  p.setOpacity(getOpacity() / 100.0);
  p.translate(-pp);

  QFile file(filename);
  file.open(QIODevice::ReadOnly);

  QDataStream stream(&file);
  stream.setByteOrder(QDataStream::LittleEndian);

  for (int idxy = idxy1; idxy < idxy2; idxy++) {
    if (map->needsRedraw()) {
      break;
    }

    for (int idxx = idxx1; idxx < idxx2; idxx++) {
      if (map->needsRedraw()) {
        break;
      }

      quint32 tag;
      quint32 len;
      quint64 offset = level.getOffsetJpeg(idxx, idxy);
      file.seek(offset);
      stream >> tag >> len;

      QImage img;
      img.load(&file, "JPG");

      if (img.isNull()) {
        continue;
      }

      qreal imgw = img.width();
      qreal imgh = img.height();

      // derive tile's corner coordinate
      QPolygonF l(4);
      l[0].rx() = xref1 + idxx * tileSizeX * level.xscale;
      l[0].ry() = yref1 + idxy * tileSizeY * level.yscale;
      l[1].rx() = xref1 + (idxx * tileSizeX + imgw) * level.xscale;
      l[1].ry() = yref1 + idxy * tileSizeY * level.yscale;
      l[2].rx() = xref1 + (idxx * tileSizeX + imgw) * level.xscale;
      l[2].ry() = yref1 + (idxy * tileSizeY + imgh) * level.yscale;
      l[3].rx() = xref1 + idxx * tileSizeX * level.xscale;
      l[3].ry() = yref1 + (idxy * tileSizeY + imgh) * level.yscale;

      proj.transform(l, PJ_FWD);

      drawTile(img, l, p);
    }
  }
}
