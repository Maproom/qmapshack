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
#include <atomic>

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
  void drawElevationShadeScale(QPainter& p) const;

  /**
     @brief Convert a position from the canvas's WGS84 [rad] space (as passed to
            getElevationAt()/getSlopeAt()) into this DEM's own pixel coordinate space.
     @param pos   position in WGS84 [rad]
     @param pixel set to the corresponding DEM pixel coordinate; left unchanged if pos is
                  outside the DEM's bounding box
     @return false if pos lies outside the DEM's bounding box
   */
  bool toRasterPixel(const QPointF& pos, QPointF& pixel) const;

  /**
     @brief Check that every file GDAL reports as part of the dataset (e.g. the files a
            VRT references) actually exists on disk.
     @param dataset     the dataset to check
     @param missingFile set to the first referenced file that could not be found; left
                         unchanged if all files exist
     @return false if a referenced file is missing
   */
  static bool allReferencedFilesExist(GDALDataset* dataset, QString& missingFile);

  /**
     @brief GDAL progress callback aborting the read once a newer redraw has been
            requested.
     @param pProgressArg the CDemDraw passed in as progress callback context
   */
  static int progressCallback(double dfComplete, const char* message, void* pProgressArg);

  /**
     @brief Close a GDAL dataset and reset the pointer, tolerating a null dataset.

     GDALClose() logs a CPL error when passed a null handle instead of just ignoring
     it, so every call site needs this guard - which happens whenever no warped VRT
     was needed (srcDataset stays null) or construction failed before dataset was set.
   */
  static void closeDataset(GDALDataset*& dataset);

  mutable QMutex mutex;

  QString filename;
  /// instance of GDAL dataset
  GDALDataset* srcDataset = nullptr;
  GDALDataset* dataset = nullptr;

  QTransform trFwd;
  QTransform trInv;

  /// true if the current draw scale is outside the object's min/max scale; written by
  /// draw() (canvas thread), read by getElevationAt()/getSlopeAt() (caller's thread)
  std::atomic<bool> outOfScale = false;

  QRectF boundingBox;

  QThreadPool threadPool;
};

#endif  // CDEMVRT_H
