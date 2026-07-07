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

#include "helpers/CVrtCombiner.h"

#include <QDomDocument>
#include <QFile>

#include <algorithm>
#include <cmath>

CVrtCombiner::layout_t CVrtCombiner::readVrtLayout(const QString& vrtPath) {
  layout_t layout;

  QFile file(vrtPath);
  if (!file.open(QIODevice::ReadOnly)) {
    return layout;
  }
  QDomDocument doc;
  if (!doc.setContent(&file)) {
    return layout;
  }
  file.close();

  // <VRTDataset rasterXSize=".." rasterYSize=".."> - the grid every <DstRect> (and
  // gdal_translate's -srcwin) is expressed in.
  const QDomElement root = doc.documentElement();
  layout.rasterWidth = root.attribute("rasterXSize").toInt();
  layout.rasterHeight = root.attribute("rasterYSize").toInt();

  // Every <DstRect>, across all bands: a multi-band VRT repeats sources per band, but
  // duplicate footprints don't change the union/overlap tests in tightenToFootprints().
  const QDomNodeList rects = doc.elementsByTagName("DstRect");
  for (int i = 0; i < rects.count(); ++i) {
    const QDomElement e = rects.at(i).toElement();
    // xOff/xSize are doubles in the spec; round outward to fully enclose the source pixels.
    const double x = e.attribute("xOff").toDouble();
    const double y = e.attribute("yOff").toDouble();
    const double w = e.attribute("xSize").toDouble();
    const double h = e.attribute("ySize").toDouble();
    if (w <= 0.0 || h <= 0.0) {
      continue;
    }
    const qint32 x0 = static_cast<qint32>(std::floor(x));
    const qint32 y0 = static_cast<qint32>(std::floor(y));
    const qint32 x1 = static_cast<qint32>(std::ceil(x + w));
    const qint32 y1 = static_cast<qint32>(std::ceil(y + h));
    layout.footprints << rect_t{x0, y0, x1 - x0, y1 - y0};
  }

  // Safety net: no <DstRect> means each source defaults to the full raster (spec), so treat
  // the whole raster as covered rather than dropping every cell. gdalbuildvrt always writes
  // DstRects, so this only guards hand-written VRTs.
  if (layout.footprints.isEmpty() && layout.valid()) {
    layout.footprints << rect_t{0, 0, layout.rasterWidth, layout.rasterHeight};
  }
  return layout;
}

QVector<CVrtCombiner::pixel_window_t> CVrtCombiner::computeGrid(qint32 rasterWidth, qint32 rasterHeight) {
  const qint64 totalPixels = static_cast<qint64>(rasterWidth) * rasterHeight;
  const qint32 numTiles = std::clamp<qint32>(
      static_cast<qint32>((totalPixels + kMaxPixelsPerTile - 1) / kMaxPixelsPerTile), 1, kMaxOutputTiles);

  // Rows/cols proportional to the raster's aspect ratio, for squarish cells.
  const qint32 rows = std::clamp<qint32>(
      static_cast<qint32>(std::lround(
          std::sqrt(static_cast<double>(numTiles) * rasterHeight / rasterWidth))),
      1, numTiles);
  // Floor division: keeps rows * cols <= numTiles, so the cap can't be exceeded.
  const qint32 cols = std::clamp<qint32>(numTiles / rows, 1, numTiles);

  const qint32 cellW = (rasterWidth + cols - 1) / cols;
  const qint32 cellH = (rasterHeight + rows - 1) / rows;

  QVector<pixel_window_t> grid;
  for (qint32 r = 0; r < rows; ++r) {
    const qint32 yoff = r * cellH;
    if (yoff >= rasterHeight) {
      break;
    }
    const qint32 ysize = std::min(cellH, rasterHeight - yoff);
    for (qint32 c = 0; c < cols; ++c) {
      const qint32 xoff = c * cellW;
      if (xoff >= rasterWidth) {
        break;
      }
      const qint32 xsize = std::min(cellW, rasterWidth - xoff);
      grid << pixel_window_t{xoff, yoff, xsize, ysize, r, c};
    }
  }
  return grid;
}

CVrtCombiner::pixel_window_t CVrtCombiner::tightenToFootprints(const QVector<rect_t>& footprints,
                                                              const pixel_window_t& window) {
  const qint32 cellX0 = window.xoff;
  const qint32 cellY0 = window.yoff;
  const qint32 cellX1 = window.xoff + window.xsize;
  const qint32 cellY1 = window.yoff + window.ysize;

  // Bbox of every footprint-vs-cell overlap (all half-open ranges).
  qint32 minX = cellX1;
  qint32 minY = cellY1;
  qint32 maxX = cellX0;
  qint32 maxY = cellY0;
  bool any = false;
  for (const rect_t& fp : footprints) {
    const qint32 ix0 = std::max(cellX0, fp.xoff);
    const qint32 iy0 = std::max(cellY0, fp.yoff);
    const qint32 ix1 = std::min(cellX1, fp.xoff + fp.xsize);
    const qint32 iy1 = std::min(cellY1, fp.yoff + fp.ysize);
    if (ix0 < ix1 && iy0 < iy1) {
      any = true;
      minX = std::min(minX, ix0);
      minY = std::min(minY, iy0);
      maxX = std::max(maxX, ix1);
      maxY = std::max(maxY, iy1);
    }
  }
  if (!any) {
    return pixel_window_t{};  // no source covers this cell - caller drops it
  }
  return pixel_window_t{minX, minY, maxX - minX, maxY - minY, window.row, window.col};
}
