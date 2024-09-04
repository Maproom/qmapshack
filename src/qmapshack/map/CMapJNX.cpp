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

#include "map/CMapJNX.h"

#include <QtCore5Compat/QTextCodec>
#include <QtGui>

#include "helpers/CDraw.h"
#include "map/CMapDraw.h"
#include "units/IUnit.h"

static void readCString(QDataStream& stream, QByteArray& ba) {
  quint8 byte;

  ba.clear();

  stream >> byte;
  while (byte != 0) {
    ba += byte;
    stream >> byte;
  }
}

static quint32 scale2jnx(qreal scale) {
  /*
     Ok, I've made some calculations, and got the following formula to
     calculate the JNX scale (S) depending on the map's meters/pixel
     ratio (R):

     S(R) =
      qRound(
        76437 *
        exp(
          ln(2.000032708011) *
          qRound(
            ln(R * 130.2084 / 76437) /
            ln(2.000032708011)
          )
        )
      )


     where
     qRound - is a function which returns the closest integer from
      floating point value, [unfortunately its defined in C99 but not standard C++]
     exp - exponent,
     ln - natural logarithm.

     Magic number 130.2084 - is an average value for
     (JNX scale) / (maps meters per pixel)
     ratio among all zoom levels in metric system.

     Magic number 2.000032708011 is a ratio on which our standard scale
     table is built. It is (76437 / 4777) ^ (1/4).
   */

  return (quint32)qFloor(
      0.5 + 76437 * exp(log(2.000032708011) * qFloor(0.5 + log(scale * 10 * 130.2084 / 76437) / log(2.000032708011))));
}

CMapJNX::CMapJNX(const QString& filename, CMapDraw* parent) : IMap(eFeatVisibility, parent), filename(filename) {
  qDebug() << "------------------------------";
  qDebug() << "JNX: try to open" << filename;

  qint32 productId = -1;
  readFile(filename, productId);

  proj.init("EPSG:3857", "EPSG:4326");

  isActivated = true;
}

