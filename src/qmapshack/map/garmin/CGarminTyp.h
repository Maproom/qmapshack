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

**********************************************************************************************/

#ifndef CGARMINTYP_H
#define CGARMINTYP_H

#include <QtGui>

class QTextCodec;

class CGarminTyp {
  Q_DECLARE_TR_FUNCTIONS(CGarminTyp)
 public:
  CGarminTyp() = default;
  virtual ~CGarminTyp() = default;

  enum label_type_e { eStandard = 0, eNone = 1, eSmall = 2, eNormal = 3, eLarge = 4 };

  struct polyline_property {
    polyline_property()
        : type(0),
          penLineDay(Qt::magenta, 3),
          penLineNight(Qt::magenta, 3),
          hasBorder(false),
          penBorderDay(Qt::NoPen),
          penBorderNight(Qt::NoPen),
          hasPixmap(false),
          labelType(eStandard),
          colorLabelDay(Qt::black),
          colorLabelNight(Qt::black),
          known(false)

    {}

    polyline_property(quint16 type, const QPen& penLineDay, const QPen& penLineNight, const QPen& penBorderDay,
                      const QPen& penBorderNight)
        : type(type),
          penLineDay(penLineDay),
          penLineNight(penLineNight),
          hasBorder(true),
          penBorderDay(penBorderDay),
          penBorderNight(penBorderNight),
          hasPixmap(false),
          labelType(eStandard),
          colorLabelDay(Qt::black),
          colorLabelNight(Qt::black),
          known(true) {}

    polyline_property(quint16 type, const QColor& color, int width, Qt::PenStyle style)
        : type(type),
          penLineDay(QPen(color, width, style)),
          penLineNight(penLineDay),
          hasBorder(false),
          penBorderDay(Qt::NoPen),
          penBorderNight(Qt::NoPen),
          hasPixmap(false),
          labelType(eStandard),
          colorLabelDay(Qt::black),
          colorLabelNight(Qt::black),
          known(true) {}

    quint16 type;

    QPen penLineDay;
    QPen penLineNight;

    bool hasBorder;
    QPen penBorderDay;
    QPen penBorderNight;

    bool hasPixmap;
    QImage imgDay;
    QImage imgNight;

    QMap<int, QString> strings;
    label_type_e labelType;
    QColor colorLabelDay;
    QColor colorLabelNight;

    bool known;
  };

  struct polygon_property {
    polygon_property()
        : type(0),
          pen(Qt::magenta),
          brushDay(Qt::magenta, Qt::BDiagPattern),
          brushNight(Qt::magenta, Qt::BDiagPattern),
          labelType(eStandard),
          colorLabelDay(Qt::black),
          colorLabelNight(Qt::black),
          known(false) {}

    polygon_property(quint16 type, const Qt::PenStyle pensty, const QColor& brushColor, Qt::BrushStyle pattern)
        : type(type),
          pen(pensty),
          brushDay(brushColor, pattern),
          brushNight(brushColor.darker(150), pattern),
          labelType(eStandard),
          colorLabelDay(Qt::black),
          colorLabelNight(Qt::black),
          known(true) {
      pen.setWidth(1);
    }

    polygon_property(quint16 type, const QColor& penColor, const QColor& brushColor, Qt::BrushStyle pattern)
        : type(type),
          pen(penColor, 1),
          brushDay(brushColor, pattern),
          brushNight(brushColor.darker(150), pattern),
          labelType(eStandard),
          colorLabelDay(Qt::black),
          colorLabelNight(Qt::black),
          known(true) {}

    quint16 type;
    QPen pen;
    QBrush brushDay;
    QBrush brushNight;

    QMap<int, QString> strings;
    label_type_e labelType;
    QColor colorLabelDay;
    QColor colorLabelNight;
    bool known;
  };

  struct point_property {
    point_property() : labelType(eStandard) {}
    QImage imgDay;
    QImage imgNight;

    QMap<int, QString> strings;
    label_type_e labelType;
    QColor colorLabelDay;
    QColor colorLabelNight;
  };

  /// decode typ file
  /**
      This pure virtual function has to be implemented in every subclass. It should
      be the only public function needed. The typ file is read and it's content is
      stored in the passed map/list objects.

      @param in input data stream
      @param polygons reference to polygon properties map
      @param polylines reference to polyline properties map
      @param drawOrder reference to list of polygon draw orders
      @param points reference to point properties map

   */
  bool decode(const QByteArray& array, QMap<quint32, polygon_property>& polygons,
              QMap<quint32, polyline_property>& polylines, QList<quint32>& drawOrder,
              QMap<quint32, point_property>& points);

  QSet<quint8> getLanguages() { return languages; }

  quint16 getFid() { return fid; }

  quint16 getPid() { return pid; }

 protected:
  virtual bool parseHeader(QDataStream& in);
  virtual bool parseDrawOrder(QDataStream& in, QList<quint32>& drawOrder);
  virtual bool parsePolygon(QDataStream& in, QMap<quint32, polygon_property>& polygons);
  virtual bool parsePolyline(QDataStream& in, QMap<quint32, polyline_property>& polylines);
  virtual bool parsePoint(QDataStream& in, QMap<quint32, point_property>& points);

  QTextCodec* getCodec(quint16 codepage);
  void decodeBitmap(QDataStream& in, QImage& img, int w, int h, int bpp);
  bool decodeBppAndBytes(int ncolors, int w, int flags, int& bpp, int& bytes);
  bool decodeColorTable(QDataStream& in, QImage& img, int ncolors, int maxcolor, bool hasAlpha);

  struct typ_section_t {
    typ_section_t() : dataOffset(0), dataLength(0), arrayOffset(0), arrayModulo(0), arraySize(0) {}
    quint32 dataOffset;
    quint32 dataLength;
    quint32 arrayOffset;
    quint16 arrayModulo;
    quint32 arraySize;
  };

  quint16 version = 0;
  quint16 codepage = 0;
  quint16 year = 0;
  quint8 month = 0;
  quint8 day = 0;
  quint8 hour = 0;
  quint8 minutes = 0;
  quint8 seconds = 0;

  quint16 fid = 0;
  quint16 pid = 0;

  typ_section_t sectPoints;
  typ_section_t sectPolylines;
  typ_section_t sectPolygons;
  typ_section_t sectOrder;

  QSet<quint8> languages;
};
#endif  // CGARMINTYP_H
