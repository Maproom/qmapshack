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

#include "gis/trk/surface/CPbfReader.h"

#include <cpl_conv.h>

#include <QtEndian>
#include <QtNumeric>
#include <limits>
#include <stdexcept>

namespace {
constexpr quint64 kMaxBlockSize = 64 * 1024 * 1024;
constexpr quint32 kMaxHeaderSize = 64 * 1024;

// The few protobuf primitives the OSM PBF format needs.
class CProtoBuf {
 public:
  CProtoBuf(const char* data, qsizetype size) : p(reinterpret_cast<const uchar*>(data)), end(p + size) {}
  explicit CProtoBuf(QByteArrayView view) : CProtoBuf(view.data(), view.size()) {}

  bool atEnd() const { return p >= end; }

  quint64 varint() {
    quint64 val = 0;
    for (qint32 shift = 0; shift < 64; shift += 7) {
      if (p >= end) {
        throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "truncated varint"));
      }
      const uchar b = *p++;
      val |= quint64(b & 0x7F) << shift;
      if ((b & 0x80) == 0) {
        return val;
      }
    }
    throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "varint too long"));
  }

  qint64 svarint() {
    const quint64 v = varint();
    return qint64(v >> 1) ^ -qint64(v & 1);
  }

  /** @brief read the next field key, false at the end of the message */
  bool next(quint32& field, quint32& wire) {
    if (atEnd()) {
      return false;
    }
    const quint64 key = varint();
    field = quint32(key >> 3);
    wire = quint32(key & 0x07);
    return true;
  }

  QByteArrayView bytes() {
    const quint64 len = varint();
    if (len > quint64(end - p)) {
      throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "truncated field"));
    }
    QByteArrayView view(reinterpret_cast<const char*>(p), qsizetype(len));
    p += len;
    return view;
  }

  void skip(quint32 wire) {
    switch (wire) {
      case 0:
        varint();
        break;
      case 1:
        advance(8);
        break;
      case 2:
        bytes();
        break;
      case 5:
        advance(4);
        break;
      default:
        throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "unsupported wire type"));
    }
  }

 private:
  void advance(qsizetype n) {
    if (n > end - p) {
      throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "truncated field"));
    }
    p += n;
  }

  const uchar* p;
  const uchar* end;
};

// a value too large for T must not wrap around, e.g. into a valid string index
template <typename T, bool zigzag>
T checked(CProtoBuf& buf) {
  if constexpr (zigzag) {
    return T(buf.svarint());
  } else {
    const quint64 value = buf.varint();
    if (value > quint64(std::numeric_limits<T>::max())) {
      throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "value out of range"));
    }
    return T(value);
  }
}

// repeated scalar fields may come packed (wire type 2) or one by one (wire type 0)
template <typename T, bool zigzag>
void readRepeated(CProtoBuf& buf, quint32 wire, QVector<T>& out) {
  if (wire == 2) {
    CProtoBuf packed(buf.bytes());
    while (!packed.atEnd()) {
      out << checked<T, zigzag>(packed);
    }
  } else if (wire == 0) {
    out << checked<T, zigzag>(buf);
  } else {
    buf.skip(wire);
  }
}

struct block_params_t {
  qint64 granularity = 100;
  qint64 latOffset = 0;
  qint64 lonOffset = 0;
};

/** @brief nanodegrees to 1e-7 degrees, truncating like libosmium; out of +-limit degrees is an error */
qint32 toE7(qint64 offset, qint64 granularity, qint64 value, qint32 limit) {
  qint64 nano;
  if (qMulOverflow(granularity, value, &nano) || qAddOverflow(offset, nano, &nano) ||
      qAbs(nano / 100) > qint64(limit) * 10000000) {
    throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "coordinate out of range"));
  }
  return qint32(nano / 100);
}

qint32 latE7(const block_params_t& params, qint64 value) {
  return toE7(params.latOffset, params.granularity, value, 90);
}

qint32 lonE7(const block_params_t& params, qint64 value) {
  return toE7(params.lonOffset, params.granularity, value, 180);
}

