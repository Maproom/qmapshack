/**********************************************************************************************
    Copyright (C) 2009 Oliver Eichler <oliver.eichler@gmx.de>

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

    NOTE:   this code is based on the opensource typ file generator at
            http://ati.land.cz/gps/typdecomp/editor.cgi

**********************************************************************************************/

#include "map/garmin/CGarminTyp.h"

#include <qtextcodec.h>
#include <string.h>

#include <QMessageBox>
#include <QtCore5Compat>
#include <QtCore>

#include "CMainWindow.h"
#include "units/IUnit.h"

#undef DBG

bool CGarminTyp::decode(const QByteArray& array, QMap<quint32, polygon_property>& polygons,
                        QMap<quint32, polyline_property>& polylines, QList<quint32>& drawOrder,
                        QMap<quint32, point_property>& points) {
  QDataStream in(array);
  in.setVersion(QDataStream::Qt_4_5);
  in.setByteOrder(QDataStream::LittleEndian);

  /* Read typ file descriptor */
  quint16 descriptor;
  in >> descriptor;

  qDebug() << "descriptor" << Qt::hex << descriptor;

  if (!parseHeader(in)) {
    return false;
  }

  if (!parseDrawOrder(in, drawOrder)) {
    return false;
  }

  if (!parsePolygon(in, polygons)) {
    return false;
  }

  if (!parsePolyline(in, polylines)) {
    return false;
  }

  if (!parsePoint(in, points)) {
    return false;
  }

  return true;
}

QTextCodec* CGarminTyp::getCodec(quint16 codepage) {
  QTextCodec* codec = QTextCodec::codecForName(QString("CP%1").arg(codepage).toLatin1());
  if (codepage == 65001) {
    codec = QTextCodec::codecForName("UTF-8");
  }

  return codec;
}

bool CGarminTyp::parseHeader(QDataStream& in) {
  int i;
  QString garmintyp;
  quint8 byte;

  for (i = 0; i < 10; ++i) {
    in >> byte;
    garmintyp.append(QChar(byte));
  }
  garmintyp.append(0);
  if (garmintyp != "GARMIN TYP") {
    qDebug() << "CMapTDB::readTYP() not a known typ file";
    return false;
  }

  /* reading typ creation date string */

  in.device()->seek(0x0c);
  in >> version >> year >> month >> day >> hour >> minutes >> seconds >> codepage;
  month -= 1; /* Month are like Microsoft starting 0 ? */
  year += 1900;

  /* Reading points / lines / polygons struct */
  in >> sectPoints.dataOffset >> sectPoints.dataLength;
  in >> sectPolylines.dataOffset >> sectPolylines.dataLength;
  in >> sectPolygons.dataOffset >> sectPolygons.dataLength;

  in >> pid >> fid;

  /* Read Array datas */
  in >> sectPoints.arrayOffset >> sectPoints.arrayModulo >> sectPoints.arraySize;
  in >> sectPolylines.arrayOffset >> sectPolylines.arrayModulo >> sectPolylines.arraySize;
  in >> sectPolygons.arrayOffset >> sectPolygons.arrayModulo >> sectPolygons.arraySize;
  in >> sectOrder.arrayOffset >> sectOrder.arrayModulo >> sectOrder.arraySize;

#ifdef DBG
  qDebug() << "Version:" << version << "Codepage:" << codepage;
  qDebug() << "PID" << Qt::hex << pid << "FID" << Qt::hex << fid;
  qDebug() << "Points     doff/dlen/aoff/amod/asize:" << Qt::hex << "\t" << sectPoints.dataOffset << "\t"
           << sectPoints.dataLength << "\t" << sectPoints.arrayOffset << "\t" << sectPoints.arrayModulo << "\t"
           << sectPoints.arrayOffset;
  qDebug() << "Polylines  doff/dlen/aoff/amod/asize:" << Qt::hex << "\t" << sectPolylines.dataOffset << "\t"
           << sectPolylines.dataLength << "\t" << sectPolylines.arrayOffset << "\t" << sectPolylines.arrayModulo << "\t"
           << sectPolylines.arrayOffset;
  qDebug() << "Polygons   doff/dlen/aoff/amod/asize:" << Qt::hex << "\t" << sectPolygons.dataOffset << "\t"
           << sectPolygons.dataLength << "\t" << sectPolygons.arrayOffset << "\t" << sectPolygons.arrayModulo << "\t"
           << sectPolygons.arrayOffset;
  qDebug() << "Order      doff/dlen/aoff/amod/asize:" << Qt::hex << "\t" << sectOrder.dataOffset << "\t"
           << sectOrder.dataLength << "\t" << sectOrder.arrayOffset << "\t" << sectOrder.arrayModulo << "\t"
           << sectOrder.arrayOffset;
#endif

  return true;
}

