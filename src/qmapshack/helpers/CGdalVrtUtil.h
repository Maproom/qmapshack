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

#ifndef CGDALVRTUTIL_H
#define CGDALVRTUTIL_H

#include <QElapsedTimer>
#include <QString>
#include <QVector>
#include <atomic>
#include <limits>

class GDALDataset;
class GDALRasterBand;
class IDrawContext;

/**
   @brief GDAL helpers shared by CMapVRT and CDemVRT.

   Both need the same answers from GDAL (does a referenced file exist, what overview
   levels are available, ...), so the logic lives here once.
 */
class CGdalVrtUtil {
 public:
  /// @brief Approximate meters per degree of latitude/longitude at the equator; used to
  ///        convert a geographic (lat/long) CRS's geotransform pixel size into real
  ///        meters. Shared by CDemVRT/CMapVRT wherever they need real-world pixel size
  ///        from a dataset that may be in either a projected or a geographic CRS.
  static constexpr qreal kMetersPerDegree = 111120;

  /// @brief Convert one geotransform scale element to real meters.
  /// @param scale        e.g. adfGeoTransform[1]/[5] - degrees/pixel if isSrcLatLong,
  ///                     meters/pixel otherwise
  /// @param isSrcLatLong CProj::isSrcLatLong() for the same dataset
  static qreal toMeters(qreal scale, bool isSrcLatLong) { return isSrcLatLong ? scale * kMetersPerDegree : scale; }

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
     @brief Check that a VRT file's bytes are valid UTF-8.

     GDAL passes a VRT's <SourceFilename> bytes to the OS verbatim - it ignores the XML
     encoding declaration - so a non-UTF-8 (e.g. legacy Latin-1) VRT resolves to broken
     paths. Rejecting it up front gives a clear message instead of a garbled
     "file does not exist". Returns 0 if the file cannot be read; the caller's
     GDALOpen() then reports that failure itself.

     @return -1 file is not UTF-8 and cannot be transcoded
     @return  0 file is UTF-8 or cannot be read
     @return +1 file is UTF-8 after being transcoded
   */
  static int isFileUtf8(const QString& filename);

  /**
     @brief One referenced source file's own overview state; see overview_advice_t::perFileInfo.
   */
  struct file_overview_info_t {
    QString path;
    QVector<qint32> overviewSizes; /**< This file's own overview pixel widths, sorted descending (finest
                                        first, coarsest last); empty if none, or unchecked. */
    bool sufficient = false;       /**< True if the coarsest entry above already meets the suggested target -
                                        see CGdalVrtUtil.cpp's meetsTarget(). Meaningless when checked is false. */
    bool checked = true;           /**< False if never probed - the container's own overview already covers it. */
  };

  /**
     @brief Close a GDAL dataset and reset the pointer; tolerates a null dataset.

     GDALClose() logs an error on a null handle, so every call site needs this guard.
   */
  static void closeDataset(GDALDataset*& dataset);

  /**
     @brief GDAL progress callback aborting the read once a newer redraw has been requested.
     @param pProgressArg an IDrawContext (the owning CDemDraw/CMapDraw) passed in as
                          progress callback context
   */
  static int progressCallback(double dfComplete, const char* message, void* pProgressArg);

  /**
     @brief Dataset dimensions/pixel size, purely for the informational line
            CVrtAdvisoryDialog shows above the overview tables - not used by the
            overview-advisory logic above.
   */
  struct raster_geometry_t {
    qint32 xsizePx = 0;   /**< Source raster width in pixels (pre-warp - matches gdalinfo). */
    qint32 ysizePx = 0;   /**< Source raster height in pixels (pre-warp). */
    qreal pixelSizeX = 0; /**< Real-world size of one source pixel along x, in meters. Exact for a
                              projected (meter) CRS - equals gdalinfo. For a geographic (lat/long)
                              CRS it is the kMetersPerDegree approximation (degrees have no single
                              meter equivalent). */
    qreal pixelSizeY = 0; /**< Same as pixelSizeX, along y. */
  };

  /**
     @brief Source-file raster geometry for the advisory dialog: the pre-warp dataset's own
            size and pixel resolution, matching gdalinfo. Reads the source's own geotransform
            and CRS, so a projected (meter) CRS gives exact meters; a geographic CRS gives the
            kMetersPerDegree approximation. Returns a zero-initialised geometry if the dataset
            has no geotransform.
     @param source the ORIGINAL (pre-warp) dataset - srcDataset if a warp was created,
                   otherwise the dataset itself
   */
  static raster_geometry_t sourceGeometry(GDALDataset* source);

