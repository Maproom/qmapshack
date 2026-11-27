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
#include "map/garmin/CGarminMarine.h"

#include <QCoreApplication>
#include <iterator>

namespace {
// flags0 of lights and buoys
constexpr quint8 kRacon = 0x01;
constexpr quint8 kNote = 0x02;
constexpr quint8 kLocalDesignator = 0x04;
constexpr quint8 kInternationalDesignator = 0x08;

constexpr quint8 kMorse = 11;

constexpr const char* kLightColours[] = {"unlit", "red", "green", "white", "blue", "yellow", "violet", "amber"};

constexpr const char* kColours[] = {"",
                                    "red",
                                    "green",
                                    "yellow",
                                    "white",
                                    "black",
                                    "black-yellow",
                                    "white-red",
                                    "black-red",
                                    "white-green",
                                    "red-yellow",
                                    "red-green",
                                    "orange",
                                    "black-yellow-black",
                                    "yellow-black",
                                    "yellow-black-yellow",
                                    "red-white",
                                    "green-red-green",
                                    "red-green-red",
                                    "black-red-black",
                                    "yellow-red-yellow",
                                    "green-red",
                                    "black-white",
                                    "white-orange",
                                    "orange-white",
                                    "green-white"};

constexpr const char* kLightTypes[] = {"",
                                       "fixed",
                                       "isophase",
                                       "flashing",
                                       "group flashing",
                                       "composite group flashing",
                                       "occulting",
                                       "group occulting",
                                       "composite group occulting",
                                       "long flashing",
                                       "group long flashing",
                                       "morse code letter",
                                       "quick",
                                       "group quick",
                                       "group quick and long flashing",
                                       "interrupted quick",
                                       "very quick",
                                       "group very quick",
                                       "group very quick and long flashing",
                                       "interrupted very quick",
                                       "ultra quick",
                                       "interrupted ultra quick",
                                       "fixed and occulting",
                                       "fixed and group occulting",
                                       "fixed and isophase",
                                       "fixed and flashing",
                                       "fixed and group flashing",
                                       "fixed and long flashing",
                                       "alternating",
                                       "alternating occulting",
                                       "alternating flashing",
                                       "alternating group flashing"};

/** Reads bytes in order; every read past the end fails the whole decode. */
class CReader {
 public:
  CReader(const QByteArray& bytes, qsizetype start) : bytes(bytes), pos(start) {}

  bool ok() const { return good; }
  bool atEnd() const { return pos >= bytes.size(); }

  quint8 u8() {
    if (pos >= bytes.size()) {
      good = false;
      return 0;
    }
    return quint8(bytes[pos++]);
  }
  quint16 u16() {
    const quint16 lo = u8();
    return lo | quint16(u8()) << 8;
  }
  quint32 u24() {
    const quint32 lo = u16();
    return lo | quint32(u8()) << 16;
  }

 private:
  const QByteArray& bytes;
  qsizetype pos;
  bool good = true;
};

/** Light and eclipse phases [s]; a phase longer than 6.3 s is a run of 0x3f bytes and the rest. */
QVector<qreal> readPhases(CReader& reader) {
  QVector<qreal> phases;
  quint32 sum = 0;
  while (!reader.atEnd()) {
    const quint8 b = reader.u8();
    sum += b;
    if (b != 0x3f) {
      phases << sum / 10.0;
      sum = 0;
    }
  }
  if (sum != 0) {
    phases << sum / 10.0;
  }
  return phases;
}

CGarminMarine::distance_t readDistance(CReader& reader, bool twoBytes, bool metric) {
  CGarminMarine::distance_t d;
  d.value = twoBytes ? reader.u16() : reader.u8();
  d.metric = metric;
  d.valid = reader.ok();
  return d;
}
}  // namespace

