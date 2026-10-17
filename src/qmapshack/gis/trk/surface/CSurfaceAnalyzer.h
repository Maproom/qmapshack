/**********************************************************************************************
    Copyright (C) 2026 Gert Pellin <gert@pellin.be>

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

#ifndef CSURFACEANALYZER_H
#define CSURFACEANALYZER_H

#include <QSpan>
#include <QVector>
#include <array>
#include <atomic>
#include <functional>
#include <optional>

#include "gis/trk/surface/CSurface.h"

/**
   @brief Surface and way type along a track

   The track is sampled about every `sampleDistance` meters. Each sample gets the way found at
   its position (see CSurfaceDbSet::lookup()) and stands for the stretch up to the next sample.
   A leg between two track points is therefore not one classification: perPoint holds the one at
   the start of the leg, changes the ones along it.
   A single sample that differs from two equal neighbours is taken for a crossing and smoothed
   away. The lengths are summed per surface class, way type and detailed surface.
 */
class CSurfaceAnalyzer {
 public:
  struct point_t {
    qreal lat = 0;
    qreal lon = 0;
    bool newSegment = false; /**< true for the first point of a track segment: no distance to the previous one */
  };

  struct options_t {
    qreal sampleDistance = 20;   /**< 0: use the track points only, as the reference implementation does */
    qreal fallbackDistance = 25; /**< nearest way within this distance if no way passes the cell */
    bool smooth = true;
  };

  using fLookup = std::function<std::optional<CSurface::info_t>(qreal lat, qreal lon)>;

  /** @brief A change of the classification within a leg */
  struct change_t {
    qint32 idxPoint;       /**< the leg from this input point to the next one */
    qreal fraction;        /**< where on the leg, 0..1 */
    CSurface::info_t info; /**< the classification from there on */
  };

  struct result_t {
    bool valid = false;                 /**< false if canceled */
    QVector<CSurface::info_t> perPoint; /**< one entry per input point, valid from it to the first change */
    QVector<change_t> changes;          /**< in track order, so the legs can be drawn as they are counted */
    qreal total = 0;                    /**< meters */
    std::array<qreal, CSurface::eClassCount> byClass{};
    std::array<qreal, CSurface::eWayCount> byWay{};
    std::array<qreal, CSurface::eLabelCount> byLabel{};
    qint32 samples = 0;
    qint32 samplesWithData = 0;

    /** @brief share of a class in percent */
    qreal percent(CSurface::class_e cls) const { return total > 0 ? 100.0 * byClass[cls] / total : 0; }
    /** @brief the changes on the leg from input point @p idxPoint to the next one */
    QSpan<const change_t> changesOf(qint32 idxPoint) const;
  };

  static result_t analyze(const QVector<point_t>& points, const fLookup& lookup, const options_t& options,
                          const std::atomic_bool* cancel = nullptr);
};

#endif  // CSURFACEANALYZER_H