  /**
     @brief The current overview situation and what's needed to fix slow rendering.

     Built by buildOverviewAdvice(); used by draw() (fire the advisory) and
     CVrtAdvisoryDialog (show the situation/fix tables).
   */
  struct overview_advice_t {
    QVector<qint32> containerOverviewSizes;    /**< The container's own verified overview pixel widths, sorted
                                                    descending (finest first, coarsest last); empty if none or
                                                    unverified. */
    bool containerHasOwnOvr = false;           /**< True only if containerSufficient's verdict came from a real .ovr
                                                    file (found via GetFileList()); false if verified indirectly
                                                    instead, or the container has no usable overview. Meaningful only
                                                    when perFileInfo is non-empty. */
    bool containerSufficient = false;          /**< True if the container's own (verified) overview already meets
                                                    suggestedLevels' target everywhere - decided directly from pixel
                                                    sizes, see CGdalVrtUtil.cpp's meetsTarget(). */
    QVector<file_overview_info_t> perFileInfo; /**< One entry per referenced source file, weakest-first; empty
                                                    when containerSufficient alone already meets suggestedLevels'
                                                    target - see file_overview_info_t::checked. */
    QVector<qint32> suggestedLevels;           /**< Target overview levels (from suggestOverviewLevels()) - what the fix
                                                    will build and what a rebuilt <OverviewList> will declare. */
    bool isPaletteIndexed = false;             /**< True for single-band palette/gray data: selects nearest-neighbour
                                                    resampling instead of average, to avoid blending palette index values. */
    qint64 diskUsageBytes = 0;                 /**< Total on-disk footprint (sub-files + overviews). Real summed size
                                                    when fully qualified; sub-files x 1 2/3 when overviews are shallow,
                                                    missing or absent - see diskUsageIsEstimate. */
    bool diskUsageIsEstimate = false;          /**< True when diskUsageBytes is the sub-files x 1 2/3 projection
                                                    (overviews not yet fully built), false when it is the real total. */

    /// @brief Subfile count above which reading gets inefficient regardless of overviews.
    static constexpr qint32 kMaxSubfileCount = 50;

    /// @brief True if overviews need fixing (old needsAttention() logic). Subfile count
    ///        is a separate, additive concern - see hasTooManySubfiles().
    bool needsOverviewFix() const {
      if (suggestedLevels.isEmpty() || containerSufficient) {
        return false;
      }
      if (perFileInfo.isEmpty()) {
        return true;  // nothing - neither the container nor any source file - backs this raster
      }
      for (const file_overview_info_t& info : perFileInfo) {
        if (info.checked && !info.sufficient) {
          return true;
        }
      }
      return false;
    }

    /// @brief True if more than kMaxSubfileCount source files are referenced.
    bool hasTooManySubfiles() const { return perFileInfo.size() > kMaxSubfileCount; }

    /// @brief Drives the tree badge; true if either problem needs attention.
    bool needsAttention() const { return needsOverviewFix() || hasTooManySubfiles(); }
  };

  /**
     @brief Build advisory info: what already speeds up a read today, and what
            gdaladdo/<OverviewList> work would close the gap to suggestedLevels.

     A read can be sped up by two independent, additive sources:
     - the container's own overview (covers the whole extent, however it's backed)
     - the individual source file for whichever region is being read (GDAL checks
       this per source, regardless of what the container declares)
     Whichever gives more decimation for a region wins; neither shadows the other.

     dataset is always a VRT (CMapVRT/CDemVRT only ever open a .vrt file directly), so
     its own overview claim always needs verifying - its <OverviewList> can be stale or
     hand-edited with nothing behind it:
     - trusted immediately if a real .ovr file is listed (cheap, no GDALOpen)
     - otherwise trusted only if every source file individually has its own overview

     If the container's own verified level already meets suggestedLevels' target,
     source files are never probed individually - keeps a healthy, well-backed mosaic
     as cheap to check as before. Only a container that falls short triggers a full
     per-source probe, to find the true weakest link.
     @param dataset      the (pre-warp) dataset, as just opened from the file
     @param band         dataset's band 1
     @param isPaletteIndexed true for single-band palette/gray data (nearest-neighbour
                          decimation instead of average, to avoid blending index
                          values)
     @param suggestedLevels suggestOverviewLevels()'s result for dataset's size
     @return advice ready for CVrtAdvisoryDialog
   */
  static overview_advice_t buildOverviewAdvice(GDALDataset* dataset, GDALRasterBand* band, bool isPaletteIndexed,
                                               const QVector<qint32>& suggestedLevels);

