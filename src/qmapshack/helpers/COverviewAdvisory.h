/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef COVERVIEWADVISORY_H
#define COVERVIEWADVISORY_H

#include <QElapsedTimer>
#include <QString>
#include <QVector>
#include <atomic>
#include <limits>

class GDALDataset;
class GDALRasterBand;
class IDrawContext;
class QSettings;

/**
   @brief One VRT's overview situation: the measured advice, the geometry the dialog shows,
          the render deadline that triggers it, and the suppression policy.

   CMapVRT and CDemVRT each hold one, and hold no advisory state of their own. Everything
   measured is immutable per instance - a Fix/Combine destroys the owner and a fresh one
   probes again.
 */
class COverviewAdvisory {
 public:
  /// @brief One leaf source raster's own overview state; see advice_t::perFileInfo.
  struct file_info_t {
    QString path;
    QVector<qint32> overviewSizes; /**< This file's own overview pixel widths, sorted descending (finest
                                        first, coarsest last); empty if none, or unchecked. */
    bool sufficient = false;       /**< True if the coarsest entry above already meets the suggested target -
                                        see COverviewAdvisory.cpp's meetsTarget(). Meaningless when checked is false. */
    bool checked = true;           /**< False if never probed - the container's own overview already covers it. */
  };

  /// @brief The current overview situation and what's needed to fix slow rendering.
  struct advice_t {
    QVector<qint32> containerOverviewSizes; /**< The container's own verified overview pixel widths, sorted
                                                 descending (finest first, coarsest last); empty if none or
                                                 unverified. */
    bool containerHasOwnOvr = false;        /**< True only if containerSufficient's verdict came from a real .ovr
                                                 file (found via GetFileList()); false if verified indirectly
                                                 instead, or the container has no usable overview. Meaningful only
                                                 when perFileInfo is non-empty. */
    bool containerSufficient = false;       /**< True if the container's own (verified) overview already meets
                                                 suggestedLevels' target everywhere - decided directly from pixel
                                                 sizes, see COverviewAdvisory.cpp's meetsTarget(). */
    QVector<file_info_t> perFileInfo;       /**< One entry per leaf source raster, weakest-first; a nested VRT is
                                                 followed through to the rasters behind it. Empty only if the
                                                 container references no sources - see file_info_t::checked. */
    QVector<qint32> suggestedLevels;        /**< Target overview levels - what a fix will build and what a rebuilt
                                                 <OverviewList> will declare. */
    bool isPaletteIndexed = false;          /**< True for single-band palette/gray data: selects nearest-neighbour
                                                 resampling instead of average, to avoid blending palette indices. */
    qint64 diskUsageBytes = 0;              /**< Total on-disk footprint (sub-files + overviews). Real summed size
                                                 when fully qualified; sub-files x 1 2/3 otherwise - see
                                                 diskUsageIsEstimate. */
    bool diskUsageIsEstimate = false;       /**< True when diskUsageBytes is the sub-files x 1 2/3 projection. */

    /// @brief Subfile count above which reading gets inefficient regardless of overviews.
    static constexpr qint32 kMaxSubfileCount = 50;

    /// @brief True if overviews need fixing. Subfile count is a separate, additive
    ///        concern - see hasTooManySubfiles().
    bool needsOverviewFix() const {
      if (suggestedLevels.isEmpty() || containerSufficient) {
        return false;
      }
      if (perFileInfo.isEmpty()) {
        return true;  // nothing - neither the container nor any source file - backs this raster
      }
      for (const file_info_t& info : perFileInfo) {
        if (info.checked && !info.sufficient) {
          return true;
        }
      }
      return false;
    }

    /// @brief True if more than kMaxSubfileCount leaf rasters are referenced - GDAL stats
    ///        every one overlapping a read region, whatever VRT levels sit above it.
    bool hasTooManySubfiles() const { return perFileInfo.size() > kMaxSubfileCount; }

    /// @brief True if either problem needs attention.
    bool needsAttention() const { return needsOverviewFix() || hasTooManySubfiles(); }
  };

  /// @brief Dataset dimensions/pixel size, purely for the informational line the dialog
  ///        shows above its tables - not used by the advisory logic.
  struct geometry_t {
    qint32 xsizePx = 0;   /**< Source raster width in pixels (pre-warp - matches gdalinfo). */
    qint32 ysizePx = 0;   /**< Source raster height in pixels (pre-warp). */
    qreal pixelSizeX = 0; /**< Real-world size of one source pixel along x, in meters. Exact for a
                              projected (meter) CRS - equals gdalinfo. For a geographic (lat/long)
                              CRS it is the CGdalVrtUtil::kMetersPerDegree approximation. */
    qreal pixelSizeY = 0; /**< Same as pixelSizeX, along y. */
  };

