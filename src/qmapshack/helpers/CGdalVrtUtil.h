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

class GDALDataset;

/**
   @brief Small GDAL conveniences shared by CMapVRT and CDemVRT.

   Plain wrappers over GDAL calls that both raster classes need in the same shape. The
   overview advisory lives in COverviewAdvisory, not here.
 */
class CGdalVrtUtil {
 public:
  /// @brief Approximate meters per degree of latitude/longitude at the equator; used to
  ///        convert a geographic (lat/long) CRS's geotransform pixel size into real meters.
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
     "file does not exist". Returns true if the file cannot be read; the caller's
     GDALOpen() then reports that failure itself.
   */
  static bool isFileUtf8(const QString& filename);

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
};

#endif  // CGDALVRTUTIL_H
