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
#include <QObject>
#include <memory>

#include "gis/trk/surface/CSurfaceAnalyzer.h"

class CGisItemTrk;
class CTrackData;

/**
   @brief The surface analysis of one track

   Owned by the track. CGisItemTrk::deriveSecondaryData() calls update() on every change. If
   the visible points are the same as for the last result, it is applied again at once.
   Otherwise an analysis starts in the background; when it is done the track's visuals are
   updated. Nothing of this is saved with the track.
 */
class CSurfaceTrk : public QObject {
  Q_OBJECT
 public:
  enum state_e {
    eStateNoTrack,     ///< no visible points
    eStateNoDatabase,  ///< no active surface database covers the track
    eStateAnalyzing,   ///< waiting for the background job
    eStateReady        ///< getResult() is valid
  };

  explicit CSurfaceTrk(CGisItemTrk& trk);
  ~CSurfaceTrk() override;

  /// apply a known result to the points or start an analysis
  void update(CTrackData& data);

  state_e getState() const { return state; }
  const CSurfaceAnalyzer::result_t& getResult() const { return result; }

 private slots:
  void slotFinished();
  void slotDatabasesChanged();

 private:
  void start();
  void apply(CTrackData& data);

  CGisItemTrk& trk;
  state_e state = eStateNoTrack;

  QVector<CSurfaceAnalyzer::point_t> points;
  /// visible point index -> total index
  QVector<qint32> indices;
  size_t hashPoints = 0;
  size_t hashResult = 0;
  size_t hashRunning = 0;

  CSurfaceAnalyzer::result_t result;
  QFutureWatcher<CSurfaceAnalyzer::result_t>* watcher;
  std::shared_ptr<std::atomic_bool> cancel;
};

#endif  // CSURFACETRK_H
