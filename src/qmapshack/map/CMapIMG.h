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

#ifndef CMAPIMG_H
#define CMAPIMG_H

#include <QMap>

#include "map/IMap.h"
#include "map/garmin/CGarminImg.h"
#include "map/garmin/CGarminPoint.h"
#include "map/garmin/CGarminPolygon.h"
#include "map/garmin/CGarminTyp.h"

class CMapDraw;
class BLContext;

class CMapIMG : public IMap {
  Q_OBJECT
 public:
  CMapIMG(const QString& filename, CMapDraw* parent);
  virtual ~CMapIMG() = default;

  void loadConfig(QSettings& cfg) override;

  void draw(IDrawContext::buffer_t& buf) override;

  void getToolTip(const QPoint& px, QString& infotext) const override;

  void findPOICloseBy(const QPoint&, IPoiItem& poi) const override;

  /**
     @brief Find a matching street polyline

     The polyline must be close enough in terms of pixel to point 1 and 2. "Close enough" is defined by
     the threshold. The returned polyline uses lon/lat as coordinates.

     @param pt1           first point in [rad]
     @param pt2           second point in [rad]
     @param threshold     the "close enough" threshold in [pixel]
     @param polyline      the resulting polyline, if any, in [rad]
     @return              Return true if a line has been found.
   */
  bool findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32 threshold, QPolygonF& polyline) override;

 public slots:
  void slotSetTypeFile(const QString& filename) override;

 private:
  struct strlbl_t {
    QPoint pt;
    QRect rect;
    QString str;
    CGarminTyp::point_property property;
    bool isNight = false;
  };

  quint8 scale2bits(const QPointF& scale);
  void setupTyp();
  bool intersectsWithExistingLabel(const QRect& rect) const;
  bool overlapsLabel(const QPainterPath& outline, const QList<QPainterPath>& placed) const;
  void addLabel(const CGarminPoint& pt, const QRect& rect, const CGarminTyp::point_property& property, bool isDay);
  void drawPolygons(BLContext& ctx, polytype_t& lines);
  void drawPolylines(BLContext& ctx, polytype_t& lines, const QPointF& scale);
  void drawPoints(BLContext& ctx, pointtype_t& pts, QVector<QRectF>& rectPois);
  void drawPois(BLContext& ctx, pointtype_t& pts, QVector<QRectF>& rectPois);
  // Text and labels are rendered through Blend2D as well: glyph outlines come from
  // QRawFont (cmap-only mapping, no complex shaping) and are filled in the shared context.
  void drawLabels(BLContext& ctx, const QVector<strlbl_t>& lbls);
  void drawText(BLContext& ctx);

  void drawLine(BLContext& ctx, CGarminPolygon& l, bool stroke, const CGarminTyp::polyline_property& property,
                const QFont& font, const QPointF& scale);
  void drawLine(BLContext& ctx, const CGarminPolygon& l);

  void collectText(const CGarminPolygon& item, const QPolygonF& line, const QFont& font, const QColor& color);

  void getInfoPoints(const pointtype_t& points, const QPoint& pt, QMultiMap<QString, QString>& dict) const;
  void getInfoPolylines(const QPoint& pt, QMultiMap<QString, QString>& dict) const;
  void getInfoPolygons(const QPoint& pt, QMultiMap<QString, QString>& dict) const;

  QFontMetrics fm;

  QMap<quint32, CGarminTyp::polyline_property> polylineProperties;
  QMap<quint32, CGarminTyp::polygon_property> polygonProperties;
  QList<quint32> polygonDrawOrder;
  QMap<quint32, CGarminTyp::point_property> pointProperties;
  QMap<quint8, QString> languages;

  polytype_t polygons;
  polytype_t polylines;
  pointtype_t points;
  pointtype_t pois;

  QVector<strlbl_t> labels;

  struct textpath_t {
    // QPainterPath path;
    QPolygonF polyline;
    QString text;
    QFont font;
    QColor color;
  };

  QVector<textpath_t> textpaths;
  qint8 selectedLanguage;
  /** reads and decodes the file */
  CGarminImg img;
};

#endif  // CMAPIMG_H
