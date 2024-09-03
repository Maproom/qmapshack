/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "realtime/IRtRecord.h"

#include <QtCore>

#include "realtime/CRtDraw.h"

IRtRecord::IRtRecord(QObject* parent) : QObject(parent) {}

bool IRtRecord::setFile(const QString& fn) {
  track.clear();
  filename = fn;

  if (QFile::exists(filename)) {
    return readFile(filename);
  }
  return true;
}

bool IRtRecord::readFile(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    error = tr("Failed to open record for reading.");
    return false;
  }

  QDataStream stream(&file);
  stream.setVersion(QDataStream::Qt_5_2);
  stream.setByteOrder(QDataStream::LittleEndian);

  while (!stream.atEnd()) {
    quint64 size = stream.device()->pos();

    quint16 crc;
    QByteArray data;
    stream >> crc >> data;

    if ((qChecksum(data.data(), data.size()) != crc) || (stream.status() != QDataStream::Ok)) {
      error = tr("Failed to read entry. Truncate record to last valid entry.");
      file.close();
      QFile::resize(filename, size);
      return false;
    }

    readEntry(data);
  }

  file.close();
  return true;
}

bool IRtRecord::writeEntry(const QByteArray& data) {
  QFile file(filename);
  if (!file.open(QIODevice::Append)) {
    error = tr("Failed to open record for writing.");
    return false;
  }

  QDataStream stream(&file);
  stream.setVersion(QDataStream::Qt_5_2);
  stream.setByteOrder(QDataStream::LittleEndian);

  quint16 crc = qChecksum(data.data(), data.size());
  stream << crc << data;

  if (stream.status() != QDataStream::Ok) {
    error = tr("Failed to write entry.");
    file.close();
    return false;
  }

  file.close();
  return true;
}

bool IRtRecord::readEntry(QByteArray& data) {
  QDataStream stream(&data, QIODevice::ReadOnly);
  stream.setVersion(QDataStream::Qt_5_2);
  stream.setByteOrder(QDataStream::LittleEndian);

  quint8 version;
  stream >> version;

  CTrackData::trkpt_t trkpt;
  stream >> trkpt;
  track << trkpt;
  return true;
}

void IRtRecord::reset() {
  track.clear();
  QFile::resize(filename, 0);
}

void IRtRecord::draw(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw* rt) {
  QPolygonF tmp;
  for (const CTrackData::trkpt_t& trkpt : std::as_const(track)) {
    tmp << QPointF(trkpt.lon * DEG_TO_RAD, trkpt.lat * DEG_TO_RAD);
  }

  rt->convertRad2Px(tmp);
  p.setPen(QPen(Qt::black, 3));
  p.drawPolyline(tmp);
}
