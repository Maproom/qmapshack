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

#include "gis/trk/surface/CSurfaceAnalyzer.h"

#include <QtMath>
#include <algorithm>

#include "gis/trk/surface/CSurfaceDb.h"

namespace {
struct sample_t {
  qint32 idxPoint; /**< the input point the sample belongs to (its start) */
  bool atPoint;    /**< the sample lies on the input point itself */
  qreal fraction;  /**< position on the leg to the next point, 0..1 */
  qreal length;
  CSurface::info_t info;
};
}  // namespace

QSpan<const CSurfaceAnalyzer::change_t> CSurfaceAnalyzer::result_t::changesOf(qint32 idxPoint) const {
  auto byPoint = [](const change_t& change, qint32 idx) { return change.idxPoint < idx; };
  const auto first = std::lower_bound(changes.cbegin(), changes.cend(), idxPoint, byPoint);
  const auto last = std::lower_bound(first, changes.cend(), idxPoint + 1, byPoint);
  return QSpan<const change_t>(first, last);
}

CSurfaceAnalyzer::result_t CSurfaceAnalyzer::analyze(const QVector<point_t>& points, const fLookup& lookup,
                                                     const options_t& options, const std::atomic_bool* cancel) {
  result_t result;
  result.perPoint.fill(CSurface::noData(), points.size());

  auto canceled = [cancel]() { return cancel != nullptr && cancel->load(); };
  auto get = [&](qreal lat, qreal lon) {
    std::optional<CSurface::info_t> info = lookup(lat, lon);
    if (info) {
      result.samplesWithData++;
    }
    result.samples++;
    return info ? *info : CSurface::noData();
  };

  qint32 start = 0;
  while (start < points.size()) {
    // one track segment: [start, end)
    qint32 end = start + 1;
    while (end < points.size() && !points[end].newSegment) {
      end++;
    }

    QVector<sample_t> samples;
    for (qint32 i = start; i < end; i++) {
      if (canceled()) {
        return result;
      }
      const point_t& p1 = points[i];
      if (i + 1 == end) {
        samples << sample_t{i, true, 0, 0, get(p1.lat, p1.lon)};
        break;
      }

      const point_t& p2 = points[i + 1];
      const qreal d = CSurfaceDb::distance(p1.lat, p1.lon, p2.lat, p2.lon);
      const qint32 n = options.sampleDistance > 0 ? qMax(1, qCeil(d / options.sampleDistance)) : 1;
      for (qint32 k = 0; k < n; k++) {
        // a long leg has thousands of samples, each a database lookup
        if (k > 0 && canceled()) {
          return result;
        }
        const qreal f = qreal(k) / n;
        const qreal lat = p1.lat + (p2.lat - p1.lat) * f;
        const qreal lon = p1.lon + (p2.lon - p1.lon) * f;
        samples << sample_t{i, k == 0, f, d / n, get(lat, lon)};
      }
    }

    if (options.smooth && samples.size() > 2) {
      const QVector<sample_t> raw = samples;
      for (qsizetype s = 1; s + 1 < raw.size(); s++) {
        if (raw[s].info != raw[s - 1].info && raw[s - 1].info == raw[s + 1].info) {
          samples[s].info = raw[s - 1].info;
        }
      }
    }

    for (qsizetype s = 0; s < samples.size(); s++) {
      const sample_t& sample = samples[s];
      if (sample.atPoint) {
        result.perPoint[sample.idxPoint] = sample.info;
      } else if (sample.info != samples[s - 1].info) {
        result.changes << change_t{sample.idxPoint, sample.fraction, sample.info};
      }
      result.total += sample.length;
      result.byClass[sample.info.cls] += sample.length;
      result.byWay[sample.info.way] += sample.length;
      result.byLabel[sample.info.label] += sample.length;
    }

    start = end;
  }

  result.valid = true;
  return result;
}
