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
#include <QStringList>
#include <QVector>
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
  /**
     @brief Check that every file GDAL reports as part of the dataset (e.g. the files a
            VRT references) actually exists on disk.
     @param dataset     the dataset to check
     @param missingFile set to the first referenced file that could not be found; left
                         unchanged if all files exist
     @return false if a referenced file is missing
   */
  static bool allReferencedFilesExist(GDALDataset* dataset, QString& missingFile);

  /// @brief One referenced source file's own overview levels; see
  ///        overview_advice_t::perFileInfo.
  struct file_overview_info_t {
    QString path;
    /// This file's own overview levels, in the dataset's pixel scale, sorted ascending.
    /// Empty if it has none. Meaningless when checked is false.
    QVector<qint32> factors;
    /// False if this file was never probed, because the container's own overview
    /// already covers it (see overview_advice_t::containerFactor). factors is then
    /// empty because it was never checked, not because there is no overview.
    bool checked = true;
  };

  /**
     @brief Overview levels every one of sourcePaths currently supports, rescaled into
            pixelSizeX's pixel scale and intersected - exactly what gdalbuildvrt would
            declare for these sources today.

     Used by COverviewAdvisoryDialog to rewrite a container's <OverviewList> in place,
     without re-running gdalbuildvrt (which could change other VRT settings).
     @param sourcePaths every source file the container references
     @param pixelSizeX  the container's own pixel size along x
     @return sorted factors every source supports; empty if any source has none, or
             sourcePaths is empty
   */
  static QVector<qint32> intersectSourceOverviewFactors(const QStringList& sourcePaths, qreal pixelSizeX);

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
     @brief The current overview situation and what's needed to fix slow rendering.

     Built by buildOverviewAdvice(); used by draw() (fire the advisory) and
     COverviewAdvisoryDialog (show the situation/fix tables).
   */
  struct overview_advice_t {
    /// The container's own verified overview levels (its own pixel scale), sorted
    /// ascending. Empty if it has none, or its overview claim couldn't be verified.
    /// Kept in full, not just the deepest level, for display purposes.
    QVector<qint32> containerFactors;
    /// containerFactors.last(), or 0 if empty. The decimation a read gets everywhere
    /// via the container itself.
    qint32 containerFactor = 0;
    /// True if containerFactor came from a real .ovr file (found via GetFileList()).
    /// False if it was verified indirectly instead (every source file has its own
    /// overview) or containerFactor is 0. Meaningful only when perFileInfo is
    /// non-empty - a VRT with no source files of its own has no <OverviewList> to
    /// compare against.
    bool containerHasOwnOvr = false;
    /// The best decimation any read can rely on, worst case:
    /// - containerFactor, if that alone covers every source region
    /// - otherwise the weakest source file's own factor (a file with none counts as
    ///   1, i.e. native resolution)
    /// See buildOverviewAdvice() for why max(containerFactor, weakestSourceFactor) is
    /// exact, not an approximation.
    qint32 weakestMaxFactor = 1;
    /// One entry per referenced source file, sorted weakest-first. Empty when
    /// containerFactor alone already meets suggestedLevels' target - source files are
    /// never probed then. See file_overview_info_t::checked.
    QVector<file_overview_info_t> perFileInfo;
    /// Target overview levels (from suggestOverviewLevels()) - what the fix will
    /// build and what a rebuilt <OverviewList> will declare.
    QVector<qint32> suggestedLevels;
    /// True for single-band palette/gray data: selects nearest-neighbour resampling
    /// instead of average, to avoid blending palette index values.
    bool isPaletteIndexed = false;
    /// Rough uncompressed size of the overview pyramid, in bytes. A full pyramid sums
    /// to 1/3 of the base layer (1/4+1/16+1/64+... = 1/3); real size is usually
    /// smaller thanks to compression.
    qint64 estimatedOverviewBytes = 0;

    /**
       @brief True if weakestMaxFactor falls short of suggestedLevels' target - worth
              fixing. Always false when suggestedLevels is empty (the raster is
              already smaller than the screen).

       A method, not a cached field: callers rescale weakestMaxFactor after
       buildOverviewAdvice() returns, so this must always be evaluated on demand.
     */
    bool needsAttention() const {
      if (suggestedLevels.isEmpty()) {
        return false;
      }
      return weakestMaxFactor < suggestedLevels.last();
    }
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
     @param pixelSizeX   real-world size of one pixel along x; 0 if unknown (skips
                         rescaling source files' factors)
     @param isPaletteIndexed true for single-band palette/gray data (nearest-neighbour
                          decimation instead of average, to avoid blending index
                          values)
     @param suggestedLevels suggestOverviewLevels()'s result for dataset's size
     @return advice ready for COverviewAdvisoryDialog
   */
  static overview_advice_t buildOverviewAdvice(GDALDataset* dataset, GDALRasterBand* band, qreal pixelSizeX,
                                               bool isPaletteIndexed, const QVector<qint32>& suggestedLevels);

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
    /// the owning CDemDraw/CMapDraw; same role as progressCallback()'s pProgressArg
    IDrawContext* drawCtx;
    /// started once, right before the first ReadRaster() call of the draw()
    QElapsedTimer timer;
    /// render timeout budget for the whole draw() call
    qint64 timeoutMs = 5000;
    /// set by progressCallbackWithDeadline() once timer exceeds timeoutMs; distinguishes
    /// a timeout abort from the ordinary "a fresher redraw superseded this one" abort
    bool timedOut = false;
  };

  /**
     @brief Like progressCallback(), but additionally aborts once deadline->timeoutMs has
            elapsed since deadline->timer was started, recording that fact in
            deadline->timedOut.
     @param pProgressArg a read_deadline_t*, with timer already started by the caller
   */
  static int progressCallbackWithDeadline(double dfComplete, const char* message, void* pProgressArg);
};

#endif  // CGDALVRTUTIL_H