void CMapJNX::readFile(const QString& fn, qint32& productId) {
  hdr_t hdr;

  qDebug() << fn;

  QFile file(fn);
  file.open(QIODevice::ReadOnly);

  QDataStream stream(&file);
  stream.setByteOrder(QDataStream::LittleEndian);

  stream >> hdr.version;    // byte 00000000..00000003
  stream >> hdr.devid;      // byte 00000004..00000007
  stream >> hdr.lat1;       // byte 00000008..0000000B
  stream >> hdr.lon2;       // byte 0000000C..0000000F
  stream >> hdr.lat2;       // byte 00000010..00000013
  stream >> hdr.lon1;       // byte 00000014..00000017
  stream >> hdr.details;    // byte 00000018..0000001B
  stream >> hdr.expire;     // byte 0000001C..0000001F
  stream >> hdr.productId;  // byte 00000020..00000023
  stream >> hdr.crc;        // byte 00000024..00000027
  stream >> hdr.signature;  // byte 00000028..0000002B
                            // byte 0000002C..0000002F
  stream >> hdr.signature_offset;

  if (hdr.version > 3) {
    stream >> hdr.zorder;
  } else {
    hdr.zorder = -1;
  }

  if (productId != -1 && hdr.productId != productId) {
    return;
  }

  productId = hdr.productId;

  files.append(file_t());
  file_t& mapFile = files.last();

  mapFile.filename = fn;

  mapFile.lat1 = hdr.lat1 * 180.0 / 0x7FFFFFFF;
  mapFile.lat2 = hdr.lat2 * 180.0 / 0x7FFFFFFF;
  mapFile.lon1 = hdr.lon1 * 180.0 / 0x7FFFFFFF;
  mapFile.lon2 = hdr.lon2 * 180.0 / 0x7FFFFFFF;
  mapFile.bbox = QRectF(QPointF(mapFile.lon1, mapFile.lat1), QPointF(mapFile.lon2, mapFile.lat2));

  qDebug() << Qt::hex << "Version:" << hdr.version << "DevId" << hdr.devid;
  qDebug() << mapFile.lon1 << mapFile.lat1 << mapFile.lon2 << mapFile.lat2;
  qDebug() << Qt::hex << hdr.lon1 << hdr.lat1 << hdr.lon2 << hdr.lat2;
  qDebug() << Qt::hex << "Details:" << hdr.details << "Expire:" << hdr.expire << "CRC:" << hdr.crc;
  qDebug() << Qt::hex << "Signature:" << hdr.signature << "Offset:" << hdr.signature_offset;

  QString strTopLeft, strBottomRight;
  IUnit::degToStr(mapFile.lon1, mapFile.lat1, strTopLeft);
  IUnit::degToStr(mapFile.lon2, mapFile.lat2, strBottomRight);

  qDebug() << "Levels:";
  mapFile.levels.resize(hdr.details);
  for (quint32 i = 0; i < hdr.details; i++) {
    level_t& level = mapFile.levels[i];
    stream >> level.nTiles >> level.offset >> level.scale;

    if (hdr.version > 3) {
      quint32 dummy;
      QTextCodec* codec = QTextCodec::codecForName("utf-8");
      QByteArray ba;

      stream >> dummy;
      readCString(stream, ba);
      level.copyright1 = codec->toUnicode(ba);
      copyright += level.copyright1 + "\n";
    }
    qDebug() << i << Qt::hex << level.nTiles << level.offset << level.scale;
  }

  quint32 infoBlockVersion;
  stream >> infoBlockVersion;
  if (infoBlockVersion == 0x9) {
    QTextCodec* codec = QTextCodec::codecForName("utf-8");
    QByteArray ba;
    quint8 dummy;
    QString groupId;
    QString groupName;
    QString groupTitle;

    readCString(stream, ba);
    groupId = codec->toUnicode(ba);
    readCString(stream, ba);
    groupName = codec->toUnicode(ba);

    stream >> dummy >> dummy >> dummy;
    readCString(stream, ba);
    groupTitle = codec->toUnicode(ba);
    qDebug() << groupId << groupName << groupTitle;

    for (quint32 i = 0; i < hdr.details; i++) {
      level_t& level = mapFile.levels[i];

      stream >> level.level;
      readCString(stream, ba);
      level.name1 = codec->toUnicode(ba);
      readCString(stream, ba);
      level.name2 = codec->toUnicode(ba);
      readCString(stream, ba);
      level.copyright2 = codec->toUnicode(ba);
      copyright += level.copyright2 + "\n";
    }
  }

  for (quint32 i = 0; i < hdr.details; i++) {
    level_t& level = mapFile.levels[i];
    const quint32 M = level.nTiles;
    file.seek(level.offset);

    level.tiles.resize(M);

    for (quint32 m = 0; m < M; m++) {
      qint32 top, right, bottom, left;
      tile_t& tile = level.tiles[m];

      stream >> top >> right >> bottom >> left;
      stream >> tile.width >> tile.height >> tile.size >> tile.offset;

      tile.area.setTop(top * 180.0 / 0x7FFFFFFF);
      tile.area.setRight(right * 180.0 / 0x7FFFFFFF);
      tile.area.setBottom(bottom * 180.0 / 0x7FFFFFFF);
      tile.area.setLeft(left * 180.0 / 0x7FFFFFFF);
    }
  }

  if (mapFile.lon1 < lon1) {
    lon1 = mapFile.lon1;
  }
  if (mapFile.lat1 > lat1) {
    lat1 = mapFile.lat1;
  }
  if (mapFile.lon2 > lon2) {
    lon2 = mapFile.lon2;
  }
  if (mapFile.lat2 < lat2) {
    lat2 = mapFile.lat2;
  }
}

qint32 CMapJNX::scale2level(qreal s, const file_t& file) {
  qint32 idxLvl = NOIDX;
  quint32 actScale = scale2jnx(s);

  for (int i = 0; i < file.levels.size(); i++) {
    const level_t& level = file.levels[i];

    if (actScale <= level.scale) {
      idxLvl = i;
    }
  }

  return idxLvl;
}