  /**
     @brief Suggest gdaladdo overview decimation levels for a raster of the given size.

     Doubles from 2 until the overview would be smaller than the primary screen's longest
     dimension — overviews smaller than the screen are never useful (they would be upscaled).
     Falls back to 1920px if no screen is available.
     @param maxFactor hard cap on the deepest level, regardless of screen size. DEM data
                      stays numerically meaningful at any decimation (it's just smoother
                      terrain), so CDemVRT leaves this unbounded. Map imagery doesn't: past
                      roughly 10-20x, a downsampled map is just a pixel mishmash, however
                      large or high-resolution the source is - CMapVRT caps it accordingly.
     @return sorted decimation factors, e.g. {2, 4, 8, 16}; empty if the raster is
             already smaller than the screen
   */
  static QVector<qint32> suggestOverviewLevels(qint32 xsize, qint32 ysize,
                                               qint32 maxFactor = std::numeric_limits<qint32>::max());

  /**
     @brief Per-draw() state for progressCallbackWithDeadline(): one read_deadline_t is
            shared by every ReadRaster() call within a single draw(), so a multi-band
            read doesn't get a fresh budget per band.
   */
  struct read_deadline_t {
    IDrawContext* drawCtx;   /**< The owning CDemDraw/CMapDraw; same role as progressCallback()'s pProgressArg. */
    QElapsedTimer timer;     /**< Started once, right before the first ReadRaster() call of the draw(). */
    qint64 timeoutMs = 5000; /**< Render timeout budget for the whole draw() call. 5s is deliberate. */
    bool timedOut = false;   /**< Set by progressCallbackWithDeadline() once timer exceeds timeoutMs;
                                  distinguishes a timeout abort from an ordinary superseded-redraw abort. */
  };

  /**
     @brief Like progressCallback(), but additionally aborts once deadline->timeoutMs has
            elapsed since deadline->timer was started, recording that fact in
            deadline->timedOut.
     @param pProgressArg a read_deadline_t*, with timer already started by the caller
   */
  static int progressCallbackWithDeadline(double dfComplete, const char* message, void* pProgressArg);

  /**
     @brief Per-instance overview-advisory session bookkeeping, identical for CDemVRT and
            CMapVRT; see handleRenderTimeout().
   */
  struct overview_advisory_state_t {
    std::atomic<bool> suppress = false; /**< Persisted via saveConfig()/loadConfig(): true once the user checked
                                             "don't show again" for this file. Written on the GUI thread, read by
                                             draw() on the canvas thread - atomic for that reason. */
    bool shownThisSession = false;      /**< Not persisted: true once the advisory has been shown for this loaded
                                             instance, so panning/zooming a slow file doesn't reopen the dialog on
                                             every redraw. Only ever touched by draw() (canvas thread). */
    std::atomic<bool> open = false;     /**< True (GUI thread) while the advisory dialog is open; draw() (canvas
                                             thread) skips emitSigCanvasUpdate() retries while set, so the render
                                             thread doesn't busy-loop. */
  };

  /**
     @brief Decide what draw() should do after a render-timeout abort: show the overview
            advisory once per loaded instance, or ask for a plain redraw retry otherwise.

     Shared by CDemVRT::draw()/CMapVRT::draw() - identical policy, only the concrete
     drawCtx/source types differ. A template rather than a virtual call: DrawCtxT's
     emitOverviewAdvisory() overload is resolved per source type at compile time
     (CDemDraw::emitOverviewAdvisory(QPointer<CDemVRT>) vs.
     CMapDraw::emitOverviewAdvisory(QPointer<CMapVRT>) - there is no common base call).

     `eligible` is the source's showsOverviewWarning() (== !suppress &&
     overviewAdvice.needsAttention()), so a timeout only raises the dialog when there is
     actually something to fix - a render that is slow for an unrelated reason no longer
     pops it. It fires at most once per loaded instance (shownThisSession), and the user
     can permanently suppress it via the dialog's checkbox.
     @param drawCtx owning CDemDraw/CMapDraw - taken by value (not DrawCtxT*) since
                    CDemVRT::dem is a raw CDemDraw* but CMapVRT::map is a QPointer<CMapDraw>;
                    both support operator->(), so a template parameter deduced from
                    whichever was passed works for either without forcing one shape.
     @param source  the CDemVRT/CMapVRT that just timed out
     @param eligible source's showsOverviewWarning() - only true when overviews actually
                    need attention and the file isn't suppressed, see above
     @param state   source's overview_advisory_state_t
   */
  template <class DrawCtxT, class SourceT>
  static void handleRenderTimeout(DrawCtxT drawCtx, SourceT* source, bool eligible, overview_advisory_state_t& state) {
    if (eligible && !state.shownThisSession) {
      // Intentionally no emitSigCanvasUpdate() here: the dialog is about to ask the user
      // to clarify the situation, so retrying the canvas update now serves no purpose.
      // Once the dialog is closed, normal panning/zooming resumes and updates whatever
      // still needs it.
      state.shownThisSession = true;
      drawCtx->emitOverviewAdvisory(source);
    } else if (!state.open) {
      drawCtx->emitSigCanvasUpdate();
    }
  }
};

#endif  // CGDALVRTUTIL_H
