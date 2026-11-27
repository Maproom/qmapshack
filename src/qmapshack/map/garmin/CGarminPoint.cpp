/**********************************************************************************************
    Copyright (C) 2006-2007 Oliver Eichler <oliver.eichler@gmx.de>

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

   Garmin and MapSource are registered trademarks or trademarks of Garmin Ltd.
   or one of its subsidiaries.

   This source is based on John Mechalas documentation "Garmin IMG File Format" found
   at sourceforge. The missing bits and error were rectified by the source code of
   Konstantin Galichsky (kg@geopainting.com), http://www.geopainting.com

**********************************************************************************************/
#include "CGarminPoint.h"

#include <QtCore>

#include "CGarminMarine.h"
#include "Garmin.h"
#include "IGarminStrTbl.h"
#include "helpers/Platform.h"
#include "units/IUnit.h"

quint32 CGarminPoint::decode(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, const quint8* pData) {
  type = (quint16)(*pData) << 8;

  ++pData;

  lbl_ptr = gar_ptr_load(uint24_t, pData);
  hasSubType = lbl_ptr & 0x00800000;
  isLbl6 = lbl_ptr & 0x00400000;
  lbl_ptr = lbl_ptr & 0x003FFFFF;

  pData += 3;

  qint16 dLng = gar_ptr_load(int16_t, pData);
  pData += 2;
  qint16 dLat = gar_ptr_load(int16_t, pData);
  pData += 2;

  qint32 x1 = ((qint32)dLng << shift) + iCenterLon;
  qint32 y1 = ((qint32)dLat << shift) + iCenterLat;
  pos = QPointF(GARMIN_RAD(x1), GARMIN_RAD(y1));

#ifdef DEBUG_SHOW_POINTS
  qDebug() << x1 << y1 << point.u << point.v;
#endif

  if (hasSubType) {
    type |= *pData;
    return 9;
  }

  return 8;
}

quint32 CGarminPoint::decode2(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, const quint8* pData,
                              const quint8* pEnd) {
  quint32 byte_size = 6;
  quint8 subtype;
  const quint8* const pStart = pData;

  // a point cut off by the section's end keeps type 0
  if (pEnd - pStart < 2 || quint32(pEnd - pStart) < byte_size + ((pData[1] & 0x20) ? 3 : 0)) {
    return pEnd - pStart;
  }

  type = (quint16)(*pData) << 8;
  ++pData;
  subtype = (quint16)(*pData);
  ++pData;

  type = 0x10000 + type + (subtype & 0x1F);

  qint16 dLng = gar_ptr_load(int16_t, pData);
  pData += 2;
  qint16 dLat = gar_ptr_load(int16_t, pData);
  pData += 2;

  qint32 x1, y1;

  x1 = ((qint32)dLng << shift) + iCenterLon;
  y1 = ((qint32)dLat << shift) + iCenterLat;
  pos = QPointF(GARMIN_RAD(x1), GARMIN_RAD(y1));

  if (subtype & 0x20) {
    byte_size += 3;
    lbl_ptr = gar_ptr_load(uint24_t, pData);
    isLbl6 = lbl_ptr & 0x00400000;
    lbl_ptr &= 0x003FFFFF;
    pData += 3;
  }

  if (subtype & 0x80) {
    const quint32 size = garminExtraDataSize(pData, pEnd);
    if (size == 0) {
      // unknown form: the rest of the section cannot be read
      return pEnd - pStart;
    }
    extra = QByteArray(reinterpret_cast<const char*>(pData), size);
    byte_size += size;
  }

  return byte_size;
}

namespace {
/** A buoy's colours in chart notation (INT 1 Q): "black-yellow" is "BY". */
QString buoyColours(quint8 colour) {
  QString text;
  for (const QString& name : QString(CGarminMarine::colourName(colour)).split('-', Qt::SkipEmptyParts)) {
    text += name == "black" ? "B" : (name == "orange" ? "Or" : name.left(1).toUpper());
  }
  return text;
}
}  // namespace