CGarminMarine CGarminMarine::decode(quint32 type, Kind kind, const QByteArray& bytes) {
  CGarminMarine marine;
  if (bytes.isEmpty() || (type & 0xFFFF0000) != 0x10000) {
    return marine;
  }

  const quint32 t = type & 0xFFFF;
  const quint32 r = t & 0xFF00;

  bool ok = false;
  switch (kind) {
    case Kind::ePoint:
      if (r == 0x0100) {
        ok = marine.decodeLight(bytes);
      } else if (r == 0x0200) {
        ok = marine.decodeBuoy(bytes);
      } else if (r == 0x0300 || r == 0x0400) {
        ok = marine.decodeDistance(bytes, r == 0x0400);
      } else if (r == 0x0500) {
        // a label point: colour only
        ok = bytes.size() == 1;
        marine.colour = quint8(bytes[0]) & 0x0F;
      } else if (t == 0x0903) {
        ok = marine.decodeFacilities(bytes);
      }
      break;

    case Kind::eLine:
      if (r == 0x0300 || t == 0x0105 || t == 0x0106 || t == 0x0107) {
        ok = marine.decodeDistance(bytes, false);
      } else if (r == 0x0400 || r == 0x0500 || r == 0x0600) {
        ok = marine.decodeStyle(bytes);
      }
      break;

    case Kind::ePolygon:
      if (r == 0x0300) {
        ok = marine.decodeDistance(bytes, false);
      }
      break;
  }

  if (!ok) {
    return CGarminMarine();
  }
  marine.empty = false;
  return marine;
}

const char* CGarminMarine::lightColourName(quint8 colour) {
  return colour < std::size(kLightColours) ? kLightColours[colour] : "";
}

const char* CGarminMarine::colourName(quint8 colour) { return colour < std::size(kColours) ? kColours[colour] : ""; }

const char* CGarminMarine::lightTypeName(quint8 type) { return type < std::size(kLightTypes) ? kLightTypes[type] : ""; }

bool CGarminMarine::decodeLight(const QByteArray& bytes) {
  CReader reader(bytes, 0);
  const quint8 flags0 = reader.u8();
  // the second byte holds the length; garminExtraDataSize() already cut the data to it
  reader.u8();
  const quint8 typeByte = reader.u8();
  const quint8 flags1 = reader.u8();
  const quint8 lightsDef = reader.u8();
  if ((flags0 & 0xE0) != 0xE0 || !(typeByte & 0x80)) {
    return false;
  }

  lightType = typeByte & 0x1F;
  racon = flags0 & kRacon;

  // BaseCamp shows the first height as above datum, the second as above foundation; mkgmap names them the other way
  if (lightsDef & 0xC0) {
    heightAboveDatum = readDistance(reader, lightsDef & 0x80, lightsDef & 0x20);
  }
  if (lightsDef & 0x0C) {
    heightAboveFoundation = readDistance(reader, lightsDef & 0x08, lightsDef & 0x02);
  }

  // period in tenths of a second, bit 8 in the type byte
  period = (reader.u8() | ((typeByte & 0x40) ? 0x100 : 0)) / 10.0;

  if (flags0 & kNote) {
    note = reader.u24();
  }
  if (flags0 & kLocalDesignator) {
    localDesignator = reader.u24();
  }
  if (flags0 & kInternationalDesignator) {
    internationalDesignator = reader.u24();
  }
  if (flags1 & 0x02) {
    leadingAngle = reader.u16() / 10.0;
  }

  if (flags1 & 0x08) {
    // several lights: colour and sector start, a range for every lit sector, 0x8000 on the last
    while (reader.ok()) {
      const quint16 val = reader.u16();
      light_t light;
      light.colour = (val >> 12) & 0x07;
      light.angle = (val & 0x0FFF) / 10.0;
      if (light.colour != 0) {
        light.range = reader.u8();
      }
      lights << light;
      if (val & 0x8000) {
        break;
      }
    }
  } else {
    const quint8 b = reader.u8();
    if (b >> 5) {
      lights << light_t{quint8(b >> 5), quint8(b & 0x1F), 0};
    }
  }

  if (!reader.atEnd()) {
    const quint8 b = reader.u8();
    if (b > 0x80) {
      lightPhases = b - 0x80;
      phases = readPhases(reader);
    } else if (lightType == kMorse && b != 0x01) {
      morse = char(b);
    }
  }
  return reader.ok();
}

bool CGarminMarine::decodeBuoy(const QByteArray& bytes) {
  CReader reader(bytes, 0);
  const quint8 flags0 = reader.u8();
  reader.u8();
  const quint8 colours = reader.u8();
  const quint8 flags1 = reader.u8();
  if ((flags0 & 0xE0) != 0xE0) {
    return false;
  }

  racon = flags0 & kRacon;
  colour = colours & 0x3F;
  const quint8 lightColour = (colours >> 6) | ((flags1 & 0x01) << 2);
  if (lightColour != 0) {
    lights << light_t{lightColour, 0, 0};
  }

  if (flags0 & kNote) {
    note = reader.u24();
  }
  if (flags0 & kLocalDesignator) {
    localDesignator = reader.u24();
  }
  if (flags0 & kInternationalDesignator) {
    internationalDesignator = reader.u24();
  }

  const quint8 typeByte = reader.u8();
  lightType = typeByte & 0x1F;
  if (typeByte & 0x80) {
    period = reader.u8() / 10.0;
    const quint8 b = reader.u8();
    if (b == 0x81 || b == 0x82) {
      phases = readPhases(reader);
    }
  } else if (lightType == kMorse) {
    const quint8 b = reader.u8();
    if (b != 0x01) {
      morse = char(b);
    }
  }
  return reader.ok();
}