bool CGarminTyp::parseDrawOrder(QDataStream& in, QList<quint32>& drawOrder) {
  if (sectOrder.arraySize == 0) {
    return true;
  }

  if (sectOrder.arrayModulo != 5) {
    return false;
  }

  if ((sectOrder.arraySize % sectOrder.arrayModulo) != 0) {
    return true;
  }

  in.device()->seek(sectOrder.arrayOffset);

  int i, n;
  quint8 typ;
  quint32 subtyp;

  const int N = sectOrder.arraySize / sectOrder.arrayModulo;

  for (i = 0; i < N; i++) {
    in >> typ >> subtyp;
    //         qDebug() << Qt::hex << typ << subtyp;
    if (typ == 0) {
    } else if (subtyp == 0) {
#ifdef DBG
      qDebug() << QString("Type 0x%1 is priority %2").arg(typ, 0, 16).arg(count);
#endif
      int idx = drawOrder.indexOf(typ);
      if (idx != NOIDX) {
        drawOrder.move(idx, 0);
      }
    } else {
      quint32 exttyp = 0x010000 | (typ << 8);
      quint32 mask = 0x1;

      for (n = 0; n < 0x20; ++n) {
        if (subtyp & mask) {
          drawOrder.push_front(exttyp | n);
#ifdef DBG
          qDebug() << QString("Type 0x%1 is priority %2").arg(exttyp | n, 0, 16).arg(count);
#endif
        }
        mask = mask << 1;
      }
    }
  }

#ifdef DBG
  for (unsigned i = 0; i < drawOrder.size(); ++i) {
    if (i && i % 16 == 0) {
      printf(" \n");
    }
    printf("%06X ", drawOrder[i]);
  }

  printf(" \n");
#endif

  return true;
}