QString CGarminPoint::getLabelText() const {
  if (labels.isEmpty()) {
    return QString();
  }

  // BaseCamp labels an obstruction with its height: "Obstruction - 3"
  if ((type & 0x1FF00) == 0x10400 && !extra.isEmpty()) {
    const CGarminMarine marine = CGarminMarine::decode(type, CGarminMarine::Kind::ePoint, extra);
    if (marine.depth.valid) {
      // BaseCamp leaves out the unit on the map
      return IGarminStrTbl::displayText(labels) + " - " + distanceText(marine.depth).section(' ', 0, 0);
    }
  }

  QString name;
  qreal feet = 0;
  bool hasHeight = false;
  // a depth or height point's label is a plain number
  if ((type == 0x6200) || (type == 0x6300)) {
    feet = IGarminStrTbl::displayText(labels.first()).toDouble(&hasHeight);
  }
  if (!hasHeight) {
    hasHeight = IGarminStrTbl::splitHeight(labels.first(), name, feet);
  }
  if (!hasHeight) {
    return IGarminStrTbl::displayText(labels);
  }
  // BaseCamp shows a height after 0x1f only on depth/height points and landmarks 0x64-0x66, hides it elsewhere
  const bool showsHeight = (type == 0x6200) || (type == 0x6300) || ((type >= 0x6400) && (type <= 0x66FF));
  if (!showsHeight) {
    return name;
  }

  QString val, unit;
  IUnit::self().feet2elevation(feet, val, unit);
  const QString height = val + " " + unit;
  return name.isEmpty() ? height : name + ", " + height;
}

QString CGarminPoint::getName() const {
  if (labels.isEmpty()) {
    return QString();
  }
  if ((type == 0x6200) || (type == 0x6300)) {
    return getLabelText();
  }

  QString name;
  qreal feet = 0;
  if (IGarminStrTbl::splitHeight(labels.first(), name, feet)) {
    return name;
  }
  return IGarminStrTbl::displayText(labels.first());
}

QString CGarminPoint::distanceText(const CGarminMarine::distance_t& d) {
  qreal val = 0;
  QString unit;
  IUnit::self().meter2elevation(d.metric ? d.value : d.value * 0.3048, val, unit);
  const qreal rounded = qRound(val * 10) / 10.0;
  return QString::number(rounded, 'f', qAbs(rounded - qRound(rounded)) < 0.05 ? 0 : 1) + " " + unit;
}

QString CGarminPoint::getInfoText() const {
  QStringList lines;
  if (hasLabel()) {
    lines << getLabelText();
  }
  const CGarminMarine marine = CGarminMarine::decode(type, CGarminMarine::Kind::ePoint, extra);
  if (marine.isEmpty()) {
    return lines.join("\n");
  }

  if ((type & 0x1FF00) == 0x10200 && marine.colour != 0) {
    lines << tr("Colour: %1").arg(buoyColours(marine.colour));
  }
  const QString light = marine.lightCharacteristic();
  if (!light.isEmpty()) {
    lines << tr("Light: %1").arg(light);
  }
  for (const QString& sector : marine.sectorTexts()) {
    lines << tr("Sector: %1").arg(sector);
  }
  if (marine.heightAboveFoundation.valid) {
    lines << tr("Height above foundation: %1").arg(distanceText(marine.heightAboveFoundation));
  }
  if (marine.leadingAngle >= 0) {
    lines << tr("Leading angle: %1°").arg(marine.leadingAngle);
  }
  if (marine.racon) {
    lines << tr("Racon");
  }
  if (marine.depth.valid) {
    const QString value = distanceText(marine.depth);
    lines << (CGarminMarine::isHeight(type, CGarminMarine::Kind::ePoint) ? tr("Height: %1").arg(value)
                                                                         : tr("Depth: %1").arg(value));
  }
  static const char* positions[] = {nullptr,
                                    nullptr,
                                    QT_TRANSLATE_NOOP("CGarminPoint", "doubtful"),
                                    QT_TRANSLATE_NOOP("CGarminPoint", "existence doubtful"),
                                    QT_TRANSLATE_NOOP("CGarminPoint", "approximate"),
                                    QT_TRANSLATE_NOOP("CGarminPoint", "reported")};
  if (marine.position < std::size(positions) && positions[marine.position] != nullptr) {
    lines << tr("Position: %1").arg(tr(positions[marine.position]));
  }
  const QStringList facilities = marine.facilityNames();
  if (!facilities.isEmpty()) {
    lines << tr("Facilities: %1").arg(facilities.join(", "));
  }
  if (!localDesignator.isEmpty()) {
    lines << tr("Local designator: %1").arg(localDesignator);
  }
  if (!internationalDesignator.isEmpty()) {
    lines << tr("International designator: %1").arg(internationalDesignator);
  }
  if (!note.isEmpty()) {
    lines << note;
  }
  return lines.join("\n");
}