void parseNode(QByteArrayView data, const block_params_t& params, const CPbfReader::fNode& onNode) {
  CProtoBuf buf(data);
  quint32 field, wire;
  qint64 id = 0, lat = 0, lon = 0;
  while (buf.next(field, wire)) {
    if (field == 1 && wire == 0) {
      id = buf.svarint();
    } else if (field == 8 && wire == 0) {
      lat = buf.svarint();
    } else if (field == 9 && wire == 0) {
      lon = buf.svarint();
    } else {
      buf.skip(wire);
    }
  }
  onNode(id, latE7(params, lat), lonE7(params, lon));
}

void parseDenseNodes(QByteArrayView data, const block_params_t& params, const CPbfReader::fNode& onNode) {
  CProtoBuf buf(data);
  quint32 field, wire;
  QVector<qint64> ids, lats, lons;
  while (buf.next(field, wire)) {
    if (field == 1) {
      readRepeated<qint64, true>(buf, wire, ids);
    } else if (field == 8) {
      readRepeated<qint64, true>(buf, wire, lats);
    } else if (field == 9) {
      readRepeated<qint64, true>(buf, wire, lons);
    } else {
      buf.skip(wire);
    }
  }
  if (ids.size() != lats.size() || ids.size() != lons.size()) {
    throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "inconsistent dense nodes"));
  }

  qint64 id = 0, lat = 0, lon = 0;
  for (qsizetype i = 0; i < ids.size(); i++) {
    id += ids[i];
    lat += lats[i];
    lon += lons[i];
    onNode(id, latE7(params, lat), lonE7(params, lon));
  }
}

void parseWay(QByteArrayView data, CPbfReader::way_t& way) {
  CProtoBuf buf(data);
  quint32 field, wire;
  way.id = 0;
  way.keys.clear();
  way.vals.clear();
  way.refs.clear();
  while (buf.next(field, wire)) {
    if (field == 1 && wire == 0) {
      way.id = qint64(buf.varint());
    } else if (field == 2) {
      readRepeated<quint32, false>(buf, wire, way.keys);
    } else if (field == 3) {
      readRepeated<quint32, false>(buf, wire, way.vals);
    } else if (field == 8) {
      readRepeated<qint64, true>(buf, wire, way.refs);
    } else {
      buf.skip(wire);
    }
  }
  if (way.keys.size() != way.vals.size()) {
    throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "inconsistent way tags"));
  }
  // refs are delta coded
  for (qsizetype i = 1; i < way.refs.size(); i++) {
    way.refs[i] += way.refs[i - 1];
  }
}
}  // namespace

CPbfReader::CPbfReader(const QString& filename) : file(filename) {}

bool CPbfReader::open() {
  if (!file.open(QIODevice::ReadOnly)) {
    error = file.errorString();
    return false;
  }

  QByteArray type, data;
  if (!nextBlob(type, data)) {
    if (error.isEmpty()) {
      error = tr("The file is empty.");
    }
    return false;
  }
  if (type != "OSMHeader") {
    error = tr("Not an OSM PBF file: first block is '%1'").arg(QString::fromUtf8(type));
    return false;
  }

  try {
    parseHeaderBlock(data, hdr);
  } catch (const std::exception& e) {
    error = tr("Broken OSM header: %1").arg(tr(e.what()));
    return false;
  }

  // A history file (.osh.pbf) holds every version of a way, deleted ones included. Its ways
  // can't be used without sorting out the versions, so it is refused instead.
  if (hdr.requiredFeatures.contains("HistoricalInformation")) {
    error = tr("This is an OSM history file (.osh.pbf), it is not supported. Please use a regular extract (.osm.pbf).");
    return false;
  }

  // LocationsOnWays is not listed: those files may lack the nodes the locations are read from
  static const QStringList supported = {"OsmSchema-V0.6", "DenseNodes", "Sort.Type_then_ID"};
  for (const QString& feature : std::as_const(hdr.requiredFeatures)) {
    if (!supported.contains(feature)) {
      error = tr("Unsupported PBF feature: %1").arg(feature);
      return false;
    }
  }

  posData = file.pos();
  return true;
}

