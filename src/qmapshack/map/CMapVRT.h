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

#ifndef CMAPVRT_H
#define CMAPVRT_H

#include "map/IMap.h"

class CMapDraw;
class GDALDataset;
class GDALRasterBand;
class QPainter;

/**
   @brief GDAL-backed IMap implementation: reads raster map tiles from any format
          GDAL can open (GeoTIFF, VRT, IMG, ...).

   On construction, the file is opened read-only and its color table (single
   palette/gray band) or band layout (multi-band RGB(A)) is read. If the dataset's
   spatial reference differs from the draw context's projection, it is wrapped in a
   GDALAutoCreateWarpedVRT so all later reads transparently happen in the draw
   context's projection; the original dataset is then kept alive as srcDataset purely
   because the warped VRT references it.

   draw() reads exactly the (downsampled) window the current view needs via GDAL's
   RasterIO/ReadRaster - letting GDAL pick overviews as needed - and composites it
   onto the map in a single draw.

   @see CDemVRT, which uses the same warp/read strategy for elevation data.
 */
class CMapVRT : public IMap {
  Q_OBJECT
 public:
  CMapVRT(const QString& filename, CMapDraw* parent);
  virtual ~CMapVRT();

  void draw(IDrawContext::buffer_t& buf) override;

 private:
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
     @brief Collect virtual-overview decimation factors for dataset, in dataset's own pixel
            scale.

     Prefers dataset's own overview list. If dataset reports none - e.g. a gdalbuildvrt
     mosaic whose <OverviewList> is stale, was never written because not every source had
     overviews when the mosaic was built, or still has one source that lacks them - falls
     back to the union of overview factors found across the individual files dataset depends
     on, so one under-prepared source no longer disables overview-accelerated reads for the
     whole mosaic. Each file's factors are converted via its own geotransform pixel size
     rather than reused as raw pixel ratios, so sources of differing native resolution stay
     consistent with dataset's own grid.
     @param dataset    the (pre-warp) dataset to collect overview factors for
     @param pBand      dataset's band 1
     @param pixelSizeX the real-world size of one of dataset's own pixels along x; pass 0 if
                       unknown to skip the per-file fallback entirely
     @return sorted, de-duplicated decimation factors; empty if none are available anywhere
   */
  static QVector<qint32> collectOverviewFactors(GDALDataset* dataset, GDALRasterBand* pBand, qreal pixelSizeX);

  /**
     @brief GDAL progress callback aborting the read once a newer redraw has been
            requested.
     @param pProgressArg the CMapDraw passed in as progress callback context
   */
  static int progressCallback(double dfComplete, const char* message, void* pProgressArg);

  /// Close a GDAL dataset and reset the pointer, tolerating a null dataset.
  static void closeDataset(GDALDataset*& dataset);

  /// Close dataset and srcDataset (either may already be null, e.g. if construction
  /// failed before a warped VRT was needed) and show msg in an error dialog.
  void fail(const QString& msg);

  /// the read window for one draw() call, in dataset's own pixel coordinate space,
  /// clipped to [0, xsize_px] x [0, ysize_px]; bufWidth/bufHeight are the dimensions of
  /// the (possibly downsampled) buffer GDAL should read into
  struct sourceWindow_t {
    qreal left;
    qreal top;
    qreal right;
    qreal bottom;
    qint32 bufWidth;
    qint32 bufHeight;
  };

  /**
     @brief Compute the read window draw() needs for the current view.
     @param buf         the canvas buffer being drawn; its ref1..ref4 corners are
                         converted into this dataset's own pixel coordinate space
     @param bufferScale buf.scale * buf.zoomFactor
     @param window      set to the clipped read window and read buffer size
     @return false if the current view does not intersect the dataset, or the
             intersection is empty after clipping (window is left unchanged)
   */
  bool computeSourceWindow(const IDrawContext::buffer_t& buf, const QPointF& bufferScale, sourceWindow_t& window) const;

  /**
     @brief Read window from dataset into a QImage.
     @param window the area/resolution to read, as computed by computeSourceWindow()
     @return Format_Indexed8 image for single-band palette/gray data, Format_ARGB32 for
             multi-band; a null QImage if the GDAL read failed or was aborted
   */
  QImage readSourceImage(const sourceWindow_t& window);

  /**
     @brief Composite img onto p at the screen position/orientation matching window.
     @param p      the canvas painter, already translated/opacity-set by draw()
     @param window the window img was read from, as computed by computeSourceWindow()
     @param img    the image returned by readSourceImage()
   */
  void drawSourceImage(QPainter& p, const sourceWindow_t& window, const QImage& img) const;

  /// Draw the outline of the loaded file's extent onto p.
  void drawOutline(QPainter& p) const;

  QString filename;
  /// the dataset as originally opened by GDAL; null unless a warped VRT was needed, in
  /// which case it is kept alive only because dataset (the warped VRT) references it
  GDALDataset* srcDataset = nullptr;
  /// the dataset actually read from: either the dataset as opened, or - if reprojection
  /// was needed - the warped VRT wrapping srcDataset
  GDALDataset* dataset = nullptr;
  /// number of color bands used by the *vrt
  qint32 rasterBandCount = 0;
  /// QT representation of the vrt's color table
  QVector<QRgb> colortable;

  /// draw()'s read buffer for the single-band indexed case; kept as a member (resized, not
  /// reallocated, per draw()) so repeated redraws at a stable viewport size don't churn the
  /// heap every frame
  QVector<quint8> indexData;
  /// draw()'s read buffer for one band of the multi-band ARGB32 case; same rationale as
  /// indexData
  QVector<quint8> bandBuf;

  /// width in number of px
  qint32 xsize_px = 0;
  /// height in number of px
  qint32 ysize_px = 0;

  /// scale [px/m]
  qreal xscale = 0;
  /// scale [px/m]
  qreal yscale = 0;

  /// corners of the *original* (pre-warp) raster's true footprint, already in
  /// canvas-internal WGS84 [rad] space; drawOutline() draws these via convertRad2Px()
  /// directly, with no further reprojection. Deliberately not derived from trFwd/proj,
  /// which (once a warp is in play) describe the padded, axis-aligned-in-target-SRS
  /// warped VRT instead of the source data's real (possibly rotated) shape.
  QPointF ref1;
  QPointF ref2;
  QPointF ref3;
  QPointF ref4;

  /// maps this map's pixel coordinates to the dataset's own (possibly warped) CRS;
  /// output is in [rad] if that CRS is geographic (see proj.isSrcLatLong()), otherwise
  /// in the CRS's native units
  QTransform trFwd;
  /// trFwd inverted: maps the dataset's CRS back to this map's pixel coordinates
  QTransform trInv;
};

#endif  // CMAPVRT_H
