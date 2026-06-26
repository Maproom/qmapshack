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

#ifndef CMAPWMTS_H
#define CMAPWMTS_H
#include <QMap>

#include "map/IMapOnline.h"

class CMapDraw;
class CDiskCache;
class QNetworkAccessManager;
class QNetworkReply;
class QListWidgetItem;

class CMapWMTS : public IMapOnline {
  Q_OBJECT
 public:
  CMapWMTS(const QString& filename, CMapDraw* parent);
  virtual ~CMapWMTS() {}

  void draw(IDrawContext::buffer_t& buf) override;

  void getLayers(QListWidget& list) override;

  void saveConfig(QSettings& cfg) override;
  void loadConfig(QSettings& cfg) override;

 private slots:
  void slotLayersChanged(QListWidgetItem* item);

 private:
  struct limit_t {
    qint32 minTileRow;
    qint32 maxTileRow;
    qint32 minTileCol;
    qint32 maxTileCol;
  };

  struct layer_t {
    bool enabled;
    QString title;
    QStringList styles;
    QString tileMatrixSet;
    QRectF boundingBox;
    QString resourceURL;
    QMap<QString, limit_t> limits;

    /**
       Ratio of the actually served tile size to the TileWidth declared in the
       TileMatrixSet. Some HiDPI servers violate the WMTS spec by reusing a
       standard TileMatrixSet (e.g. TileWidth 256) while serving larger tiles
       (e.g. 512px). The ratio is learned from the first fetched tile and used
       to pick a matching (coarser) matrix so an over-sized tile still maps 1:1
       onto the physical buffer pixels. 1.0 means the server is spec-compliant.
     */
    qreal tileScale = 1.0;
  };

  QList<layer_t> layers;

  struct tilematrix_t {
    QPointF topLeft;
    qreal scale;
    qint32 tileWidth;
    qint32 tileHeight;
    qint32 matrixWidth;
    qint32 matrixHeight;
  };

  struct tileset_t {
    CProj proj;
    QMap<QString, tilematrix_t> tilematrix;
  };

  QMap<QString, tileset_t> tilesets;
};

#endif  // CMAPWMTS_H