bool CGarminTyp::parsePolygon(QDataStream& in, QMap<quint32, polygon_property>& polygons) {
  bool tainted = false;

  if (sectPolygons.arraySize == 0) {
    return true;
  }

  if (!sectPolygons.arrayModulo || ((sectPolygons.arraySize % sectPolygons.arrayModulo) != 0)) {
    return true;
  }

  QTextCodec* codec = getCodec(codepage);

  const int N = sectPolygons.arraySize / sectPolygons.arrayModulo;
  for (int element = 0; element < N; element++) {
    quint16 t16_1 = 0, t16_2, subtyp;
    quint8 t8;
    quint32 typ, offset = 0;
    bool hasLocalization = false;
    bool hasTextColor = false;
    quint8 ctyp;
    QImage xpmDay(32, 32, QImage::Format_Indexed8);
    QImage xpmNight(32, 32, QImage::Format_Indexed8);
    quint8 r, g, b;
    quint8 langcode;

    in.device()->seek(sectPolygons.arrayOffset + (sectPolygons.arrayModulo * element));

    if (sectPolygons.arrayModulo == 5) {
      in >> t16_1 >> t16_2 >> t8;
      offset = t16_2 | (t8 << 16);
    } else if (sectPolygons.arrayModulo == 4) {
      in >> t16_1 >> t16_2;
      offset = t16_2;
    } else if (sectPolygons.arrayModulo == 3) {
      in >> t16_1 >> t8;
      offset = t8;
    }

    t16_2 = (t16_1 >> 5) | ((t16_1 & 0x1f) << 11);
    typ = t16_2 & 0x7F;
    subtyp = t16_1 & 0x1F;

    if (t16_1 & 0x2000) {
      typ = 0x10000 | (typ << 8) | subtyp;
    }

    in.device()->seek(sectPolygons.dataOffset + offset);
    in >> t8;
    hasLocalization = t8 & 0x10;
    hasTextColor = t8 & 0x20;
    ctyp = t8 & 0x0F;

#ifdef DBG
    qDebug() << "Polygon typ:" << Qt::hex << typ << "ctype:" << ctyp << "offset:" << (sectPolygons.dataOffset + offset)
             << "orig data:" << t16_1;
#endif

    polygon_property& property = polygons[typ];

    switch (ctyp) {
      case 0x01: {
        // day & night single color
        in >> b >> g >> r;
        property.brushDay = QBrush(qRgb(r, g, b));
        in >> b >> g >> r;
        property.brushNight = QBrush(qRgb(r, g, b));

        // night and day color for line?
        in >> b >> g >> r;
        property.pen = QPen(QBrush(qRgb(r, g, b)), 2);
        in >> b >> g >> r;
        property.known = true;

        break;
      }

      case 0x06: {
        // day & night single color
        in >> b >> g >> r;
        property.brushDay = QBrush(qRgb(r, g, b));
        property.brushNight = QBrush(qRgb(r, g, b));
        property.pen = Qt::NoPen;
        property.known = true;

        break;
      }

      case 0x07: {
        // day single color & night single color
        in >> b >> g >> r;
        property.brushDay = QBrush(qRgb(r, g, b));
        in >> b >> g >> r;
        property.brushNight = QBrush(qRgb(r, g, b));
        property.pen = Qt::NoPen;
        property.known = true;

        break;
      }

      case 0x08: {
        // day & night two color
        xpmDay.setColorCount(2);

        in >> b >> g >> r;
        xpmDay.setColor(1, qRgb(r, g, b));
        in >> b >> g >> r;
        xpmDay.setColor(0, qRgb(r, g, b));

        decodeBitmap(in, xpmDay, 32, 32, 1);
        property.brushDay.setTextureImage(xpmDay);
        property.brushNight.setTextureImage(xpmDay);
        property.pen = Qt::NoPen;
        property.known = true;
        break;
      }

      case 0x09: {
        // day two color & night two color
        xpmDay.setColorCount(2);
        xpmNight.setColorCount(2);
        in >> b >> g >> r;
        xpmDay.setColor(1, qRgb(r, g, b));
        in >> b >> g >> r;
        xpmDay.setColor(0, qRgb(r, g, b));
        in >> b >> g >> r;
        xpmNight.setColor(1, qRgb(r, g, b));
        in >> b >> g >> r;
        xpmNight.setColor(0, qRgb(r, g, b));

        decodeBitmap(in, xpmDay, 32, 32, 1);
        memcpy(xpmNight.bits(), xpmDay.bits(), (32 * 32));
        property.brushDay.setTextureImage(xpmDay);
        property.brushNight.setTextureImage(xpmNight);
        property.pen = Qt::NoPen;
        property.known = true;

        break;
      }

      case 0x0B: {
        // day one color, transparent & night two color
        xpmDay.setColorCount(2);
        xpmNight.setColorCount(2);
        in >> b >> g >> r;
        xpmDay.setColor(1, qRgb(r, g, b));
        xpmDay.setColor(0, qRgba(255, 255, 255, 0));

        in >> b >> g >> r;
        xpmNight.setColor(1, qRgb(r, g, b));
        in >> b >> g >> r;
        xpmNight.setColor(0, qRgb(r, g, b));

        decodeBitmap(in, xpmDay, 32, 32, 1);
        memcpy(xpmNight.bits(), xpmDay.bits(), (32 * 32));
        property.brushDay.setTextureImage(xpmDay);
        property.brushNight.setTextureImage(xpmNight);
        property.pen = Qt::NoPen;
        property.known = true;
        break;
      }

      case 0x0D: {
        // day two color & night one color, transparent

        xpmDay.setColorCount(2);
        xpmNight.setColorCount(2);
        in >> b >> g >> r;
        xpmDay.setColor(1, qRgb(r, g, b));
        in >> b >> g >> r;
        xpmDay.setColor(0, qRgb(r, g, b));

        in >> b >> g >> r;
        xpmNight.setColor(1, qRgb(r, g, b));
        xpmNight.setColor(0, qRgba(255, 255, 255, 0));

        decodeBitmap(in, xpmDay, 32, 32, 1);
        memcpy(xpmNight.bits(), xpmDay.bits(), (32 * 32));
        property.brushDay.setTextureImage(xpmDay);
        property.brushNight.setTextureImage(xpmNight);
        property.pen = Qt::NoPen;
        property.known = true;

        break;
      }

      case 0x0E: {
        // day & night one color, transparent
        xpmDay.setColorCount(2);
        in >> b >> g >> r;
        xpmDay.setColor(1, qRgb(r, g, b));
        xpmDay.setColor(0, qRgba(255, 255, 255, 0));

        decodeBitmap(in, xpmDay, 32, 32, 1);
        property.brushDay.setTextureImage(xpmDay);
        property.brushNight.setTextureImage(xpmDay);
        property.pen = Qt::NoPen;
        property.known = true;

        break;
      }

      case 0x0F: {
        // day one color, transparent & night one color, transparent
        xpmDay.setColorCount(2);
        xpmNight.setColorCount(2);
        in >> b >> g >> r;
        xpmDay.setColor(1, qRgb(r, g, b));
        xpmDay.setColor(0, qRgba(255, 255, 255, 0));

        in >> b >> g >> r;
        xpmNight.setColor(1, qRgb(r, g, b));
        xpmNight.setColor(0, qRgba(255, 255, 255, 0));

        decodeBitmap(in, xpmDay, 32, 32, 1);
        memcpy(xpmNight.bits(), xpmDay.bits(), (32 * 32));
        property.brushDay.setTextureImage(xpmDay);
        property.brushNight.setTextureImage(xpmNight);
        property.pen = Qt::NoPen;
        property.known = true;

        break;
      }

      default:
        if (!tainted) {
          QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Warning..."),
                               tr("This is a typ file with unknown polygon encoding. Please report!"),
                               QMessageBox::Abort, QMessageBox::Abort);
          tainted = true;
        }
        qDebug() << "Failed polygon:" << typ << subtyp << Qt::hex << typ << subtyp << ctyp;
    }

    if (hasLocalization) {
      qint16 len;
      quint8 n = 1;

      in >> t8;
      len = t8;

      if (!(t8 & 0x01)) {
        n = 2;
        in >> t8;
        len |= t8 << 8;
      }

      len -= n;
      while (len > 0) {
        QByteArray str;
        in >> langcode;
        languages << langcode;
        len -= 2 * n;
        while (len > 0) {
          in >> t8;
          len -= 2 * n;

          if (t8 == 0) {
            break;
          }

          str += t8;
        }
        if (codec != nullptr) {
          property.strings[langcode] = codec->toUnicode(str);
        }
#ifdef DBG
        qDebug() << len << langcode << property.strings[langcode];
#endif
      }
    }

    if (hasTextColor) {
      in >> t8;
      property.labelType = (label_type_e)(t8 & 0x07);

      if (t8 & 0x08) {
        in >> b >> g >> r;
        property.colorLabelDay = qRgb(r, g, b);
      }

      if (t8 & 0x10) {
        in >> b >> g >> r;
        property.colorLabelNight = qRgb(r, g, b);
      }
#ifdef DBG
      qDebug() << "ext. label: type" << property.labelType << "day" << property.colorLabelDay << "night"
               << property.colorLabelNight;
#endif
    }
  }

  return true;
}

