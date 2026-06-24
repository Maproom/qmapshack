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

#include <QString>
#include <QVector>

class GDALDataset;
class GDALRasterBand;

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
};

#endif  // CGDALVRTUTIL_H