bool CPbfReader::nextBlob(QByteArray& type, QByteArray& data) {
  type.clear();
  data.clear();

  const QByteArray len = file.read(4);
  if (len.isEmpty()) {
    if (file.error() != QFileDevice::NoError) {
      error = tr("Failed to read at offset %1: %2").arg(file.pos()).arg(file.errorString());
    }
    return false;  // else the regular end of file
  }
  try {
    if (len.size() != 4) {
      throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "truncated block header"));
    }
    const quint32 sizeHeader = qFromBigEndian<quint32>(len.constData());
    if (sizeHeader > kMaxHeaderSize) {
      throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "block header too large"));
    }
    const QByteArray header = file.read(sizeHeader);
    if (header.size() != qsizetype(sizeHeader)) {
      throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "truncated block header"));
    }

    quint64 sizeData = 0;
    CProtoBuf bufHeader(header);
    quint32 field, wire;
    while (bufHeader.next(field, wire)) {
      if (field == 1 && wire == 2) {
        type = bufHeader.bytes().toByteArray();
      } else if (field == 3 && wire == 0) {
        sizeData = bufHeader.varint();
      } else {
        bufHeader.skip(wire);
      }
    }
    if (sizeData > kMaxBlockSize) {
      throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "block too large"));
    }

    const QByteArray blob = file.read(qint64(sizeData));
    if (blob.size() != qsizetype(sizeData)) {
      throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "truncated block"));
    }

    QByteArrayView raw, zlib;
    quint64 rawSize = 0;
    bool otherCompression = false;
    CProtoBuf bufBlob(blob);
    while (bufBlob.next(field, wire)) {
      if (field == 1 && wire == 2) {
        raw = bufBlob.bytes();
      } else if (field == 2 && wire == 0) {
        rawSize = bufBlob.varint();
      } else if (field == 3 && wire == 2) {
        zlib = bufBlob.bytes();
      } else if (field >= 4 && field <= 7) {
        otherCompression = true;
        bufBlob.skip(wire);
      } else {
        bufBlob.skip(wire);
      }
    }

    if (!raw.isNull()) {
      data = raw.toByteArray();
    } else if (!zlib.isNull()) {
      if (rawSize > kMaxBlockSize) {
        throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "block too large"));
      }
      if (!inflate(zlib, qsizetype(rawSize), data)) {
        throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "zlib decompression failed"));
      }
    } else if (otherCompression) {
      throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "unsupported compression (only zlib is supported)"));
    }
  } catch (const std::exception& e) {
    error = tr("Broken PBF file at offset %1: %2").arg(file.pos()).arg(tr(e.what()));
    return false;
  }

  return true;
}

bool CPbfReader::inflate(QByteArrayView zlib, qsizetype size, QByteArray& out) {
  // qUncompress() takes the size as a hint only and grows its buffer for as long as the data goes on
  out = QByteArray(size, Qt::Uninitialized);
  size_t written = 0;
  if (CPLZLibInflateEx(zlib.data(), size_t(zlib.size()), out.data(), size_t(size), false, &written) == nullptr ||
      written != size_t(size)) {
    out.clear();
    return false;
  }
  return true;
}

void CPbfReader::parseHeaderBlock(const QByteArray& data, header_t& hdr) {
  CProtoBuf buf(data);
  quint32 field, wire;
  while (buf.next(field, wire)) {
    if (field == 1 && wire == 2) {
      CProtoBuf bbox(buf.bytes());
      quint32 f, w;
      while (bbox.next(f, w)) {
        if (w != 0) {
          bbox.skip(w);
          continue;
        }
        const qreal deg = bbox.svarint() * 1e-9;
        switch (f) {
          case 1:
            hdr.left = deg;
            break;
          case 2:
            hdr.right = deg;
            break;
          case 3:
            hdr.top = deg;
            break;
          case 4:
            hdr.bottom = deg;
            break;
          default:
            break;
        }
      }
      hdr.hasBBox = true;
    } else if (field == 4 && wire == 2) {
      hdr.requiredFeatures << QString::fromUtf8(buf.bytes());
    } else if (field == 5 && wire == 2) {
      hdr.optionalFeatures << QString::fromUtf8(buf.bytes());
    } else if (field == 16 && wire == 2) {
      hdr.writingProgram = QString::fromUtf8(buf.bytes());
    } else if (field == 17 && wire == 2) {
      hdr.source = QString::fromUtf8(buf.bytes());
    } else if (field == 32 && wire == 0) {
      hdr.timestamp = qint64(buf.varint());
    } else {
      buf.skip(wire);
    }
  }
}

