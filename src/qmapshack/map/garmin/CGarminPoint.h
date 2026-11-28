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
#ifndef CGARMINPOINT_H
#define CGARMINPOINT_H

#include <QByteArray>
#include <QCoreApplication>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QSize>
#include <QStringList>
#include <QVector>
#include <QtGlobal>

#include "map/garmin/CGarminMarine.h"

class CGarminTile;

class CGarminPoint {
  Q_DECLARE_TR_FUNCTIONS(CGarminPoint)

 public:
  /** One facility at a highway exit (LBL10). */
  struct exit_facility_t {
    QString description;
    quint8 type = 0;       /**< 4 bit, as mkgmap's exit:facility writes it */
    quint8 direction = 7;  /**< index into "NSEWIOB", 7 none */
    quint8 facilities = 0; /**< bit set, as mkgmap's exit:facility writes it */
  };

  CGarminPoint() = default;

  quint32 decode(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, const quint8* pData);
  quint32 decode2(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, const quint8* pData, const quint8* pEnd);

  /** The label as shown on the map, a height in feet converted to the user's unit. */
  QString getLabelText() const;
  /** The label without a height after 0x1f; a depth or height point's converted height. */
  QString getName() const;
  /** The label and, for a marine point, its extra data: light in chart notation, depth, facilities, ... */
  QString getInfoText() const;
  /** Address and phone as BaseCamp shows them: "12 Main Street", "Hilltown, Upper Valley 87654", phone; one per line */
  QString getAddressText() const;
  /** A highway exit: its highway, exit_to, overnight parking and one line per facility; empty if not an exit */
  QString getExitText() const;
  /** A facility as BaseCamp shows it: "<description>: <type>, <direction>" */
  static QString facilityText(const exit_facility_t& facility);

  bool hasLabel() const { return !labels.isEmpty(); }

  /**
     @brief Where a point's label goes, as BaseCamp places it: 1 px right of the symbol, the text's bottom on the point

     @param pos     the point in screen pixels
     @param symbol  the symbol drawn for the point in screen pixels, empty at @p pos for none
     @param text    advance and height (ascent plus descent) of the label in its font
     @return the text box
   */
  static QRect labelBox(const QPointF& pos, const QRectF& symbol, const QSize& text);

  /** A marine depth or height in the user's unit, one decimal where the value has one: "5.5 m". */
  static QString distanceText(const CGarminMarine::distance_t& d);

  quint32 type = 0;
  quint16 family = 0; /**< map family of the object's tile, from the MPS map list; 0 if unknown */
  bool isLbl6 = false;
  bool hasSubType = false;

  QPointF pos;

  QStringList labels;

  quint32 lbl_ptr = 0xFFFFFFFF;

  /** Extra data of an extended point, see CGarminMarine; empty if none. */
  QByteArray extra;
  /** Note, local and international designator of a light or buoy, read from LBL; empty if none. */
  QString note;
  QString localDesignator;
  QString internationalDesignator;
  /** Address and phone of a POI, read from its LBL6 record; empty if none. */
  QString houseNumber;
  QString street;
  QString city;
  QString region;
  QString country;
  QString zip;
  QString phone;
  /** A highway exit: its highway, its description (OSM exit_to) and facilities */
  bool isExit = false;
  QString exitHighway;
  QString exitTo;
  bool exitOvernightParking = false;
  QVector<exit_facility_t> exitFacilities;
};
#endif  // CGARMINPOINT_H
