/**********************************************************************************************
    Copyright (C) 2008 Oliver Eichler <oliver.eichler@gmx.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

**********************************************************************************************/
#ifndef IUNIT_H
#define IUNIT_H
#include <QObject>
#include <QTimeZone>

#define NOFLOAT 1000000000000.0
#define NOINT 0x7FFFFFFF
#define NOTIME 0xFFFFFFFF
#define NOIDX (-1)

extern const QPointF NOPOINTF;
extern const QPoint NOPOINT;

class IUnit : public QObject {
  Q_OBJECT
 public:
  static constexpr qreal footPerMeter = 3.28084;
  static constexpr qreal nauticalMilePerMeter = 1. / 1852;
  static constexpr qreal meterPerSecToKnots = 1.94361780;
  static constexpr qreal milePerMeter = 0.6213699E-3;
  static constexpr qreal meterPerSecToMilePerHour = 2.23693164;

  virtual ~IUnit() = default;

  static const IUnit& self() { return *m_self; }

  /// convert meter of elevation into a value and unit string
  virtual void meter2elevation(qreal meter, QString& val, QString& unit) const;
  virtual void meter2elevation(qreal meter, qreal& val, QString& unit) const;
  virtual void feet2elevation(qreal feet, QString& val, QString& unit) const {
    meter2elevation(feet / 3.28084, val, unit);
  }

  /// convert meter of distance into a value and unit string
  virtual void meter2distance(qreal meter, QString& val, QString& unit) const = 0;
  virtual void meter2distance(qreal meter, qreal& val, QString& unit) const = 0;
  /// convert meter per second to a speed value string and unit label
  virtual void meter2speed(qreal meter, QString& val, QString& unit) const;
  virtual void meter2speed(qreal meter, qreal& val, QString& unit) const;
  /// convert square meter to string and unit label
  virtual void meter2area(qreal meter, QString& val, QString& unit) const = 0;
  virtual void meter2area(qreal meter, qreal& val, QString& unit) const = 0;
  /// convert seconds to a timespan of days, hours, minutes and seconds
  virtual void seconds2time(quint32 ttime, QString& val, QString& unit) const;
  /// convert an elevation string to a float
  virtual qreal elevation2meter(const QString& val) const = 0;
  /// convert a range in meter into a scale and a matching unit
  virtual void meter2unit(qreal meter, qreal& scale, QString& unit) const = 0;
  /// convert meter into the base unit (ft, m)
  virtual void meter2base(qreal meter, QString& val, QString& unit) const;

  static bool convert(qreal& value, QString& unit, const QString& targetUnit);
  static QStringList getUnits() {
    QStringList list;
    list.append(timeToMKSMap.keys());
    list.append(distanceToMKSMap.keys());
    list.append(speedToMKSMap.keys());
    list.append(areaToMKSMap.keys());
    return list;
  }

  enum type_e { eTypeMetric, eTypeImperial, eTypeNautic, eTypeAviation };
  /// instantiate the correct unit object
  static void setUnitType(type_e t, QObject* parent);

  enum slope_mode_e { eSlopePercent, eSlopeDegrees };
  static void setSlopeMode(slope_mode_e mode) { slopeMode = mode; }
  static enum slope_mode_e getSlopeMode() { return slopeMode; }
  static void slope2string(qreal slope, QString& val, QString& unit);
  static void slope2unit(qreal slope, qreal& val, QString& unit);
  static qreal slopeConvert(slope_mode_e fromMode, qreal fromSlope);

  /// parse a string for a timestamp
  static bool parseTimestamp(const QString& time, QDateTime& datetime);

  enum time_format_e { eTimeFormatLong, eTimeFormatShort, eTimeFormatIso };
  /**
     @brief Convert date time object to string using the current timezone configuration


     @param time          the date/time object
     @param format        one of time_format_e
     @param pos           optional a position attached to the date/time object [rad]
     @return              A time string.
   */
  static QString datetime2string(const QDateTime& time, time_format_e format, const QPointF& pos = NOPOINTF);

  /// find the timezone setup by position
  static QByteArray pos2timezone(const QPointF& pos);

  const type_e type;
  const QString baseUnit;
  const qreal baseFactor;
  const QString speedUnit;
  const qreal speedFactor;
  const QString elevationUnit;
  const qreal elevationFactor;
  static const char* tblTimezone[];

  enum tz_mode_e { eTZUtc, eTZLocal, eTZAuto, eTZSelected };

  static void getTimeZoneSetup(tz_mode_e& mode, QByteArray& zone, bool& format) {
    mode = timeZoneMode;
    zone = timeZone;
    format = useShortFormat;
  }

  static void setTimeZoneSetup(tz_mode_e mode, const QByteArray& zone, bool format) {
    timeZoneMode = mode;
    timeZone = zone;
    useShortFormat = format;
  }

  enum coord_format_e { eCoordFormat1, eCoordFormat2, eCoordFormat3 };

  static enum coord_format_e getCoordFormat() { return coordFormat; }

  static void setCoordFormat(const coord_format_e format) { coordFormat = format; }

  static void degToStr(const qreal& x, const qreal& y, QString& str);

  static bool strToDeg(const QString& str, qreal& lon, qreal& lat);

  static bool isValidCoordString(const QString& str);

 protected:
  IUnit(const type_e& type, const QString& baseUnit, const qreal baseFactor, const QString& speedUnit,
        const qreal speedFactor, const QString& elevationUnit, const qreal elevationFactor, QObject* parent);

  static slope_mode_e slopeMode;

  static QDateTime parseTimestamp(const QString& timetext, int& tzoffset);

  static tz_mode_e timeZoneMode;
  static QByteArray timeZone;
  static bool useShortFormat;

  static coord_format_e coordFormat;

 private:
  static const IUnit* m_self;

  static const QRegularExpression reCoord1;
  static const QRegularExpression reCoord2;
  static const QRegularExpression reCoord3;
  static const QRegularExpression reCoord4;
  static const QRegularExpression reCoord5;

  enum unit_type_e { eUnitTypeTime, eUnitTypeDistance, eUnitTypeSpeed, eUnitTypeArea };

  static QMap<QString, qreal> timeToMKSMap;
  static QMap<QString, qreal> initTimeToMKSMap();
  static QMap<QString, qreal> distanceToMKSMap;
  static QMap<QString, qreal> initDistanceToMKSMap();
  static QMap<QString, qreal> speedToMKSMap;
  static QMap<QString, qreal> initSpeedToMKSMap();
  static QMap<QString, qreal> areaToMKSMap;
  static QMap<QString, qreal> initAreaToMKSMap();
};
#endif  // IUNIT_H
