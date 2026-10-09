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

#ifndef CSURFACEDBSET_H
#define CSURFACEDBSET_H

#include <QList>
#include <QRectF>
#include <QStringList>
#include <optional>

#include "gis/trk/surface/CSurface.h"

class CSurfaceDb;

/**
   @brief All active surface databases, used together

   Like CSurfaceDb, an object must stay in the thread it was created in.
 */
class CSurfaceDbSet {
 public:
  explicit CSurfaceDbSet(const QStringList& files);
  ~CSurfaceDbSet();

  /// true if any database's area intersects the rectangle (x = longitude, y = latitude)
  bool covers(const QRectF& area) const;
  bool isEmpty() const { return dbs.isEmpty(); }

  /**
     @brief The way at a position

     First the cell majority (CSurfaceDb::lookupCell()) of the first database containing the
     position; if no way passes the cell, the nearest way within fallbackDist meters.
   */
  std::optional<CSurface::info_t> lookup(qreal lat, qreal lon, qreal fallbackDist);

 private:
  QList<CSurfaceDb*> dbs;
};

#endif  // CSURFACEDBSET_H
