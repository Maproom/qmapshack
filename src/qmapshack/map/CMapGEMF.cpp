/**********************************************************************************************
    Copyright (C) 2016 Peter Schumann <peter.schumann@jeepxj.de>

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
/*
 * Map Driver Implementation for GEMF Maps
 * SPECs --> http://www.cgtk.co.uk/gemf
 */

#include "map/CMapGEMF.h"

#include <QDebug>
#include <QtGui>
#include <QtWidgets>

#include "CMainWindow.h"
#include "helpers/CDraw.h"
#include "map/CMapDraw.h"
#include "units/IUnit.h"

#define NAMEBUFLEN 1024

inline int lon2tile(double lon, int z) { return (int)(qRound(256 * (lon + 180.0) / 360.0 * qPow(2.0, z))); }

inline int lat2tile(double lat, int z) {
  return (int)(qRound(256 * (1.0 - log(qTan(lat * M_PI / 180.0) + 1.0 / qCos(lat * M_PI / 180.0)) / M_PI) / 2.0 *
                      qPow(2.0, z)));
}

inline double tile2lon(int x, int z) { return x / qPow(2.0, z) * 360.0 - 180; }

inline double tile2lat(int y, int z) {
  double n = M_PI - 2.0 * M_PI * y / qPow(2.0, z);
  return 180.0 / M_PI * qAtan(0.5 * (exp(n) - exp(-n)));
}

CMapGEMF::CMapGEMF(const QString& filename, CMapDraw* parent) : IMap(eFeatVisibility, parent), filename(filename) {
  qDebug() << "CMapGEMF: try to open " << filename;
  proj.init(
      "EPSG:3857",
      "EPSG:4326");
  qDebug() << "CMapGEMF:" << proj.getProjSrc();

  QFile file(filename);
  file.open(QIODevice::ReadOnly);

  QDataStream stream(&file);
  stream.setByteOrder(QDataStream::BigEndian);

  stream >> version;
  stream >> tileSize;
  stream >> sourceNr;

  for (quint32 i = 0; i < sourceNr; i++) {
    source_t source;
    quint32 len;
    QByteArray name(NAMEBUFLEN, 0);
    stream >> source.index;
    stream >> len;
    if (len > NAMEBUFLEN) {
      qDebug() << "CMapGEMF: Name longer than " << NAMEBUFLEN << " byte";
      len = NAMEBUFLEN;
    }

    stream.readRawData(name.data(), len);
    source.name = QString().fromLocal8Bit(name, len);
    sources << source;
    qDebug() << "CMapGEMF: Read Source " << i << " " << source.name;
  }

  stream >> rangeNum;
  QList<range_t> ranges;
  quint64 tiles = 0;
  for (quint32 i = 0; i < rangeNum; i++) {
    range_t range;
    stream >> range.zoomlevel;
    stream >> range.minX;
    stream >> range.maxX;
    stream >> range.minY;
    stream >> range.maxY;
    stream >> range.sourceIdx;
    stream >> range.offset;

    ranges << range;
    tiles += (range.maxX + 1 - range.minX) * (range.maxY + 1 - range.minY);
  }
  qDebug() << "CMapGEMF: Read " << rangeNum << "Ranges with " << tiles << " Tiles";

  minZoom = MAX_ZOOM_LEVEL;
  maxZoom = MIN_ZOOM_LEVEL;

  for (quint32 i = 0; i <= MAX_ZOOM_LEVEL; i++) {
    QList<range_t> rangeZoom;
    for (const range_t& range : std::as_const(ranges)) {
      if (range.zoomlevel == i) {
        rangeZoom << range;
        minZoom = qMin(i, minZoom);
        maxZoom = qMax(i, maxZoom);
      }
    }
    if (!rangeZoom.empty()) {
      rangesByZoom[i] = rangeZoom;
      qDebug() << "CMapGEMF: Found " << rangeZoom.length() << " ranges for zoomlevel " << i;
    }
  }
  QString partfile = filename;
  QFile f(partfile);
  f.open(QIODevice::ReadOnly);
  quint32 i = 1;
  do {
    gemffile_t gf;
    gf.filename = partfile;
    gf.size = f.size();
    f.close();
    files << gf;
    partfile = filename + "-" + QString::number(i);
    i++;
    f.setFileName(partfile);
  } while (f.open(QIODevice::ReadOnly));
  isActivated = true;
}