bool CGarminMarine::decodeDistance(const QByteArray& bytes, bool withPosition) {
  const quint8 b0 = quint8(bytes[0]);
  const bool twoBytes = (b0 & 0xE0) == 0xA0;
  if ((b0 & 0xE0) != 0x80 && !twoBytes) {
    return false;
  }
  CReader reader(bytes, 1);
  depth = readDistance(reader, twoBytes, b0 & 0x10);
  if (b0 & 0x08) {
    depth.value /= 10.0;
  }
  if (withPosition) {
    position = b0 & 0x07;
  }
  return depth.valid;
}

bool CGarminMarine::decodeStyle(const QByteArray& bytes) {
  const quint8 b0 = quint8(bytes[0]);
  colour = b0 & 0x0F;
  if (!(b0 & 0x80)) {
    return bytes.size() == 1;
  }
  if (bytes.size() != 2) {
    return false;
  }
  // mkgmap keeps bits 0-1 and 4-5 of the style byte, the latter at bits 3-4
  const quint8 s = quint8(bytes[1]);
  lineStyle = (s & 0x03) | ((s & 0x18) << 1);
  return true;
}

bool CGarminMarine::decodeFacilities(const QByteArray& bytes) {
  if (bytes.size() != 3 || (quint8(bytes[0]) & 0xE0) != 0xA0) {
    return false;
  }
  const quint32 b0 = quint8(bytes[0]);
  const quint32 b1 = quint8(bytes[1]);
  const quint32 b2 = quint8(bytes[2]);
  facilities = (b0 & 0x1F) | (b1 << 5) | ((b2 & 0x07) << 13) | ((b2 & 0x10) << 12);
  return true;
}

namespace {
/** Light colour in chart notation (INT 1 P11): R, G, W, Bu, Y, Vi, Am. */
QString colourAbbreviation(quint8 colour) {
  static const char* abbreviations[] = {"", "R", "G", "W", "Bu", "Y", "Vi", "Am"};
  return colour < std::size(abbreviations) ? abbreviations[colour] : "";
}

/** Light character in chart notation (INT 1 P10); @p group is the flashes per group. */
QString characterOf(quint8 type, const QString& group, char morse) {
  const QString g = "(" + group + ")";
  switch (type) {
    case 1:
      return "F";
    case 2:
      return "Iso";
    case 3:
      return "Fl";
    case 4:
      return "Fl" + g;
    case 5:
      return "Fl(…)";
    case 6:
      return "Oc";
    case 7:
      return "Oc" + g;
    case 8:
      return "Oc(…)";
    case 9:
      return "LFl";
    case 10:
      return "LFl" + g;
    case kMorse:
      return QString("Mo(%1)").arg(morse != 0 ? QString(QChar(morse)) : QString("…"));
    case 12:
      return "Q";
    case 13:
      return "Q" + g;
    case 14:
      return "Q" + g + "+LFl";
    case 15:
      return "IQ";
    case 16:
      return "VQ";
    case 17:
      return "VQ" + g;
    case 18:
      return "VQ" + g + "+LFl";
    case 19:
      return "IVQ";
    case 20:
      return "UQ";
    case 21:
      return "IUQ";
    case 22:
      return "FOc";
    case 23:
      return "FOc" + g;
    case 24:
      return "FIso";
    case 25:
      return "FFl";
    case 26:
      return "FFl" + g;
    case 27:
      return "FLFl";
    case 28:
      return "Al.";
    case 29:
      return "Al.Oc";
    case 30:
      return "Al.Fl";
    case 31:
      return "Al.Fl" + g;
    default:
      return QString();
  }
}

qreal toMetres(const CGarminMarine::distance_t& d) { return d.metric ? d.value : d.value * 0.3048; }
}  // namespace

