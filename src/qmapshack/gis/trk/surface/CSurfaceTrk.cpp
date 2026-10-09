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

#include "gis/trk/surface/CSurfaceTrk.h"

#include <QApplication>
#include <QThreadPool>
#include <QtConcurrentRun>

#include "canvas/CCanvas.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/surface/CSurfaceDbManager.h"
#include "gis/trk/surface/CSurfaceDbSet.h"

static QThreadPool& pool() {
  // each job opens its own database connections, a few in parallel are plenty
  static QThreadPool* pool = [] {
    QThreadPool* p = new QThreadPool();
    p->setMaxThreadCount(2);
    return p;
  }();
  return *pool;
}

CSurfaceTrk::CSurfaceTrk(CGisItemTrk& trk) : trk(trk) {
  if (thread() != qApp->thread()) {
    moveToThread(qApp->thread());
  }
  watcher = new QFutureWatcher<CSurfaceAnalyzer::result_t>(this);
  connect(watcher, &QFutureWatcher<CSurfaceAnalyzer::result_t>::finished, this, &CSurfaceTrk::slotFinished);
  connect(&CSurfaceDbManager::self(), &CSurfaceDbManager::sigChanged, this, &CSurfaceTrk::slotDatabasesChanged);
}

CSurfaceTrk::~CSurfaceTrk() {
  if (cancel) {
    cancel->store(true);
  }
}

void CSurfaceTrk::update(CTrackData& data) {
  points.clear();
  indices.clear();
  QRectF area;
  for (const CTrackData::trkseg_t& seg : std::as_const(data.segs)) {
    bool newSegment = true;
    for (const CTrackData::trkpt_t& pt : seg.pts) {
      if (pt.isHidden()) {
        continue;
      }
      points << CSurfaceAnalyzer::point_t{pt.lat, pt.lon, newSegment};
      indices << pt.idxTotal;
      area |= QRectF(pt.lon, pt.lat, 1e-9, 1e-9);
      newSegment = false;
    }
  }

  size_t hash = qHash(CSurfaceDbManager::self().getGeneration());
  for (const CSurfaceAnalyzer::point_t& pt : std::as_const(points)) {
    hash = qHashMulti(hash, pt.lat, pt.lon, pt.newSegment);
  }
  hashPoints = hash;

  if (points.isEmpty()) {
    state = eStateNoTrack;
    return;
  }

  if (!CSurfaceDbManager::self().covers(area)) {
    state = eStateNoDatabase;
    for (CTrackData::trkpt_t& pt : data) {
      pt.surfaceClass = -1;
      pt.surfaceWay = -1;
      pt.surfaceLabel = -1;
    }
    if (cancel) {
      cancel->store(true);
    }
    hashRunning = 0;
    return;
  }

  if (hashPoints == hashResult) {
    state = eStateReady;
    apply(data);
    return;
  }

  state = eStateAnalyzing;
  if (hashPoints != hashRunning) {
    // the track may live in another thread while it is loaded
    QMetaObject::invokeMethod(this, &CSurfaceTrk::start, Qt::QueuedConnection);
    hashRunning = hashPoints;
  }
}

void CSurfaceTrk::start() {
  if (cancel) {
    cancel->store(true);
  }
  cancel = std::make_shared<std::atomic_bool>(false);

  const QVector<CSurfaceAnalyzer::point_t> pts = points;
  const QStringList files = CSurfaceDbManager::self().getActiveFiles();
  const std::shared_ptr<std::atomic_bool> flag = cancel;
  QFuture<CSurfaceAnalyzer::result_t> future = QtConcurrent::run(&pool(), [pts, files, flag]() {
    CSurfaceDbSet set(files);
    const CSurfaceAnalyzer::options_t opts;
    return CSurfaceAnalyzer::analyze(
        pts, [&](qreal lat, qreal lon) { return set.lookup(lat, lon, opts.fallbackDistance); }, opts, flag.get());
  });
  // the input the result will belong to
  watcher->setProperty("hash", QVariant::fromValue(quint64(hashRunning)));
  watcher->setFuture(future);
}

void CSurfaceTrk::slotFinished() {
  const CSurfaceAnalyzer::result_t res = watcher->result();
  const size_t hash = size_t(watcher->property("hash").toULongLong());
  if (!res.valid || hash != hashRunning || hash != hashPoints) {
    return;  // canceled or outdated, a newer job is on its way
  }

  result = res;
  hashResult = hash;
  hashRunning = 0;
  state = eStateReady;
  trk.updateSurface();
}

void CSurfaceTrk::slotDatabasesChanged() {
  hashResult = 0;
  hashRunning = 0;
  trk.updateSurface();
}

void CSurfaceTrk::apply(CTrackData& data) {
  if (result.perPoint.size() != indices.size()) {
    return;
  }

  QHash<qint32, qint32> visible;
  for (qint32 i = 0; i < indices.size(); i++) {
    visible[indices[i]] = i;
  }

  for (CTrackData::trkpt_t& pt : data) {
    auto it = visible.constFind(pt.idxTotal);
    if (it == visible.constEnd()) {
      pt.surfaceClass = -1;
      pt.surfaceWay = -1;
      pt.surfaceLabel = -1;
      continue;
    }
    const CSurface::info_t& info = result.perPoint[*it];
    pt.surfaceClass = info.cls;
    pt.surfaceWay = info.way;
    pt.surfaceLabel = info.label;
  }
}
