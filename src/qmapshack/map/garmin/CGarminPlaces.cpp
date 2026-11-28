/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "map/garmin/CGarminPlaces.h"

#include <QByteArray>
#include <QStringList>

#include "map/garmin/CGarminPoint.h"
#include "map/garmin/IGarminStrTbl.h"

namespace {
constexpr quint8 kHouseNumber = 0x01;
constexpr quint8 kStreet = 0x02;
constexpr quint8 kCity = 0x04;
constexpr quint8 kZip = 0x08;
constexpr quint8 kPhone = 0x10;
constexpr quint8 kExit = 0x20;

/** Facilities of one exit read at most; mkgmap marks the last one. */
constexpr quint32 kMaxFacilities = 32;

/** Bytes of a POI record read at most: name, flags, two numbers, street, city and zip index, exit. */
constexpr quint32 kMaxRecord = 64;

constexpr quint32 kNoLabel = 0xFFFFFFFF;

quint32 load3(const quint8* p) { return p[0] | (p[1] << 8) | (p[2] << 16); }

/** Bytes of an index into a table of @p count entries (mkgmap's Utils.numberToPointerSize). */
quint32 pointerSize(quint32 count) { return count <= 0xFF ? 1 : count <= 0xFFFF ? 2 : 3; }

/** File offset of record @p index (1-based) of @p section; false past 4 GB. */
bool recordOffset(const CGarminPlaces::section_t& section, quint32 index, quint32& offset) {
  const quint64 at = quint64(section.offset) + quint64(index - 1) * section.recSize;
  offset = quint32(at);
  return at <= 0xFFFFFFFFu;
}

quint32 loadN(const quint8* p, quint32 n) {
  quint32 value = 0;
  for (quint32 i = 0; i < n; ++i) {
    value |= quint32(p[i]) << (8 * i);
  }
  return value;
}
}  // namespace

CGarminPlaces::CGarminPlaces(const layout_t& layout, IGarminStrTbl& strtbl, const read_t& read)
    : layout(layout), strtbl(strtbl), read(read) {}

QString CGarminPlaces::base11(const quint8*& data, const quint8* end) {
  QString text;
  // the first and the last byte have bit 7 set
  qint32 marks = 2;
  while (marks > 0) {
    if (data >= end) {
      return QString();
    }
    const quint8 byte = *data++;
    if (byte & 0x80) {
      --marks;
    }
    const quint8 value = byte & 0x7F;
    for (const quint8 digit : {quint8(value / 11), quint8(value % 11)}) {
      text += digit == 10 ? QChar('-') : QChar('0' + digit);
    }
  }
  while (text.endsWith('-')) {
    text.chop(1);
  }
  return text;
}

QString CGarminPlaces::label(CFileExt& file, quint32 offset) {
  if (offset == kNoLabel) {
    return QString();
  }
  QStringList labels;
  strtbl.get(file, offset & 0x003FFFFF, IGarminStrTbl::norm, labels);
  return IGarminStrTbl::displayText(labels);
}

quint32 CGarminPlaces::recordLabel(CFileExt& file, const section_t& section, quint32 index) {
  if (index == 0 || index > section.count() || section.recSize < 3) {
    return kNoLabel;
  }
  QByteArray data;
  quint32 at = 0;
  if (recordOffset(section, index, at)) {
    read(file, at, 3, data);
  }
  if (data.size() < 3) {
    return kNoLabel;
  }
  return load3(reinterpret_cast<const quint8*>(data.constData()));
}

void CGarminPlaces::readFacilities(CFileExt& file, quint32 index, details_t& p) {
  const section_t& section = layout.exitFacility;
  if (section.recSize < 5) {
    return;
  }
  for (quint32 i = index; i > 0 && i <= section.count() && i < index + kMaxFacilities; ++i) {
    QByteArray data;
    quint32 at = 0;
    if (recordOffset(section, i, at)) {
      read(file, at, 5, data);
    }
    if (data.size() < 5) {
      return;
    }
    const quint8* q = reinterpret_cast<const quint8*>(data.constData());
    // label 0-21, bit 23 last facility of the exit, type 24-27, direction 29-31
    const quint32 word = load3(q) | (quint32(q[3]) << 24);
    CGarminPoint::exit_facility_t facility;
    facility.description = label(file, word & 0x003FFFFF);
    facility.type = (word >> 24) & 0x0F;
    facility.direction = word >> 29;
    facility.facilities = q[4];
    p.exitFacilities << facility;
    if (word & 0x00800000) {
      return;
    }
  }
}

QString CGarminPlaces::country(CFileExt& file, quint32 index) {
  return label(file, recordLabel(file, layout.country, index));
}

