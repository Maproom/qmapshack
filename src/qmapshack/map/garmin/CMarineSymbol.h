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
#ifndef CMARINESYMBOL_H
#define CMARINESYMBOL_H

#include <blend2d/blend2d.h>

#include <QPointF>
#include <QRectF>

#include "map/garmin/CGarminMarine.h"

/**
   @brief Chart symbols of marine points without a TYP, composed from INT 1 parts.

   Lights: a flare from a star (P1). Buoys: the shape by type leaning on a waterline with a position circle
   (Q20-Q26), filled in the buoy's colours in bands from top to bottom (Q4), a topmark by category (Q9, Q130).
   Beacons: an upright bar on the position circle (Q80). Obstructions: a dotted danger circle (K40).
   Coordinates are logical pixels; the position is the point given.
 */
class CMarineSymbol {
 public:
  /** Whether a point of @p type gets a symbol. */
  static bool hasSymbol(quint32 type);
  /** Draw the symbol of a point of @p type at @p pos. */
  static void draw(BLContext& ctx, const QPointF& pos, quint32 type, const CGarminMarine& marine);
  /** The area the symbol covers, relative to its position. */
  static QRectF bounds(quint32 type);
};

#endif  // CMARINESYMBOL_H
