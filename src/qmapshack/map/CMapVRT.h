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

#include <atomic>

#include "helpers/CGdalVrtUtil.h"
#include "map/IMap.h"

class CMapDraw;
class GDALDataset;
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

  /// @brief Persist the overview-advisory-dialog suppression flag in addition to IMap::saveConfig().
  void saveConfig(QSettings& cfg) override;

  /// @brief Restore what saveConfig() persisted.
  void loadConfig(QSettings& cfg) override;

  void draw(IDrawContext::buffer_t& buf) override;

  /// @brief The path passed to the constructor; used by the overview advisory dialog.
  const QString& getFilename() const { return filename; }

  /// @brief Cached suggested-overview info for this file; used by the overview advisory dialog.
  const CGdalVrtUtil::overview_advice_t& getOverviewAdvice() const { return overviewAdvice; }

  /// @brief Set by the overview advisory dialog's "don't show again for this file" checkbox.
  void setSuppressOverviewAdvisory(bool yes) { suppressOverviewAdvisory = yes; }
  /// @brief Set true while the advisory dialog is open; suppresses draw retries during that time.
  void setAdvisoryOpen(bool yes) { advisoryOpen = yes; }

  bool showsOverviewWarning() const override { return !suppressOverviewAdvisory && overviewAdvice.needsAttention(); }

 private:
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
    /// the clamped (>=1.0) decimation factor computeSourceWindow() used to size
    /// bufWidth/bufHeight; exposed so draw() can reuse the exact same values for its
    /// overview-advisory needed-factor check instead of recomputing them
    qreal bufScaleX;
    qreal bufScaleY;
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
     @param window  the area/resolution to read, as computed by computeSourceWindow()
     @param deadline shared render-timeout budget for every ReadRaster() call this draw()
                     makes (the multi-band loop reads one band per call); deadline.timedOut
                     is set if any of them aborts due to the timeout
     @return Format_Indexed8 image for single-band palette/gray data, Format_ARGB32 for
             multi-band; a null QImage if the GDAL read failed or was aborted
   */
  QImage readSourceImage(const sourceWindow_t& window, CGdalVrtUtil::read_deadline_t& deadline);

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

  /// suggested gdaladdo command(s), computed once at construction from the dataset's own
  /// characteristics; reused (never re-derived) whenever draw() hits the render timeout
  CGdalVrtUtil::overview_advice_t overviewAdvice;

  /// persisted via saveConfig()/loadConfig(): true once the user checked "don't show
  /// again" on the overview advisory dialog for this file. Written by
  /// setSuppressOverviewAdvisory()/loadConfig() (GUI thread), read by draw() (canvas
  /// thread) - must be atomic to avoid a data race across that boundary.
  std::atomic<bool> suppressOverviewAdvisory = false;

  /// not persisted: true once the advisory has been shown for this loaded instance, so
  /// panning/zooming a slow file doesn't reopen the dialog on every redraw
  bool advisoryShownThisSession = false;

  /// set true (GUI thread) while the advisory dialog is open, cleared when it closes;
  /// draw() (canvas thread) skips emitSigCanvasUpdate() retries while this is set so
  /// animations stop and the render thread doesn't busyloop during the dialog
  std::atomic<bool> advisoryOpen = false;
};

#endif  // CMAPVRT_H