const CGarminPlaces::city_t& CGarminPlaces::city(CFileExt& file, quint32 index, const point_label_t& pointLabel) {
  auto it = cities.constFind(index);
  if (it != cities.constEnd()) {
    return it.value();
  }

  city_t city;
  QByteArray data;
  if (index > 0 && index <= layout.city.count() && layout.city.recSize >= 5) {
    quint32 at = 0;
    if (recordOffset(layout.city, index, at)) {
      read(file, at, 5, data);
    }
  }
  if (data.size() == 5) {
    const quint8* p = reinterpret_cast<const quint8*>(data.constData());
    const quint32 ref = load3(p);
    const quint16 info = p[3] | (p[4] << 8);
    // bit 15: the name is the label of point (ref & 0xFF) in subdivision (ref >> 8)
    city.name = (info & 0x8000) ? pointLabel(file, ref >> 8, ref & 0xFF) : label(file, ref);
    // bit 14: the 14 bits are a country index, else a region index
    if (info & 0x4000) {
      city.country = country(file, info & 0x3FFF);
    } else {
      QByteArray region;
      const quint32 r = info & 0x3FFF;
      if (r > 0 && r <= layout.region.count() && layout.region.recSize >= 5) {
        quint32 at = 0;
        if (recordOffset(layout.region, r, at)) {
          read(file, at, 5, region);
        }
      }
      if (region.size() == 5) {
        const quint8* q = reinterpret_cast<const quint8*>(region.constData());
        city.country = country(file, q[0] | (q[1] << 8));
        city.region = label(file, load3(q + 2));
      }
    }
  }
  return cities.insert(index, city).value();
}

void CGarminPlaces::readDetails(CFileExt& file, quint32 offset, const point_label_t& pointLabel, CGarminPoint& p) {
  auto it = details.find(offset);
  if (it == details.end()) {
    details_t d;
    decodeDetails(file, offset, pointLabel, d);
    it = details.insert(offset, d);
  }
  const details_t& d = it.value();
  p.houseNumber = d.houseNumber;
  p.street = d.street;
  p.city = d.city;
  p.region = d.region;
  p.country = d.country;
  p.zip = d.zip;
  p.phone = d.phone;
  p.isExit = d.isExit;
  p.exitHighway = d.exitHighway;
  p.exitTo = d.exitTo;
  p.exitOvernightParking = d.exitOvernightParking;
  p.exitFacilities = d.exitFacilities;
}

void CGarminPlaces::decodeDetails(CFileExt& file, quint32 offset, const point_label_t& pointLabel, details_t& p) {
  if (offset >= layout.sizePoiProps) {
    return;
  }
  QByteArray data;
  if (quint64(layout.offsetPoiProps) + offset > 0xFFFFFFFFu) {
    return;
  }
  read(file, layout.offsetPoiProps + offset, qMin(kMaxRecord, layout.sizePoiProps - offset), data);
  const quint8* pData = reinterpret_cast<const quint8*>(data.constData());
  const quint8* const pEnd = pData + data.size();
  if (pEnd - pData < 3) {
    return;
  }

  quint8 flags = layout.globalFlags;
  const bool ownFlags = pData[2] & 0x80;
  pData += 3;
  if (ownFlags) {
    if (pData >= pEnd) {
      return;
    }
    // bit j of the own flags is the j-th field set in the global flags
    const quint8 local = *pData++;
    flags = 0;
    quint32 j = 0;
    for (quint32 i = 0; i < 7; ++i) {
      const quint8 field = 1 << i;
      if (layout.globalFlags & field) {
        if (local & (1 << j)) {
          flags |= field;
        }
        ++j;
      }
    }
  }

  // a number of digits, or a label for one like "221b"
  const auto number = [&](QString& text) {
    if (pData >= pEnd) {
      return false;
    }
    if (*pData & 0x80) {
      text = base11(pData, pEnd);
      return !text.isEmpty();
    }
    if (pEnd - pData < 3) {
      return false;
    }
    text = label(file, (pData[0] << 16) | pData[1] | (pData[2] << 8));
    pData += 3;
    return true;
  };
  const auto index = [&](quint32 count, quint32& value) {
    const quint32 n = pointerSize(count);
    if (quint32(pEnd - pData) < n) {
      return false;
    }
    value = loadN(pData, n);
    pData += n;
    return true;
  };

  if ((flags & kHouseNumber) && !number(p.houseNumber)) {
    return;
  }
  if (flags & kStreet) {
    if (pEnd - pData < 3) {
      return;
    }
    p.street = label(file, load3(pData));
    pData += 3;
  }
  if (flags & kCity) {
    quint32 i = 0;
    if (!index(layout.city.count(), i)) {
      return;
    }
    const city_t& c = city(file, i, pointLabel);
    p.city = c.name;
    p.region = c.region;
    p.country = c.country;
  }
  if (flags & kZip) {
    quint32 i = 0;
    if (!index(layout.zip.count(), i)) {
      return;
    }
    p.zip = label(file, recordLabel(file, layout.zip, i));
  }
  if ((flags & kPhone) && !number(p.phone)) {
    return;
  }
  if (flags & kExit) {
    if (pEnd - pData < 3) {
      return;
    }
    // label 0-21, bit 22 overnight parking, bit 23 a facility index follows
    const quint32 info = load3(pData);
    pData += 3;
    quint32 highway = 0;
    if (!index(layout.highway.count(), highway)) {
      return;
    }
    p.isExit = true;
    p.exitTo = (info & 0x003FFFFF) ? label(file, info) : QString();
    p.exitOvernightParking = info & 0x00400000;
    p.exitHighway = label(file, recordLabel(file, layout.highway, highway));
    quint32 facility = 0;
    if ((info & 0x00800000) && index(layout.exitFacility.count(), facility)) {
      readFacilities(file, facility, p);
    }
  }
}
