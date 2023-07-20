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

#ifndef CDEMVRT_H
#define CDEMVRT_H

#include <QMutex>
#include <QThreadPool>

#include "dem/IDem.h"

class CDemDraw;
class GDALDataset;

class CDemVRT : public IDem {
  Q_OBJECT
 public:
  CDemVRT(const QString& filename, CDemDraw* parent);
  virtual ~CDemVRT();

  void draw(IDrawContext::buffer_t& buf) override;

  qreal getElevationAt(const QPointF& pos, bool checkScale) override;
  qreal getSlopeAt(const QPointF& pos, bool checkScale) override;

 private slots:
  void slotNeedsRedraw();

 private:
  using IDem::drawTile;
  void drawElevationShadeScale(QPainter& p) const;
  void drawTile(const qint32 x, const qint32 y, const qint32 w, const qint32 h,
                const qreal o1, const qreal o2, QPainter& p) const;

  mutable QMutex mutex;

  QString filename;
  /// instance of GDAL dataset
  GDALDataset* dataset;

  QPointF ref1;
  QPointF ref2;
  QPointF ref3;
  QPointF ref4;

  QTransform trFwd;
  QTransform trInv;

  bool hasOverviews = false;
  bool outOfScale = false;

  QRectF boundingBox;

  QThreadPool threadPool;
};

#endif  // CDEMVRT_H
