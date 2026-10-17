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

#include <QThread>
#include <QtConcurrentRun>

#include "canvas/CCanvas.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/surface/CSurfaceDbManager.h"
#include "gis/trk/surface/CSurfaceDbSet.h"

CSurfaceTrk::CSurfaceTrk(CGisItemTrk& trk)
    : trk(trk), enabled(QThread::isMainThread()), state(enabled ? eStateNoTrack : eStateOff) {
  watcher = new QFutureWatcher<CSurfaceAnalyzer::result_t>(this);
  if (!enabled) {
    // the object belongs to that thread, which may end before the track does: nothing could reach it
    return;
  }
  connect(watcher, &QFutureWatcher<CSurfaceAnalyzer::result_t>::finished, this, &CSurfaceTrk::slotFinished);
  connect(&CSurfaceDbManager::self(), &CSurfaceDbManager::sigChanged, this, &CSurfaceTrk::slotDatabasesChanged);
  connect(&CSurfaceDbManager::self(), &CSurfaceDbManager::sigQuit, this, &CSurfaceTrk::stop);
}

CSurfaceTrk::~CSurfaceTrk() { stop(); }

void CSurfaceTrk::stop() {
  if (cancel) {
    cancel->store(true);
  }
  hashRunning = 0;
}

void CSurfaceTrk::update(CTrackData& data) {
  if (!enabled) {
    clear(data);
    return;
  }
  Q_ASSERT(QThread::isMainThread());

  QVector<CSurfaceAnalyzer::point_t> points;
  indices.clear();
  area = QRectF();
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

  coverage = CSurfaceDbManager::self().getCoverage(area);
  size_t hash = coverage.key;
  for (const CSurfaceAnalyzer::point_t& pt : std::as_const(points)) {
    hash = qHashMulti(hash, pt.lat, pt.lon, pt.newSegment);
  }
  hashPoints = hash;

  if (points.isEmpty()) {
    state = eStateNoTrack;
    clear(data);
    stop();
    return;
  }

  if (coverage.files.isEmpty()) {
    state = eStateNoDatabase;
    clear(data);
    stop();
    return;
  }

  if (hashPoints == hashResult) {
    // e.g. undo: a job for other points still running is of no use any longer
    stop();
    state = eStateReady;
    apply(data);
    return;
  }

  state = eStateAnalyzing;
  // the points keep their old values meanwhile, but the changes along the legs belong to the old indices
  applied.clear();
  if (hashPoints != hashRunning) {
    start(points);
  }
}

void CSurfaceTrk::start(const QVector<CSurfaceAnalyzer::point_t>& points) {
  stop();
  cancel = std::make_shared<std::atomic_bool>(false);
  hashRunning = hashPoints;

  // only the copies go to the job, the track keeps no list of its own
  const QStringList files = coverage.files;
  const std::shared_ptr<std::atomic_bool> flag = cancel;
  QFuture<CSurfaceAnalyzer::result_t> future =
      QtConcurrent::run(&CSurfaceDbManager::self().getPool(),
                        [points, files, flag]() { return CSurfaceDbSet::analyze(files, points, flag.get()); });
  // the input the result will belong to
  watcher->setProperty("hash", QVariant::fromValue(quint64(hashRunning)));
  watcher->setFuture(future);
}

void CSurfaceTrk::slotFinished() {
  const CSurfaceAnalyzer::result_t res = watcher->result();
  const size_t hash = size_t(watcher->property("hash").toULongLong());
  if (hash == hashRunning) {
    // whatever the outcome, this job is done: the same points again need a new one
    hashRunning = 0;
  }
  if (!res.valid || hash != hashPoints) {
    return;  // canceled or outdated
  }

  result = res;
  hashResult = hash;
  state = eStateReady;
  trk.updateSurface();
}

void CSurfaceTrk::slotDatabasesChanged() {
  // a track the change does not touch keeps its result: no job, nothing to redraw
  if (state != eStateNoTrack && CSurfaceDbManager::self().getCoverage(area).key == coverage.key) {
    return;
  }
  trk.updateSurface();
}

QSpan<const CSurfaceAnalyzer::change_t> CSurfaceTrk::getChanges(qint32 idxTotal) const {
  const auto it = applied.constFind(idxTotal);
  return it == applied.constEnd() ? QSpan<const CSurfaceAnalyzer::change_t>() : result.changesOf(*it);
}

void CSurfaceTrk::clear(CTrackData& data) {
  applied.clear();
  for (CTrackData::trkpt_t& pt : data) {
    pt.surfaceClass = -1;
    pt.surfaceWay = -1;
    pt.surfaceLabel = -1;
  }
}

void CSurfaceTrk::apply(CTrackData& data) {
  applied.clear();
  if (result.perPoint.size() != indices.size()) {
    return;
  }

  for (qint32 i = 0; i < indices.size(); i++) {
    applied[indices[i]] = i;
  }

  for (CTrackData::trkpt_t& pt : data) {
    auto it = applied.constFind(pt.idxTotal);
    if (it == applied.constEnd()) {
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
