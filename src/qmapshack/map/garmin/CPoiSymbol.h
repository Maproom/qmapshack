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
#ifndef CPOISYMBOL_H
#define CPOISYMBOL_H

#include <blend2d/blend2d.h>

#include <QPointF>
#include <QRectF>

/**
   @brief Symbols of Garmin's standard POI types for points a TYP does not style.

   Icons from Maki, Temaki and osmic (CC0, `src/icons/garmin-poi`): white on a rounded square in the colour of the
   type's group; geographic points (0x64-0x66) in their colour with a light rim. Each symbol is rendered once per
   pixel ratio. Coordinates are logical pixels, the symbol centred on the position.
 */
class CPoiSymbol {
 public:
  /** Whether a point of @p type gets a symbol. */
  static bool hasSymbol(quint32 type);
  /** Draw the symbol of a point of @p type centred on @p pos. */
  static void draw(BLContext& ctx, const QPointF& pos, quint32 type);
  /** The area the symbol covers, relative to its position. */
  static QRectF bounds(quint32 type);
};

#endif  // CPOISYMBOL_H
