/**********************************************************************************************
    Copyright (C) 2023 Gunnar Skjold <gunnar.skjold@gmail.com>

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

#include "realtime/ais/CRtAisRecord.h"

CRtAisRecord::CRtAisRecord(QObject* parent) : IRtRecord(parent) {}

bool CRtAisRecord::writeEntry(const CRtAis::ship_t& ship) {
  QByteArray data;
  QDataStream stream(&data, QIODevice::WriteOnly);
  stream.setVersion(QDataStream::Qt_5_2);
  stream.setByteOrder(QDataStream::LittleEndian);

  // it's always a good idea to start with a version tag for future changes.
  stream << quint8(1);

  CTrackData::trkpt_t trkpt;
  trkpt.lon = ship.longitude;
  trkpt.lat = ship.latitude;
  trkpt.ele = 0;
  trkpt.time = QDateTime::fromSecsSinceEpoch(ship.timePosition);

  stream << trkpt;
  track << trkpt;

  return writeEntry(data);
}