  /// @brief Per-draw() budget: one is shared by every ReadRaster() of a single draw(), so
  ///        a multi-band read doesn't get a fresh budget per band.
  struct read_deadline_t {
    IDrawContext* drawCtx;   /**< The owning CDemDraw/CMapDraw. */
    QElapsedTimer timer;     /**< Started once, right before the first ReadRaster() call of the draw(). */
    qint64 timeoutMs = 5000; /**< Render timeout budget for the whole draw() call. 5s is deliberate. */
    bool timedOut = false;   /**< Set once timer exceeds timeoutMs; distinguishes a timeout abort from an
                                  ordinary superseded-redraw abort. */
  };

  /// @brief Abort the read on a newer redraw request, or once the deadline expires.
  /// @param pProgressArg a read_deadline_t*, timer already started by the caller
  static int progressCallbackWithDeadline(double dfComplete, const char* message, void* pProgressArg);

  /// @brief Turn the advisory off for this instance (CDemWCS). Call before probe(); a
  ///        disabled advisory never warns and offers no info.
  void setEnabled(bool yes) { enabled_ = yes; }
  bool isEnabled() const { return enabled_; }

  /**
     @brief Measure dataset's overview situation and cache the verdict; no-op when disabled.
     @param dataset          the pre-warp dataset, as just opened from the file
     @param band             dataset's band 1
     @param isPaletteIndexed true for single-band palette/gray data
     @param maxFactor        cap on the deepest suggested level. DEM data stays meaningful
                             at any decimation and leaves it unbounded; a map past roughly
                             10-20x is a pixel mishmash, so CMapVRT caps.
   */
  void probe(GDALDataset* dataset, GDALRasterBand* band, bool isPaletteIndexed,
             qint32 maxFactor = std::numeric_limits<qint32>::max());

  /// @brief Record the geometry the dialog displays.
  /// @param preWarpSource srcDataset if a warp was created, else the dataset itself. The
  ///        warped geotransform gives wrong pixel sizes (+26% for UTM drawn in 4326).
  void setGeometry(GDALDataset* preWarpSource);

  const advice_t& advice() const { return advice_; }
  const geometry_t& geometry() const { return geometry_; }

  /// @brief Drives the tree badge. Cached at probe() time - the delegate polls it per
  ///        paint and needsAttention() walks every source file.
  bool showsWarning() const { return enabled_ && !suppress_ && needsAttention_; }

  /// @brief Seeds the dialog's "don't show again" checkbox, and drives the tree's
  ///        "Overview Info..." entry regardless of attention state.
  bool suppressed() const { return suppress_; }
  void setSuppressed(bool yes) { suppress_ = yes; }

  /// @brief Persist/restore the "don't show again" choice under the owner's config group.
  void save(QSettings& cfg) const;
  void load(QSettings& cfg);

  /**
     @brief draw()'s render-timeout policy: raise the advisory once per loaded instance.

     Never requests a redraw - the view is unchanged, so a retry hits the same deadline and
     requests again, an unbounded loop on any dataset that stays slow.

     A template because emitOverviewAdvisory() is overloaded per source type with no common
     base call, and drawCtx is a raw CDemDraw* on one side and a QPointer<CMapDraw> on the
     other; both support operator->().
   */
  template <class DrawCtxT, class SourceT>
  void onRenderTimeout(DrawCtxT drawCtx, SourceT* source) {
    if (showsWarning() && !advisoryRequested_) {
      advisoryRequested_ = true;
      drawCtx->emitOverviewAdvisory(source);
    }
  }

 private:
  bool enabled_ = true;
  advice_t advice_;
  geometry_t geometry_;
  /// Cached advice_.needsAttention(); see showsWarning().
  bool needsAttention_ = false;
  /// Persisted: written on the GUI thread, read by draw() on the canvas thread.
  std::atomic<bool> suppress_ = false;
  /// Not persisted: true once this instance has asked for the advisory, so panning a slow
  /// file doesn't ask again on every redraw. Only draw() touches it. "Asked", not "shown" -
  /// the canvas declines while another advisory is open; the tree badge covers that case.
  bool advisoryRequested_ = false;
};

#endif  // COVERVIEWADVISORY_H
