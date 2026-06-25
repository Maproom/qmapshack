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
#include "helpers/CGdalVrtUtil.h"

class CDemDraw;
class GDALDataset;

/**
   @brief GDAL-backed IDem implementation: reads elevation data from any raster format
          GDAL can open (GeoTIFF, VRT, IMG, ...).

   On construction, the file is opened read-only and validated (single band, 16/32 bit
   integer or float, georeferenced). If the dataset's spatial reference differs from the
   draw context's projection, it is wrapped in a GDALAutoCreateWarpedVRT so all later
   reads transparently happen in the draw context's projection; the original dataset is
   then kept alive as srcDataset purely because the warped VRT references it.

   draw() reads exactly the (downsampled) elevation window the current view needs via
   GDAL's RasterIO/ReadRaster, including a 1px border, then runs the requested IDem
   shading layers over that window in parallel (see threadPool) before compositing them
   onto the map.

   @note All access to dataset/srcDataset must hold mutex. draw() runs on the canvas
         thread while getElevationAt()/getSlopeAt() can be called from other threads
         (e.g. the GUI thread for cursor readouts), and GDAL datasets are not safe for
         concurrent access from multiple threads.

   @see CDemWCS, a thin subclass for WCS-backed sources that otherwise behaves exactly
        like a local file.
 */
class CDemVRT : public IDem {
  Q_OBJECT
 public:
  /**
     @brief Open filename via GDAL and set up georeferencing.

     On any failure (file cannot be opened, a file referenced by a VRT is missing,
     unsupported raster layout, no usable georeference, ...) a warning dialog is shown
     and the object is left with isActivated == false; callers must check activated()
     after construction.

     @param filename path of a file GDAL can open as a single-band raster
     @param parent    the owning CDemDraw, forwarded to IDem
     @param supportsOverviewAdvisory false for remote sources (CDemWCS): skips collecting
                       overview-advisory info entirely, not just showing the dialog for
                       it, since collectOverviewFactors()'s per-file fallback calls
                       GetFileList() then GDALOpen() on every referenced "file" -
                       meaningless for a source with no local files to inspect. Must be a
                       constructor parameter, not a virtual method CDemWCS overrides: a
                       virtual call made while CDemVRT's own constructor is still running
                       always resolves to CDemVRT's own implementation, never a derived
                       override, since the derived part of the object hasn't been
                       constructed yet.
   */
  CDemVRT(const QString& filename, CDemDraw* parent, bool supportsOverviewAdvisory = true);

  /// Waits for any in-flight threadPool work to finish, then closes the GDAL dataset(s).
  virtual ~CDemVRT();

  /// @brief Persist the overview-advisory-dialog suppression flag in addition to IDem::saveConfig().
  void saveConfig(QSettings& cfg) override;

  /// @brief Restore what saveConfig() persisted.
  void loadConfig(QSettings& cfg) override;

  /**
     @brief Render the requested view rectangle's hillshading/slope/elevation layers
            into buf.image.

     Reads just the part of the dataset the current view covers (at a resolution
     matching the current zoom), computes the enabled shading layers in parallel chunks
     on threadPool, and composites them onto buf.image. A no-op if the dataset is out of
     its configured min/max scale or no shading layer is enabled.

     @param buf the canvas buffer to draw into; also carries the view's corner
                coordinates and zoom/scale factors
   */
  void draw(IDrawContext::buffer_t& buf) override;

  /**
     @brief Look up the bilinearly-interpolated elevation at a single point.
     @param pos        position in WGS84 [rad]
     @param checkScale if true, return NOFLOAT when the dataset is out of its configured
                        min/max draw scale (see IDrawObject::isOutOfScale())
     @return elevation in meters, or NOFLOAT if pos is outside the dataset, the sample
             window contains a noData value, or the read fails
   */
  qreal getElevationAt(const QPointF& pos, bool checkScale) override;

  /**
     @brief Look up the interpolated terrain slope at a single point.
     @param pos        position in WGS84 [rad]
     @param checkScale if true, return NOFLOAT when the dataset is out of its configured
                        min/max draw scale (see IDrawObject::isOutOfScale())
     @return slope in degrees, or NOFLOAT if pos is outside the dataset, the sample
             window contains a noData value, or the read fails
   */
  qreal getSlopeAt(const QPointF& pos, bool checkScale) override;

