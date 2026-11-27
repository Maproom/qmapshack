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
#include <QStringList>
#include <QtGlobal>

#include "map/garmin/CGarminMarine.h"

class CGarminTile;

class CGarminPoint {
  Q_DECLARE_TR_FUNCTIONS(CGarminPoint)

 public:
  CGarminPoint() = default;

  quint32 decode(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, const quint8* pData);
  quint32 decode2(qint32 iCenterLon, qint32 iCenterLat, quint32 shift, const quint8* pData, const quint8* pEnd);

  /** The label as shown on the map, a height in feet converted to the user's unit. */
  QString getLabelText() const;
  /** The label without a height after 0x1f; a depth or height point's converted height. */
  QString getName() const;
  /** The label and, for a marine point, its extra data: light in chart notation, depth, facilities, ... */
  QString getInfoText() const;

  bool hasLabel() const { return !labels.isEmpty(); }

  /** A marine depth or height in the user's unit, one decimal where the value has one: "5.5 m". */
  static QString distanceText(const CGarminMarine::distance_t& d);

  quint32 type = 0;
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
};
#endif  // CGARMINPOINT_H