void CMapGEMF::draw(IDrawContext::buffer_t& buf) {
  if (map->needsRedraw()) {
    return;
  }
  QPointF bufferScale = buf.scale * buf.zoomFactor;
  if (isOutOfScale(bufferScale)) {
    return;
  }
  QPointF pp = buf.ref1;
  map->convertRad2Px(pp);

  // start to draw the map
  QPainter p(&buf.image);
  USE_ANTI_ALIASING(p, true);
  p.setOpacity(getOpacity() / 100.0);
  p.translate(-pp);

  qreal x1 = qMin(buf.ref1.x(), buf.ref4.x());
  qreal y1 = qMax(buf.ref1.y(), buf.ref2.y());

  qreal x2 = qMax(buf.ref2.x(), buf.ref3.x());
  qreal y2 = qMin(buf.ref3.y(), buf.ref4.y());

  if (x1 < -180.0 * DEG_TO_RAD) {
    x1 = -180 * DEG_TO_RAD;
  }
  if (x2 > 180.0 * DEG_TO_RAD) {
    x2 = 180 * DEG_TO_RAD;
  }

  QPointF s1 = buf.scale * buf.zoomFactor;
  qreal d = NOFLOAT;
  quint32 z = MAX_ZOOM_LEVEL;

  for (quint32 i = 0; i < MAX_ZOOM_LEVEL; i++) {
    qreal s2 = 0.055 * (1 << i);
    if (qAbs(s2 - s1.x()) < d) {
      z = i;
      d = qAbs(s2 - s1.x());
    }
  }

  z = MAX_ZOOM_LEVEL - z;

  qint32 col1 = lon2tile(x1 * RAD_TO_DEG, z) / 256;
  qint32 col2 = lon2tile(x2 * RAD_TO_DEG, z) / 256;
  qint32 row1 = lat2tile(y1 * RAD_TO_DEG, z) / 256;
  qint32 row2 = lat2tile(y2 * RAD_TO_DEG, z) / 256;
  for (qint32 row = row1; row <= row2; row++) {
    for (qint32 col = col1; col <= col2; col++) {
      qreal xx1 = tile2lon(col, z) * DEG_TO_RAD;
      qreal yy1 = tile2lat(row, z) * DEG_TO_RAD;
      qreal xx2 = tile2lon(col + 1, z) * DEG_TO_RAD;
      qreal yy2 = tile2lat(row + 1, z) * DEG_TO_RAD;

      QPolygonF l;
      l << QPointF(xx1, yy1) << QPointF(xx2, yy1) << QPointF(xx2, yy2) << QPointF(xx1, yy2);

      const QImage& img = getTile(col, row, z);
      drawTile(img, l, p);
    }
  }
}

quint64 CMapGEMF::getFilenameFromAddress(const quint64 offset, QString& filename) {
  quint64 temp = offset;

  for (const gemffile_t& gf : std::as_const(files)) {
    if (temp < gf.size) {
      filename = gf.filename;

      return temp;
    }
    temp -= gf.size;
  }

  qDebug() << "CMAPGemf: ImageAddress was wrong " << offset;
  return 0;
}

QImage CMapGEMF::getTile(const quint32 row, const quint32 col, const quint32 z) {
  if (!rangesByZoom.contains(z)) {
    qDebug() << "CMapGEMF: getTile called for a zoomlevel not available";
    return QImage();
  }
  QList<range_t> ranges = rangesByZoom[z];

  for (const range_t& range : ranges) {
    if (row >= range.minX && row <= range.maxX && col >= range.minY && col <= range.maxY) {
      const quint32 Xidx = row - range.minX;
      const quint32 Yidx = col - range.minY;
      const quint32 nrYVals = range.maxY + 1 - range.minY;
      const quint32 TileIdx = Xidx * nrYVals + Yidx;
      const quint64 offsetRange = TileIdx * 12;  // 4 + 8

      quint64 offsetGEMF = offsetRange + range.offset;
      QString splitfile;
      offsetGEMF = getFilenameFromAddress(offsetGEMF, splitfile);

      QFile file(splitfile);
      file.open(QIODevice::ReadOnly);
      QDataStream dataFile(&file);

      dataFile.skipRawData(offsetGEMF);

      quint64 imageDataAddress;
      dataFile >> imageDataAddress;

      quint32 size;
      dataFile >> size;

      file.close();

      quint64 imageDataOffset = getFilenameFromAddress(imageDataAddress, splitfile);
      QFile imageFile(splitfile);
      imageFile.open(QIODevice::ReadOnly);
      imageFile.seek(imageDataOffset);
      QByteArray imageData(size, 0);
      imageFile.read(imageData.data(), size);
      return QImage::fromData((uchar*)imageData.data(), size, 0);
    }
  }

  return QImage();
}