void CMapJNX::draw(IDrawContext::buffer_t& buf) /* override */
{
  if (map->needsRedraw()) {
    return;
  }

  // convert top left buffer corner
  // into buffer's coordinate system
  QPointF pp = buf.ref1;
  map->convertRad2Px(pp);

  QPointF bufferScale = buf.scale * buf.zoomFactor;

  qreal u1 = buf.ref1.x() * RAD_TO_DEG;
  qreal v1 = buf.ref1.y() * RAD_TO_DEG;
  qreal u2 = buf.ref3.x() * RAD_TO_DEG;
  qreal v2 = buf.ref3.y() * RAD_TO_DEG;

  QRectF viewport;
  viewport.setTop(v2);
  viewport.setRight(u2);
  viewport.setBottom(v1);
  viewport.setLeft(u1);

  // ----- start drawing -----
  QPainter p(&buf.image);
  USE_ANTI_ALIASING(p, true);
  p.setOpacity(getOpacity() / 100.0);
  p.translate(-pp);

  for (const file_t& mapFile : std::as_const(files)) {
    if (!viewport.intersects(mapFile.bbox)) {
      continue;
    }

    if (map->needsRedraw()) {
      break;
    }

    qint32 level = scale2level(bufferScale.x() / 5, mapFile);

    // no scalable level found, draw bounding box of map
    // derive maps corner coordinate
    QPolygonF l(4);
    l[0].rx() = mapFile.lon1 * DEG_TO_RAD;
    l[0].ry() = mapFile.lat1 * DEG_TO_RAD;
    l[1].rx() = mapFile.lon2 * DEG_TO_RAD;
    l[1].ry() = mapFile.lat1 * DEG_TO_RAD;
    l[2].rx() = mapFile.lon2 * DEG_TO_RAD;
    l[2].ry() = mapFile.lat2 * DEG_TO_RAD;
    l[3].rx() = mapFile.lon1 * DEG_TO_RAD;
    l[3].ry() = mapFile.lat2 * DEG_TO_RAD;

    map->convertRad2Px(l);

    // finally scale, rotate and draw tile
    p.setPen(Qt::black);
    p.setBrush(Qt::NoBrush);
    p.drawPolygon(l);

    if (level < 0) {
      continue;
    }
    if (isOutOfScale(bufferScale)) {
      continue;
    }

    QByteArray data(1024 * 1024 * 4, 0);
    //(char) typecast needed to avoid MSVC compiler warning
    // in MSVC, char is a signed type.
    // Maybe the QByteArray declaration should be fixed ;-)
    data[0] = (char)0xFF;
    data[1] = (char)0xD8;
    char* pData = data.data() + 2;

    QFile file(mapFile.filename);
    file.open(QIODevice::ReadOnly);

    const QVector<tile_t>& tiles = mapFile.levels[level].tiles;
    const quint32 M = tiles.size();
    for (quint32 m = 0; m < M; m++) {
      if (map->needsRedraw()) {
        break;
      }

      const tile_t& tile = tiles[m];

      if (viewport.intersects(tile.area)) {
        QImage img;
        file.seek(tile.offset);
        file.read(pData, tile.size);
        img.loadFromData(data);

        QPolygonF l(4);
        l[0].rx() = tile.area.left() * DEG_TO_RAD;
        l[0].ry() = tile.area.top() * DEG_TO_RAD;
        l[1].rx() = tile.area.right() * DEG_TO_RAD;
        l[1].ry() = tile.area.top() * DEG_TO_RAD;
        l[2].rx() = tile.area.right() * DEG_TO_RAD;
        l[2].ry() = tile.area.bottom() * DEG_TO_RAD;
        l[3].rx() = tile.area.left() * DEG_TO_RAD;
        l[3].ry() = tile.area.bottom() * DEG_TO_RAD;

        drawTile(img, l, p);
      }
    }
  }
}