bool CPbfReader::parsePrimitiveBlock(const QByteArray& data, qint32 what, const fNode& onNode, const fWay& onWay) {
  CProtoBuf buf(data);
  quint32 field, wire;
  block_params_t params;
  QVector<QByteArray> strings;
  QVector<QByteArrayView> groups;

  // granularity and offsets follow the groups, so collect the groups first
  while (buf.next(field, wire)) {
    if (field == 1 && wire == 2) {
      CProtoBuf table(buf.bytes());
      quint32 f, w;
      while (table.next(f, w)) {
        if (f == 1 && w == 2) {
          strings << table.bytes().toByteArray();
        } else {
          table.skip(w);
        }
      }
    } else if (field == 2 && wire == 2) {
      groups << buf.bytes();
    } else if (field == 17 && wire == 0) {
      params.granularity = qint64(buf.varint());
    } else if (field == 19 && wire == 0) {
      params.latOffset = qint64(buf.varint());
    } else if (field == 20 && wire == 0) {
      params.lonOffset = qint64(buf.varint());
    } else {
      buf.skip(wire);
    }
  }

  way_t way;
  bool beyondNodes = false;
  for (const QByteArrayView& group : std::as_const(groups)) {
    CProtoBuf bufGroup(group);
    while (bufGroup.next(field, wire)) {
      beyondNodes = beyondNodes || field == 3 || field == 4;
      if (field == 1 && wire == 2 && (what & eNodes)) {
        parseNode(bufGroup.bytes(), params, onNode);
      } else if (field == 2 && wire == 2 && (what & eNodes)) {
        parseDenseNodes(bufGroup.bytes(), params, onNode);
      } else if (field == 3 && wire == 2 && (what & eWays)) {
        parseWay(bufGroup.bytes(), way);
        for (quint32 idx : std::as_const(way.keys)) {
          if (idx >= quint32(strings.size())) {
            throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "string index out of range"));
          }
        }
        for (quint32 idx : std::as_const(way.vals)) {
          if (idx >= quint32(strings.size())) {
            throw std::runtime_error(QT_TRANSLATE_NOOP("CPbfReader", "string index out of range"));
          }
        }
        onWay(way, strings);
      } else {
        bufGroup.skip(wire);
      }
    }
  }
  return beyondNodes;
}

bool CPbfReader::readData(qint32 what, const fNode& onNode, const fWay& onWay, const fProgress& progress) {
  if (!file.isOpen() || !file.seek(posData)) {
    error = tr("PBF file is not open");
    return false;
  }

  // a sorted file has all nodes before the ways and relations
  const QString sorted = "Sort.Type_then_ID";
  const bool nodesOnly =
      what == eNodes && (hdr.requiredFeatures.contains(sorted) || hdr.optionalFeatures.contains(sorted));

  QByteArray type, data;
  while (nextBlob(type, data)) {
    bool beyondNodes = false;
    if (type == "OSMData") {
      try {
        beyondNodes = parsePrimitiveBlock(data, what, onNode, onWay);
      } catch (const std::exception& e) {
        error = tr("Broken PBF data block before offset %1: %2").arg(file.pos()).arg(tr(e.what()));
        return false;
      }
    }
    if (progress && !progress(file.pos())) {
      error = tr("aborted");
      return false;
    }
    if (nodesOnly && beyondNodes) {
      break;
    }
  }

  return error.isEmpty();
}
