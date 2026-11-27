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
#ifndef CGARMINMARINE_H
#define CGARMINMARINE_H

#include <QByteArray>
#include <QStringList>
#include <QVector>
#include <QtGlobal>

/**
   @brief The extra data of an extended (0x1xxxx) object as mkgmap writes it: depth or height, colour and line
          style, lights, facilities.

   The meaning of the bytes depends on the object kind and its type range; see ExtTypeAttributes in mkgmap.
 */
class CGarminMarine {
 public:
  enum class Kind { ePoint, eLine, ePolygon };

  /** One light of a light or buoy; several lights split the horizon into sectors. */
  struct light_t {
    quint8 colour = 0; /**< 0 unlit, 1 red, 2 green, 3 white, 4 blue, 5 yellow, 6 violet, 7 amber */
    quint8 range = 0;  /**< visible range [nm] */
    qreal angle = 0;   /**< start of the sector [deg]; 0 for a single light */
  };

  /** A distance in the map's unit. */
  struct distance_t {
    bool valid = false;
    qreal value = 0;    /**< in metres if metric, else feet */
    bool metric = true; /**< false: feet */
  };

  static constexpr quint32 kNoLabel = 0xFFFFFFFF;

  /**
     @brief Decode the extra data of an object.
     @param type  The object's full type, e.g. 0x10301.
     @param kind  Point, line or polygon.
     @param bytes The extra data as stored after the object.
     @return The decoded data; isEmpty() if the type range carries none or the data does not match it.
   */
  static CGarminMarine decode(quint32 type, Kind kind, const QByteArray& bytes);

  /** Light colour name in English, as mkgmap reads it: "red", "green", ... */
  static const char* lightColourName(quint8 colour);
  /** Foundation or line colour name in English, as mkgmap reads it: "red", "black-yellow", ... */
  static const char* colourName(quint8 colour);
  /** Light type name in English, as mkgmap reads it: "fixed", "flashing", ... */
  static const char* lightTypeName(quint8 type);

  bool isEmpty() const { return empty; }

  /**
     @brief The light in chart notation (INT 1 P16): character, colours, period, elevation, range.
     @return E.g. "Fl(3)WRG.15s21m15-11M"; empty if the object has no light.
   */
  QString lightCharacteristic() const;
  /** One line per sector of a sector light, e.g. "R 0°-120° 10M"; empty for a light without sectors. */
  QStringList sectorTexts() const;
  /** The facilities of a facility point, translated. */
  QStringList facilityNames() const;
  /** Whether a line of @p type is marine: a depth or height line, or a line with colour and style. */
  static bool isMarineLine(quint32 type);
  /** Whether a depth point, line or area holds a height rather than a depth, as BaseCamp shows it. */
  static bool isHeight(quint32 type, Kind kind);

  /** Depth or height of a depth point, obstruction, depth line or area. */
  distance_t depth;
  /** Position quality of an obstruction: 0 unknown, 2 doubtful, 3 existence doubtful, 4 approximate, 5 reported. */
  quint8 position = 0;

  /** Foundation colour of a buoy, colour of a label point or a line; see colourName(). */
  quint8 colour = 0;
  /** Line style, the upper byte of mkgmap's style value. */
  quint8 lineStyle = 0;

  /** Light type of a light or buoy; see lightTypeName(). */
  quint8 lightType = 0;
  /** The Morse letter of a Morse light, 0 if none. */
  char morse = 0;
  QVector<light_t> lights;
  /** Total period of a light or buoy [s], 0 if none. */
  qreal period = 0;
  /** Light and eclipse phases [s], in that order; empty if only the total period is known. */
  QVector<qreal> phases;
  /** Number of light phases at the start of phases, 0 if not known. */
  quint8 lightPhases = 0;
  bool racon = false;
  /** Height above foundation; BaseCamp's reading, mkgmap writes its height above datum here. */
  distance_t heightAboveFoundation;
  /** Height above datum, the elevation on charts; BaseCamp's reading, mkgmap's height above foundation. */
  distance_t heightAboveDatum;
  /** Leading angle of a leading light [deg]; negative if none. */
  qreal leadingAngle = -1;

  /** LBL offsets of the note, local and international designator; kNoLabel if none. */
  quint32 note = kNoLabel;
  quint32 localDesignator = kNoLabel;
  quint32 internationalDesignator = kNoLabel;

  /** Facilities of a facility point 0x10903, a bit mask as mkgmap's xt-facilities. */
  quint32 facilities = 0;

 private:
  bool decodeLight(const QByteArray& bytes);
  bool decodeBuoy(const QByteArray& bytes);
  bool decodeDistance(const QByteArray& bytes, bool withPosition);
  bool decodeStyle(const QByteArray& bytes);
  bool decodeFacilities(const QByteArray& bytes);

  bool empty = true;
};

#endif  // CGARMINMARINE_H
