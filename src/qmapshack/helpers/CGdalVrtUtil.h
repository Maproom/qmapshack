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
     @brief One referenced source file's own overview levels; see
            overview_advice_t::perFileInfo.
   */
  struct file_overview_info_t {
    QString path;
    QVector<qint32> factors; /**< This file's own overview levels, sorted ascending; empty if none, or unchecked. */
    bool checked = true;     /**< False if never probed - the container's own overview already covers it. */
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
     @brief The current overview situation and what's needed to fix slow rendering.

     Built by buildOverviewAdvice(); used by draw() (fire the advisory) and
     COverviewAdvisoryDialog (show the situation/fix tables).
   */
  struct overview_advice_t {
    QVector<qint32> containerFactors; /**< The container's own verified overview levels, sorted ascending; empty
                                           if none or unverified. Kept in full, not just the deepest level, for
                                           display purposes. */
    qint32 containerFactor = 0;       /**< containerFactors.last(), or 0 if empty - the decimation a read gets
                                           everywhere via the container itself. */
    bool containerHasOwnOvr = false;  /**< True only if containerFactor came from a real .ovr file (found via
                                           GetFileList()); false if verified indirectly instead, or
                                           containerFactor is 0. Meaningful only when perFileInfo is non-empty. */
    qint32 weakestMaxFactor = 1;      /**< The best decimation any read can rely on: max(containerFactor,
                                           weakestSourceFactor) - see buildOverviewAdvice() for why this is
                                           exact, not an approximation. */
    QVector<file_overview_info_t> perFileInfo; /**< One entry per referenced source file, weakest-first; empty
                                                    when containerFactor alone already meets suggestedLevels'
                                                    target - see file_overview_info_t::checked. */
    QVector<qint32> suggestedLevels;           /**< Target overview levels (from suggestOverviewLevels()) - what the fix
                                                    will build and what a rebuilt <OverviewList> will declare. */
    bool isPaletteIndexed = false;             /**< True for single-band palette/gray data: selects nearest-neighbour
                                                    resampling instead of average, to avoid blending palette index values. */
    qint64 estimatedOverviewBytes = 0;         /**< Rough uncompressed pyramid size in bytes (sums to 1/3 of the base
                                                    layer); real size is usually smaller thanks to compression. */

    /**
       @brief True if weakestMaxFactor falls short of suggestedLevels' target - worth
              fixing. Always false when suggestedLevels is empty (the raster is
              already smaller than the screen).
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
     @param isPaletteIndexed true for single-band palette/gray data (nearest-neighbour
                          decimation instead of average, to avoid blending index
                          values)
     @param suggestedLevels suggestOverviewLevels()'s result for dataset's size
     @return advice ready for COverviewAdvisoryDialog
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
    qint64 timeoutMs = 5000; /**< Render timeout budget for the whole draw() call. */
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

     `eligible` deliberately does not check overviewAdvice.needsAttention(): any render
     timeout, whatever its actual cause, is worth raising the dialog for once, since the
     user can check the overview situation even if it turns out something else made this
     particular render slow. It only ever fires once per loaded instance
     (shownThisSession), and a user who finds it unhelpful (e.g. slow rendering they've
     already accepted for an unrelated reason) can permanently suppress it via the
     dialog's checkbox.
     @param drawCtx owning CDemDraw/CMapDraw - taken by value (not DrawCtxT*) since
                    CDemVRT::dem is a raw CDemDraw* but CMapVRT::map is a QPointer<CMapDraw>;
                    both support operator->(), so a template parameter deduced from
                    whichever was passed works for either without forcing one shape.
     @param source  the CDemVRT/CMapVRT that just timed out
     @param eligible supportsOverviewAdvisory (if applicable) && !state.suppress -
                    intentionally independent of whether overviews are actually adequate,
                    see above
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
