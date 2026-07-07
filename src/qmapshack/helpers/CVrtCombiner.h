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

#ifndef CVRTCOMBINER_H
#define CVRTCOMBINER_H

#include <QVector>

class QString;

/**
   @brief Splits a container VRT's own raster (already one continuous, resolved grid)
          into a handful of large tiles for CVrtAdvisoryDialog's "Combine files..."
          action. Pure geometry from the VRT XML (raster grid + each source's <DstRect>);
          no pixel data is read, so even a huge VRT resolves in milliseconds.
 */
class CVrtCombiner {
 public:
  /// @brief One output tile's pixel window. row/col are its grid position (stable across
  ///        tightenToFootprints(), used for output naming).
  struct pixel_window_t {
    qint32 xoff = 0;
    qint32 yoff = 0;
    qint32 xsize = 0;
    qint32 ysize = 0;
    qint32 row = 0;
    qint32 col = 0;

    /// @brief All-zero sentinel: tightenToFootprints() found no source covering the window.
    bool empty() const { return xsize <= 0 || ysize <= 0; }
  };

  /// @brief A source's rectangle in the container's pixel grid, from its <DstRect>.
  ///        Half-open: covers [xoff, xoff+xsize) x [yoff, yoff+ysize).
  struct rect_t {
    qint32 xoff = 0;
    qint32 yoff = 0;
    qint32 xsize = 0;
    qint32 ysize = 0;
  };

  /// @brief The container's native raster grid plus every source footprint, from the XML.
  ///        rasterWidth/Height match GetXSize()/GetYSize() and gdal_translate's -srcwin.
  struct layout_t {
    qint32 rasterWidth = 0;
    qint32 rasterHeight = 0;
    QVector<rect_t> footprints;

    /// @brief False if the VRT couldn't be read/parsed or declared no raster grid.
    bool valid() const { return rasterWidth > 0 && rasterHeight > 0; }
  };

  /// @brief Hard cap on output tiles - stays under kMaxSubfileCount (50). Placeholder,
  ///        needs tuning against a real large VRT.
  static constexpr qint32 kMaxOutputTiles = 40;

  /// @brief Target pixel count per output tile. Placeholder, needs tuning.
  static constexpr qint64 kMaxPixelsPerTile = 150000000;

  /// @brief Parse a VRT into its raster grid and all source footprints. Invalid layout_t
  ///        if it can't be read. A VRT with a grid but no <DstRect> (sources default to the
  ///        full raster per spec) yields one full-raster footprint, so no cell is dropped.
  static layout_t readVrtLayout(const QString& vrtPath);

  /// @brief Divide a rasterWidth x rasterHeight raster into an aspect-ratio-proportional
  ///        grid targeting kMaxPixelsPerTile pixels/cell, capped at kMaxOutputTiles cells.
  static QVector<pixel_window_t> computeGrid(qint32 rasterWidth, qint32 rasterHeight);

  /// @brief Crop window to the bbox of the footprints overlapping it, or empty() if none
  ///        does (a cell over a coverage gap - caller drops it). Trims the nodata border a
  ///        uniform grid cell would include, without reading pixels.
  static pixel_window_t tightenToFootprints(const QVector<rect_t>& footprints, const pixel_window_t& window);
};

#endif  // CVRTCOMBINER_H
