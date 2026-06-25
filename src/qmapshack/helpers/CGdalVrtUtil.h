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

class GDALDataset;
class GDALRasterBand;
class IDrawContext;

/**
   @brief GDAL helpers shared by the warped-VRT-backed map/DEM classes (CMapVRT, CDemVRT).

   These operate purely on GDAL types and the IDrawContext interface both classes' draw
   contexts (CMapDraw/CDemDraw) already implement, with no knowledge of maps or DEMs
   specifically: both classes need the exact same answer to the exact same low-level
   question (does a referenced file exist, what overview factors are available, ...), so
   the logic lives here once instead of being kept in sync by hand in two places.
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

  /// @brief One referenced file's own overview levels; see overview_factors_t::perFileInfo.
  struct file_overview_info_t {
    QString path;
    /// this file's own overview decimation factors, in the dataset's pixel scale, sorted
    /// ascending; empty if it has none
    QVector<qint32> factors;
  };

  /// @brief Result of collectOverviewFactors() - the three pieces always travel and get
  ///        consumed together (see buildOverviewAdvice()), so they're one struct rather
  ///        than a return value plus two out-params.
  struct overview_factors_t {
    /// sorted, de-duplicated decimation factors; empty if none are available anywhere
    QVector<qint32> factors;
    /// the *weakest* file's own deepest overview factor (in dataset's pixel scale) - i.e.
    /// the most decimation any single read can get from overviews alone in the worst
    /// case. A file contributing no usable overviews counts as 1 (native resolution
    /// only), so one under-prepared file in an otherwise well-prepared mosaic still pulls
    /// this down correctly. 1 if dataset itself reports no overviews and the per-file
    /// fallback found nothing either.
    qint32 weakestMaxFactor = 1;
    /// one entry per referenced file (or a single entry for dataset itself, if it reports
    /// its own overviews directly) - the breakdown the advisory dialog tables, so the
    /// user can see which specific file is the weak link
    QVector<file_overview_info_t> perFileInfo;
  };

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
     @return see overview_factors_t
   */
  static overview_factors_t collectOverviewFactors(GDALDataset* dataset, GDALRasterBand* pBand, qreal pixelSizeX);

  /**
     @brief Close a GDAL dataset and reset the pointer, tolerating a null dataset.

     GDALClose() logs a CPL error when passed a null handle instead of just ignoring
     it, so every call site needs this guard - which happens whenever no warped VRT
     was needed (srcDataset stays null) or construction failed before dataset was set.
   */
  static void closeDataset(GDALDataset*& dataset);

  /**
     @brief GDAL progress callback aborting the read once a newer redraw has been requested.
     @param pProgressArg an IDrawContext (the owning CDemDraw/CMapDraw) passed in as
                          progress callback context
   */
  static int progressCallback(double dfComplete, const char* message, void* pProgressArg);

  /**
     @brief Advisory info on whether/how to add overviews to a slow-reading dataset.

     Both commands always add `-ro` (never alter the original file(s)) and
     `--config COMPRESS_OVERVIEW DEFLATE` (safe, lossless). vrtCommand and filesCommand
     are mutually exclusive with the single-file case: a plain (non-container) file only
     ever fills filesCommand, targeting itself. They are filled in whenever there is
     anything sensible to suggest at all (see buildOverviewAdvice()), regardless of
     overviewsMissing - callers decide whether to act on them per-draw(), since whether
     the *existing* overviews are deep enough depends on what that particular read needed.
   */
  struct overview_advice_t {
    /// true if neither the dataset nor any file it references has overviews yet - always
    /// the "fixable by adding overviews" case
    bool overviewsMissing = false;
    /// the weakest referenced file's own deepest overview factor (see
    /// overview_factors_t::weakestMaxFactor); even when overviewsMissing is false, a
    /// draw() that needs more decimation than this is still hitting an inadequately
    /// prepared file and the advisory still applies
    qint32 weakestMaxFactor = 1;
    /// per-file breakdown (overview_factors_t::perFileInfo), sorted weakest-first; only
    /// meaningful (and only worth tabling in the dialog) when overviewsMissing is false -
    /// if it's true every entry is trivially maxFactor == 1
    QVector<file_overview_info_t> perFileInfo;
    /// gdaladdo command targeting the container file itself (e.g. a .vrt mosaic); empty
    /// if filename is not itself a multi-file container
    QString vrtCommand;
    /// gdaladdo command(s) targeting the distinct underlying source file(s): a single
    /// command if filename is a plain file, one full standalone command per line (no
    /// shell loop construct, so it pastes/runs unmodified in any shell on any OS) if it
    /// is a multi-file container
    QString filesCommand;
    /// rough *uncompressed* size of the overview pyramid these commands would add, in
    /// bytes; an infinite decimation pyramid sums to 1/3 of the base layer's own size
    /// (1/4 + 1/16 + 1/64 + ... converges to 1/3), and truncating it at suggestOverviewLevels()'s
    /// stopping point changes that negligibly for any raster with more than a couple of
    /// levels. Actual on-disk size is usually smaller thanks to DEFLATE compression -
    /// this is the size before that, a safe upper bound to warn the user with.
    qint64 estimatedOverviewBytes = 0;
  };

  /**
     @brief Build the suggested overview command(s) for filename.
     @param dataset      the (pre-warp) dataset, as just opened from filename
     @param band         dataset's band 1
     @param filename     the path passed to CDemVRT's/CMapVRT's constructor
     @param isCategorical true for single-band palette/gray data (selects nearest
                          neighbour decimation instead of average, to avoid blending
                          index values)
     @param overviewFactors collectOverviewFactors()'s result for dataset; an empty
                          overviewFactors.factors means overviews are missing anywhere in
                          the dataset
     @return advice with overviewsMissing/weakestMaxFactor/perFileInfo set accordingly and
             the relevant command(s) filled in whenever there is anything sensible to suggest
   */
  static overview_advice_t buildOverviewAdvice(GDALDataset* dataset, GDALRasterBand* band, const QString& filename,
                                               bool isCategorical, overview_factors_t overviewFactors);

  /**
     @brief Suggest gdaladdo overview decimation levels for a raster of the given size.

     Doubles from 2 until the smallest overview would be about one tile (256px) across,
     the standard "stop once an overview is about as small as a single tile" heuristic.
     @return sorted decimation factors, e.g. {2, 4, 8, 16}; empty if the raster is
             already smaller than one tile
   */
  static QVector<qint32> suggestOverviewLevels(qint32 xsize, qint32 ysize);

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
    qint64 timeoutMs = 10000;
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