QString CGarminMarine::lightCharacteristic() const {
  QVector<light_t> lit;
  for (const light_t& light : lights) {
    if (light.colour != 0) {
      lit << light;
    }
  }
  if (lit.isEmpty() && lightType == 0) {
    return QString();
  }

  QString colours;
  for (const light_t& light : std::as_const(lit)) {
    const QString abbreviation = colourAbbreviation(light.colour);
    if (!colours.contains(abbreviation)) {
      colours += abbreviation;
    }
  }
  // white is written only on sector and alternating lights
  const bool alternating = lightType >= 28 && lightType <= 31;
  if (colours == "W" && !alternating) {
    colours.clear();
  }

  const QString group = lightPhases > 0 ? QString::number(lightPhases) : QString("…");
  QString text = characterOf(lightType, group, morse) + colours;

  QString tail;
  if (period > 0) {
    tail += QString::number(period) + "s";
  }
  if (heightAboveDatum.valid) {
    tail += QString::number(qRound(toMetres(heightAboveDatum))) + "m";
  }
  quint8 rangeMin = 0xFF;
  quint8 rangeMax = 0;
  for (const light_t& light : std::as_const(lit)) {
    if (light.range != 0) {
      rangeMin = qMin(rangeMin, light.range);
      rangeMax = qMax(rangeMax, light.range);
    }
  }
  if (rangeMax != 0) {
    tail += rangeMin == rangeMax ? QString("%1M").arg(rangeMax) : QString("%1-%2M").arg(rangeMax).arg(rangeMin);
  }
  return tail.isEmpty() ? text : text + "." + tail;
}

QStringList CGarminMarine::sectorTexts() const {
  QStringList texts;
  if (lights.size() < 2) {
    return texts;
  }
  for (qsizetype i = 0; i < lights.size(); ++i) {
    const light_t& light = lights[i];
    if (light.colour == 0) {
      continue;
    }
    const qreal end = lights[(i + 1) % lights.size()].angle;
    texts << QString("%1 %2°-%3° %4M").arg(colourAbbreviation(light.colour)).arg(light.angle).arg(end).arg(light.range);
  }
  return texts;
}

QStringList CGarminMarine::facilityNames() const {
  static const char* names[] = {
      QT_TRANSLATE_NOOP("CGarminMarine", "boat ramp"),
      QT_TRANSLATE_NOOP("CGarminMarine", "drinking water"),
      QT_TRANSLATE_NOOP("CGarminMarine", "restrooms"),
      QT_TRANSLATE_NOOP("CGarminMarine", "picnic area"),
      QT_TRANSLATE_NOOP("CGarminMarine", "campground"),
      QT_TRANSLATE_NOOP("CGarminMarine", "marina"),
      QT_TRANSLATE_NOOP("CGarminMarine", "fuel"),
      QT_TRANSLATE_NOOP("CGarminMarine", "marine supply"),
      QT_TRANSLATE_NOOP("CGarminMarine", "bait and tackle"),
      QT_TRANSLATE_NOOP("CGarminMarine", "groceries"),
      QT_TRANSLATE_NOOP("CGarminMarine", "restaurant"),
      QT_TRANSLATE_NOOP("CGarminMarine", "water and electric hook-up"),
      QT_TRANSLATE_NOOP("CGarminMarine", "boat and motor rental"),
      QT_TRANSLATE_NOOP("CGarminMarine", "guide service"),
      QT_TRANSLATE_NOOP("CGarminMarine", "lodging"),
      QT_TRANSLATE_NOOP("CGarminMarine", "dump station"),
      QT_TRANSLATE_NOOP("CGarminMarine", "handicap accessible"),
  };
  QStringList list;
  for (quint32 i = 0; i < std::size(names); ++i) {
    if (facilities & (1u << i)) {
      list << QCoreApplication::translate("CGarminMarine", names[i]);
    }
  }
  return list;
}

bool CGarminMarine::isHeight(quint32 type, Kind kind) {
  const quint32 t = type & 0xFFFF;
  if (kind == Kind::eLine) {
    // contour line, overhead cable, bridge
    return t == 0x0105 || t == 0x0106 || t == 0x0107;
  }
  // BaseCamp 4.7.5: 0x10302 and obstructions 0x104xx show a height, 0x10301 a depth
  return kind == Kind::ePoint && (t == 0x0302 || (t & 0xFF00) == 0x0400);
}

bool CGarminMarine::isMarineLine(quint32 type) {
  if ((type & 0xFFFF0000) != 0x10000) {
    return false;
  }
  const quint32 t = type & 0xFFFF;
  const quint32 r = t & 0xFF00;
  return r == 0x0300 || r == 0x0400 || r == 0x0500 || r == 0x0600 || (t >= 0x0105 && t <= 0x0107);
}
