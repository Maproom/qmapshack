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

#ifndef CSURFACETRK_H
#define CSURFACETRK_H

#include <QFutureWatcher>
#include <QHash>
#include <QObject>
#include <memory>

#include "gis/trk/surface/CSurfaceAnalyzer.h"
#include "gis/trk/surface/CSurfaceDbManager.h"

class CGisItemTrk;
class CTrackData;

/**
   @brief The surface analysis of one track

   Owned by the track. CGisItemTrk::deriveSecondaryData() calls update() on every change. If
   the visible points are the same as for the last result, it is applied again at once.
   Otherwise an analysis starts in the background; when it is done the track's visuals are
   updated. Nothing of this is saved with the track.

   Everything happens in the GUI thread, which also owns CSurfaceDbManager; only the
   analysis itself runs in a pool thread, on copies. A track created in another thread
   (e.g. by a database export or loaded from a device) does not analyze its surfaces at all,
   a copy of it in a project does.
 */
class CSurfaceTrk : public QObject {
  Q_OBJECT
 public:
  enum state_e {
    eStateOff,        /**< created outside the GUI thread */
    eStateNoTrack,    /**< no visible points */
    eStateNoDatabase, /**< no active surface database covers the track */
    eStateAnalyzing,  /**< waiting for the background job */
    eStateReady       /**< getResult() is valid */
  };

  explicit CSurfaceTrk(CGisItemTrk& trk);
  ~CSurfaceTrk() override;

  /** @brief apply a known result to the points or start an analysis */
  void update(CTrackData& data);

  state_e getState() const { return state; }
  /** @brief a database switched off covers the track */
  bool isSwitchedOff() const { return coverage.switchedOff; }
  const CSurfaceAnalyzer::result_t& getResult() const { return result; }
  /** @brief the changes of the classification on the leg starting at a track point, by its total index */
  QSpan<const CSurfaceAnalyzer::change_t> getChanges(qint32 idxTotal) const;

 private slots:
  void slotFinished();
  void slotDatabasesChanged();

 private:
  void start(const QVector<CSurfaceAnalyzer::point_t>& points);
  /** @brief cancel a running job */
  void stop();
  void apply(CTrackData& data);
  void clear(CTrackData& data);

  CGisItemTrk& trk;
  /** @brief false if created outside the GUI thread */
  const bool enabled;
  state_e state;

  /** @brief the area of the visible points, x = longitude, y = latitude */
  QRectF area;
  CSurfaceDbManager::coverage_t coverage;
  /** @brief visible point index -> total index */
  QVector<qint32> indices;
  size_t hashPoints = 0;
  size_t hashResult = 0;
  size_t hashRunning = 0;

  CSurfaceAnalyzer::result_t result;
  /** @brief total index -> index in the result, for the points the result is applied to */
  QHash<qint32, qint32> applied;
  QFutureWatcher<CSurfaceAnalyzer::result_t>* watcher;
  std::shared_ptr<std::atomic_bool> cancel;
};

#endif  // CSURFACETRK_H