  /// @brief The path passed to the constructor; used by the overview advisory dialog.
  const QString& getFilename() const { return filename; }

  /// @brief Cached suggested-overview info for this file; used by the overview advisory dialog.
  const CGdalVrtUtil::overview_advice_t& getOverviewAdvice() const { return overviewAdvice; }

 public slots:
  /// @brief Set by the overview advisory dialog's "don't show again for this file" checkbox.
  void slotSetSuppressOverviewAdvisory(bool yes) { suppressOverviewAdvisory = yes; }

 private slots:
  /// Cancel any shading work still queued/running for a draw() call that is now stale.
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

  /// guards all access to dataset/srcDataset (see class-level @note)
  mutable QMutex mutex;

  /// the path passed to the constructor; kept only for error messages and logging
  QString filename;
  /// the dataset as originally opened by GDAL; null unless a warped VRT was needed, in
  /// which case it is kept alive only because dataset (the warped VRT) references it
  GDALDataset* srcDataset = nullptr;
  /// the dataset actually read from: either the dataset as opened, or - if reprojection
  /// was needed - the warped VRT wrapping srcDataset
  GDALDataset* dataset = nullptr;

  /// maps DEM pixel coordinates to the dataset's own (possibly warped) CRS; output is in
  /// [rad] if that CRS is geographic (see proj.isSrcLatLong()), otherwise in the CRS's
  /// native units
  QTransform trFwd;
  /// trFwd inverted: maps the dataset's CRS back to DEM pixel coordinates
  QTransform trInv;

  /// true if the current draw scale is outside the object's min/max scale; written by
  /// draw() (canvas thread), read by getElevationAt()/getSlopeAt() (caller's thread)
  std::atomic<bool> outOfScale = false;

  /// the dataset's extent in trFwd's output space; used by toRasterPixel() to reject
  /// queries outside dataset coverage before touching GDAL
  QRectF boundingBox;

  /// false for remote sources (CDemWCS, via the constructor parameter of the same name) -
  /// set once at construction, never changes, so draw() can read it directly instead of
  /// through a virtual call (see the constructor's doc comment for why it has to be a
  /// constructor parameter rather than a virtual method)
  const bool supportsOverviewAdvisory;

  /// suggested gdaladdo command(s), computed once at construction from the dataset's own
  /// characteristics (skipped entirely when !supportsOverviewAdvisory); reused (never
  /// re-derived) whenever draw() hits the render timeout
  CGdalVrtUtil::overview_advice_t overviewAdvice;

  /// persisted via saveConfig()/loadConfig(): true once the user checked "don't show
  /// again" on the overview advisory dialog for this file. Written by
  /// slotSetSuppressOverviewAdvisory()/loadConfig() (GUI thread), read by draw() (canvas
  /// thread) - atomic for the same reason as outOfScale above.
  std::atomic<bool> suppressOverviewAdvisory = false;

  /// not persisted: true once the advisory has been shown for this loaded instance, so
  /// panning/zooming a slow file doesn't reopen the dialog on every redraw
  bool advisoryShownThisSession = false;

  /// runs the per-chunk shading work started by draw(); cancelled by slotNeedsRedraw()
  /// when a fresher redraw has been requested
  QThreadPool threadPool;

  /// draw()'s read buffer for raw elevation samples; kept as a member (resized, not
  /// reallocated, per draw()) so repeated redraws at a stable viewport size don't churn
  /// the heap every frame; see CMapVRT::indexData/bandBuf for the same pattern. Only ever
  /// touched from draw() (canvas thread), so no mutex is needed.
  QVector<float> data;

  /// draw()'s output buffers, one per shading layer (IDem::computeShading() computes every
  /// enabled layer in a single pass, so they must all coexist until painted, unlike data
  /// they can't share one buffer). Same reuse rationale as data; a layer that's never been
  /// enabled simply stays empty.
  QVector<quint8> hillshadeBuf;
  QVector<quint8> slopeShadeBuf;
  QVector<quint8> slopeColorBuf;
  QVector<quint8> elevationLimitBuf;
  QVector<quint8> elevationShadeBuf;
};

#endif  // CDEMVRT_H
