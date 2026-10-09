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

#include "gis/trk/surface/CSurfaceDbSet.h"

#include <QDebug>

#include "gis/trk/surface/CSurfaceDb.h"

CSurfaceDbSet::CSurfaceDbSet(const QStringList& files) {
  for (const QString& file : files) {
    CSurfaceDb* db = new CSurfaceDb(file);
    if (db->isValid()) {
      dbs << db;
    } else {
      qWarning() << "Surface database" << file << db->errorString();
      delete db;
    }
  }
}

CSurfaceDbSet::~CSurfaceDbSet() { qDeleteAll(dbs); }

bool CSurfaceDbSet::covers(const QRectF& area) const {
  for (const CSurfaceDb* db : dbs) {
    if (db->boundingBox().intersects(area) || db->boundingBox().contains(area.center())) {
      return true;
    }
  }
  return false;
}

std::optional<CSurface::info_t> CSurfaceDbSet::lookup(qreal lat, qreal lon, qreal fallbackDist) {
  // a little kSlack: a way just outside the data's box can still pass the cell
  constexpr qreal kSlack = 0.001;
  QList<CSurfaceDb*> candidates;
  for (CSurfaceDb* db : std::as_const(dbs)) {
    if (db->boundingBox().adjusted(-kSlack, -kSlack, kSlack, kSlack).contains(QPointF(lon, lat))) {
      candidates << db;
    }
  }

  for (CSurfaceDb* db : std::as_const(candidates)) {
    std::optional<CSurface::info_t> info = db->lookupCell(lat, lon);
    if (info) {
      return info;
    }
  }

  for (CSurfaceDb* db : std::as_const(candidates)) {
    std::optional<CSurface::info_t> info = db->lookupNearest(lat, lon, fallbackDist);
    if (info) {
      return info;
    }
  }

  return std::nullopt;
}