bool CGarminTyp::parsePolyline(QDataStream& in, QMap<quint32, polyline_property>& polylines) {
  bool tainted = false;

  if (sectPolylines.arraySize == 0) {
    return true;
  }

  if (!sectPolylines.arrayModulo || ((sectPolylines.arraySize % sectPolylines.arrayModulo) != 0)) {
    return true;
  }

  QTextCodec* codec = getCodec(codepage);

  const int N = sectPolylines.arraySize / sectPolylines.arrayModulo;
  for (int element = 0; element < N; element++) {
    quint16 t16_1 = 0, t16_2, subtyp;
    quint8 t8_1, t8_2;
    quint32 typ, offset = 0;
    bool hasLocalization = false;
    bool hasTextColor = false;
    // bool renderMode = false;
    quint8 ctyp, rows;
    quint8 r, g, b;
    quint8 langcode;

    in.device()->seek(sectPolylines.arrayOffset + (sectPolylines.arrayModulo * element));

    if (sectPolylines.arrayModulo == 5) {
      in >> t16_1 >> t16_2 >> t8_1;
      offset = t16_2 | (t8_1 << 16);
    } else if (sectPolylines.arrayModulo == 4) {
      in >> t16_1 >> t16_2;
      offset = t16_2;
    } else if (sectPolylines.arrayModulo == 3) {
      in >> t16_1 >> t8_1;
      offset = t8_1;
    }

    t16_2 = (t16_1 >> 5) | ((t16_1 & 0x1f) << 11);
    typ = t16_2 & 0x7F;
    subtyp = t16_1 & 0x1F;

    if (t16_1 & 0x2000) {
      typ = 0x10000 | (typ << 8) | subtyp;
    }

    in.device()->seek(sectPolylines.dataOffset + offset);
    in >> t8_1 >> t8_2;
    ctyp = t8_1 & 0x07;
    rows = t8_1 >> 3;

    hasLocalization = t8_2 & 0x01;
    // renderMode      = t8_2 & 0x02;
    hasTextColor = t8_2 & 0x04;

#ifdef DBG
    qDebug() << "Polyline typ:" << Qt::hex << typ << "ctyp:" << ctyp << "offset:" << (sectPolylines.dataOffset + offset)
             << "orig data:" << t16_1;
#endif

    polyline_property& property = polylines[typ];
#ifdef DBG
    qDebug() << "rows" << rows << "t8_2" << Qt::hex << t8_2;
#endif

    switch (ctyp) {
      case 0x00: {
        if (rows) {
          QImage xpm(32, rows, QImage::Format_Indexed8);
          in >> b >> g >> r;
          xpm.setColor(1, qRgb(r, g, b));
          in >> b >> g >> r;
          xpm.setColor(0, qRgb(r, g, b));
          decodeBitmap(in, xpm, 32, rows, 1);
          property.imgDay = xpm;
          property.imgNight = xpm;
          property.hasPixmap = true;
          property.known = true;
        } else {
          quint8 w1, w2;
          in >> b >> g >> r;
          property.penLineDay = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          property.penLineNight = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          in >> b >> g >> r;
          property.penBorderDay = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          property.penBorderNight = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          in >> w1 >> w2;
          property.penLineDay.setWidth(w1);
          property.penLineNight.setWidth(w1);
          property.penBorderDay.setWidth(w2);
          property.penBorderNight.setWidth(w2);
          property.hasBorder = w2 > w1;
          property.hasPixmap = false;
          property.known = true;
        }

        break;
      }

      case 0x01: {
        if (rows) {
          QImage xpm1(32, rows, QImage::Format_Indexed8);
          QImage xpm2(32, rows, QImage::Format_Indexed8);
          in >> b >> g >> r;
          xpm1.setColor(1, qRgb(r, g, b));
          in >> b >> g >> r;
          xpm1.setColor(0, qRgb(r, g, b));
          in >> b >> g >> r;
          xpm2.setColor(1, qRgb(r, g, b));
          in >> b >> g >> r;
          xpm2.setColor(0, qRgb(r, g, b));
          decodeBitmap(in, xpm1, 32, rows, 1);
          memcpy(xpm2.bits(), xpm1.bits(), (32 * rows));
          property.imgDay = xpm1;
          property.imgNight = xpm2;
          property.hasPixmap = true;
          property.known = true;
        } else {
          quint8 w1, w2;
          in >> b >> g >> r;
          property.penLineDay = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          in >> b >> g >> r;
          property.penBorderDay = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          in >> b >> g >> r;
          property.penLineNight = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          in >> b >> g >> r;
          property.penBorderNight = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          in >> w1 >> w2;
          property.penLineDay.setWidth(w1);
          property.penLineNight.setWidth(w1);
          property.penBorderDay.setWidth(w2);
          property.penBorderNight.setWidth(w2);
          property.hasBorder = w2 > w1;
          property.hasPixmap = false;
          property.known = true;
        }
        break;
      }

      case 0x03: {
        if (rows) {
          QImage xpm1(32, rows, QImage::Format_Indexed8);
          QImage xpm2(32, rows, QImage::Format_Indexed8);
          in >> b >> g >> r;
          xpm1.setColor(1, qRgb(r, g, b));
          xpm1.setColor(0, qRgba(255, 255, 255, 0));
          in >> b >> g >> r;
          xpm2.setColor(1, qRgb(r, g, b));
          in >> b >> g >> r;
          xpm2.setColor(0, qRgb(r, g, b));
          decodeBitmap(in, xpm1, 32, rows, 1);
          memcpy(xpm2.bits(), xpm1.bits(), (32 * rows));
          property.imgDay = xpm1;
          property.imgNight = xpm2;
          property.hasPixmap = true;
          property.known = true;
        } else {
          quint8 w1, w2;
          in >> b >> g >> r;
          property.penLineDay = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          property.penBorderDay = QPen(Qt::NoPen);
          in >> b >> g >> r;
          property.penLineNight = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          in >> b >> g >> r;
          property.penBorderNight = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          in >> w1 >> w2;
          property.penLineDay.setWidth(w1);
          property.penLineNight.setWidth(w1);
          property.penBorderDay.setWidth(w2);
          property.penBorderNight.setWidth(w2);
          property.hasBorder = w2 > w1;
          property.hasPixmap = false;
          property.known = true;
        }

        break;
      }

      case 0x05: {
        if (rows) {
          QImage xpm1(32, rows, QImage::Format_Indexed8);
          QImage xpm2(32, rows, QImage::Format_Indexed8);
          in >> b >> g >> r;
          xpm1.setColor(1, qRgb(r, g, b));
          in >> b >> g >> r;
          xpm1.setColor(0, qRgb(r, g, b));
          in >> b >> g >> r;
          xpm2.setColor(1, qRgb(r, g, b));
          xpm2.setColor(0, qRgba(255, 255, 255, 0));
          decodeBitmap(in, xpm1, 32, rows, 1);
          memcpy(xpm2.bits(), xpm1.bits(), (32 * rows));
          property.imgDay = xpm1;
          property.imgNight = xpm2;
          property.hasPixmap = true;
          property.known = true;
        } else {
          quint8 w1;
          in >> b >> g >> r;
          property.penLineDay = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          in >> b >> g >> r;
          property.penBorderDay = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          in >> b >> g >> r;
          property.penLineNight = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          property.penBorderNight = QPen(Qt::NoPen);
          in >> w1;
          property.penLineDay.setWidth(w1);
          property.penLineNight.setWidth(w1);
          property.hasBorder = false;
          property.hasPixmap = false;
          property.known = true;
        }
        break;
      }

      case 0x06: {
        if (rows) {
          QImage xpm(32, rows, QImage::Format_Indexed8);
          in >> b >> g >> r;
          xpm.setColor(1, qRgb(r, g, b));
          xpm.setColor(0, qRgba(255, 255, 255, 0));
          decodeBitmap(in, xpm, 32, rows, 1);
          property.imgDay = xpm;
          property.imgNight = xpm;
          property.hasPixmap = true;
          property.known = true;
        } else {
          quint8 w1;
          in >> b >> g >> r;
          property.penLineDay = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          property.penBorderDay = QPen(Qt::NoPen);
          property.penLineNight = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          property.penBorderNight = QPen(Qt::NoPen);
          in >> w1;
          property.penLineDay.setWidth(w1);
          property.penLineNight.setWidth(w1);
          property.hasBorder = false;
          property.hasPixmap = false;
          property.known = true;
        }
        break;
      }

      case 0x07: {
        if (rows) {
          QImage xpm1(32, rows, QImage::Format_Indexed8);
          QImage xpm2(32, rows, QImage::Format_Indexed8);
          in >> b >> g >> r;
          xpm1.setColor(1, qRgb(r, g, b));
          xpm1.setColor(0, qRgba(255, 255, 255, 0));
          in >> b >> g >> r;
          xpm2.setColor(1, qRgb(r, g, b));
          xpm2.setColor(0, qRgba(255, 255, 255, 0));
          decodeBitmap(in, xpm1, 32, rows, 1);
          memcpy(xpm2.bits(), xpm1.bits(), (32 * rows));
          property.imgDay = xpm1;
          property.imgNight = xpm2;
          property.hasPixmap = true;
          property.known = true;
        } else {
          quint8 w1;
          in >> b >> g >> r;
          property.penLineDay = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          property.penBorderDay = QPen(Qt::NoPen);
          in >> b >> g >> r;
          property.penLineNight = QPen(QBrush(qRgb(r, g, b)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
          property.penBorderNight = QPen(Qt::NoPen);
          in >> w1;
          property.penLineDay.setWidth(w1);
          property.penLineNight.setWidth(w1);
          property.hasBorder = false;
          property.hasPixmap = false;
          property.known = true;
        }
        break;
      }

      default:
        if (!tainted) {
          QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Warning..."),
                               tr("This is a typ file with unknown polyline encoding. Please report!"),
                               QMessageBox::Abort, QMessageBox::Abort);
          tainted = true;
        }

        qDebug() << "Failed polyline" << Qt::hex << ":" << typ << ctyp << rows;
        continue;
    }

    property.imgDay = property.imgDay.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    property.imgNight = property.imgNight.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    if (hasLocalization) {
      qint16 len;
      quint8 n = 1;

      in >> t8_1;
      len = t8_1;

      if (!(t8_1 & 0x01)) {
        n = 2;
        in >> t8_1;
        len |= t8_1 << 8;
      }

      len -= n;
      while (len > 0) {
        QByteArray str;
        in >> langcode;
        languages << langcode;
        len -= 2 * n;
        while (len > 0) {
          in >> t8_1;
          len -= 2 * n;

          if (t8_1 == 0) {
            break;
          }

          str += t8_1;
        }
        if (codec != nullptr) {
          property.strings[langcode] = codec->toUnicode(str);
        }
#ifdef DBG
        qDebug() << len << langcode << property.strings[langcode];
#endif
      }
    }

    if (hasTextColor) {
      in >> t8_1;
      property.labelType = (label_type_e)(t8_1 & 0x07);

      if (t8_1 & 0x08) {
        in >> b >> g >> r;
        property.colorLabelDay = qRgb(r, g, b);
      }

      if (t8_1 & 0x10) {
        in >> b >> g >> r;
        property.colorLabelNight = qRgb(r, g, b);
      }
#ifdef DBG
      qDebug() << "ext. label: type" << property.labelType << "day" << property.colorLabelDay << "night"
               << property.colorLabelNight;
#endif
    }

    if (property.hasPixmap) {
      property.imgDay = property.imgDay.mirrored(false, true);
      property.imgNight = property.imgNight.mirrored(false, true);
    }
  }
  return true;
}

bool CGarminTyp::decodeBppAndBytes(int ncolors, int w, int flags, int& bpp, int& bytes) {
  switch (flags) {
    case 0x00: {
      if (ncolors < 3) {
        bpp = ncolors;
      } else if (ncolors == 3) {
        bpp = 2;
      } else if (ncolors < 16) {
        bpp = 4;
      } else if (ncolors < 256) {
        bpp = 8;
      } else {
        return false;
      }
      break;
    }

    case 0x10: {
      if (ncolors == 0) {
        bpp = 1;
      } else if (ncolors < 3) {
        bpp = 2;
      } else if (ncolors < 15) {
        bpp = 4;
      } else if (ncolors < 256) {
        bpp = 8;
      } else {
        return false;
      }
      break;
    }

    case 0x20: {
      if (ncolors == 0) {
        bpp = 16;
      } else if (ncolors < 3) {
        bpp = ncolors;
      } else if (ncolors < 4) {
        bpp = 2;
      } else if (ncolors < 16) {
        bpp = 4;
      } else if (ncolors < 256) {
        bpp = 8;
      } else {
        return false;
      }
      break;
    }

    default:
      return false;
  }

  bytes = (w * bpp) / 8;
  if ((w * bpp) & 0x07) {
    ++bytes;
  }

  return true;
}

bool CGarminTyp::decodeColorTable(QDataStream& in, QImage& img, int ncolors, int maxcolor, bool hasAlpha) {
  img.setColorCount(ncolors);

  if (hasAlpha) {
    int i;
    quint8 byte;
    quint32 bits = 0;
    quint32 reg = 0;
    quint32 mask = 0x000000FF;

    for (i = 0; i < ncolors; i++) {
      while (bits < 28) {
        in >> byte;
        mask = 0x000000FF << bits;
        reg = reg & (~mask);
        reg = reg | (byte << bits);
        bits += 8;
      }

      img.setColor(i, qRgba((reg >> 16) & 0x0FF, (reg >> 8) & 0x0FF, reg & 0x0FF, ~((reg >> 24) & 0x0F) << 4));

      reg = reg >> 28;
      bits -= 28;
    }
    for (; i < maxcolor; ++i) {
      img.setColor(i, qRgba(0, 0, 0, 0));
    }
  } else {
    int i;
    quint8 r, g, b;
    for (i = 0; i < ncolors; ++i) {
      in >> b >> g >> r;
      img.setColor(i, qRgb(r, g, b));
    }
    for (; i < maxcolor; ++i) {
      img.setColor(i, qRgba(0, 0, 0, 0));
    }
  }
  return true;
}

void CGarminTyp::decodeBitmap(QDataStream& in, QImage& img, int w, int h, int bpp) {
  int x = 0;
  quint8 color;

  if (bpp == 0) {
    return;
  }

  for (int y = 0; y < h; y++) {
    while (x < w) {
      in >> color;

      for (int i = 0; (i < (8 / bpp)) && (x < w); i++) {
        int value;
        if (i > 0) {
          value = (color >>= bpp);
        } else {
          value = color;
        }
        if (bpp == 4) {
          value = value & 0xf;
        }
        if (bpp == 2) {
          value = value & 0x3;
        }
        if (bpp == 1) {
          value = value & 0x1;
        }
        img.setPixel(x, y, value);
        //                 qDebug() << QString("value(%4) pixel at (%1,%2) is 0x%3 j is
        //                 %5").arg(x).arg(y).arg(value,0,16).arg(color).arg(j);
        x += 1;
      }
    }
    x = 0;
  }
}

bool CGarminTyp::parsePoint(QDataStream& in, QMap<quint32, point_property>& points) {
  //    bool tainted = false;

  if (!sectPoints.arrayModulo || ((sectPoints.arraySize % sectPoints.arrayModulo) != 0)) {
    return true;
  }

  QTextCodec* codec = getCodec(codepage);

  const int N = sectPoints.arraySize / sectPoints.arrayModulo;
  for (int element = 0; element < N; element++) {
    quint16 t16_1 = 0, t16_2, subtyp;
    quint8 t8_1;
    quint32 typ, offset = 0;
    bool hasLocalization = false;
    bool hasTextColor = false;
    quint8 langcode;
    quint8 r, g, b;

    in.device()->seek(sectPoints.arrayOffset + (sectPoints.arrayModulo * element));

    if (sectPoints.arrayModulo == 5) {
      in >> t16_1 >> t16_2 >> t8_1;
      offset = t16_2 | (t8_1 << 16);
    } else if (sectPoints.arrayModulo == 4) {
      in >> t16_1 >> t16_2;
      offset = t16_2;
    } else if (sectPoints.arrayModulo == 3) {
      in >> t16_1 >> t8_1;
      offset = t8_1;
    }

    t16_2 = (t16_1 >> 5) | ((t16_1 & 0x1f) << 11);
    typ = t16_2 & 0x7FF;
    subtyp = t16_1 & 0x01F;

    if (t16_1 & 0x2000) {
      typ = 0x10000 | (typ << 8) | subtyp;
    } else {
      typ = (typ << 8) + subtyp;
    }

    in.device()->seek(sectPoints.dataOffset + offset);

    int bpp = 0, wbytes = 0;
    quint8 w, h, ncolors, ctyp;
    in >> t8_1 >> w >> h >> ncolors >> ctyp;

    hasLocalization = t8_1 & 0x04;
    hasTextColor = t8_1 & 0x08;
    t8_1 = t8_1 & 0x03;
#ifdef DBG
    qDebug() << "Point typ:" << Qt::hex << typ << "ctyp:" << ctyp << "offset:" << (sectPoints.dataOffset + offset)
             << "orig data:" << t16_1;
#endif

    if (!decodeBppAndBytes(ncolors, w, ctyp, bpp, wbytes)) {
      continue;
    }

#ifdef DBG
    qDebug() << "          " << dec << "w" << w << "h" << h << "ncolors" << ncolors << "bpp" << bpp << "wbytes"
             << wbytes;
#endif

    if (ctyp == 0x20 || ctyp == 0x00) {
      if ((ncolors == 0) && (bpp >= 16)) {
        ncolors = w * h;
      }
    }

    point_property& property = points[typ];
    QImage imgDay(w, h, QImage::Format_Indexed8);
    QImage imgNight(w, h, QImage::Format_Indexed8);

    if (!decodeColorTable(in, imgDay, ncolors, 1 << bpp, ctyp == 0x20)) {
      continue;
    }

    if (bpp >= 16) {
      continue;
    } else {
      decodeBitmap(in, imgDay, w, h, bpp);
      property.imgDay = imgDay;
    }

    if (t8_1 == 0x03) {
      in >> ncolors >> ctyp;
      if (!decodeBppAndBytes(ncolors, w, ctyp, bpp, wbytes)) {
        continue;
      }
      if (!decodeColorTable(in, imgNight, ncolors, 1 << bpp, ctyp == 0x20)) {
        continue;
      }
      decodeBitmap(in, imgNight, w, h, bpp);
      points[typ].imgNight = imgNight;
    } else if (t8_1 == 0x02) {
      in >> ncolors >> ctyp;
      if (!decodeBppAndBytes(ncolors, w, ctyp, bpp, wbytes)) {
        continue;
      }
      if (!decodeColorTable(in, imgDay, ncolors, 1 << bpp, ctyp == 0x20)) {
        continue;
      }
      property.imgNight = imgDay;
    } else {
      property.imgNight = imgDay;
    }

    if (hasLocalization) {
      qint16 len;
      quint8 n = 1;

      in >> t8_1;
      len = t8_1;

      if (!(t8_1 & 0x01)) {
        n = 2;
        in >> t8_1;
        len |= t8_1 << 8;
      }

      len -= n;
      while (len > 0) {
        QByteArray str;
        in >> langcode;
        languages << langcode;
        len -= 2 * n;
        while (len > 0) {
          in >> t8_1;
          len -= 2 * n;

          if (t8_1 == 0) {
            break;
          }

          str += t8_1;
        }
        if (codec != nullptr) {
          property.strings[langcode] = codec->toUnicode(str);
        }
#ifdef DBG
        qDebug() << len << langcode << property.strings[langcode];
#endif
      }
    }

    if (hasTextColor) {
      in >> t8_1;
      property.labelType = (label_type_e)(t8_1 & 0x07);

      if (t8_1 & 0x08) {
        in >> b >> g >> r;
        property.colorLabelDay = qRgb(r, g, b);
      }

      if (t8_1 & 0x10) {
        in >> b >> g >> r;
        property.colorLabelNight = qRgb(r, g, b);
      }
#ifdef DBG
      qDebug() << "ext. label: type" << property.labelType << "day" << property.colorLabelDay << "night"
               << property.colorLabelNight;
#endif
    }
  }

  return true;
}
