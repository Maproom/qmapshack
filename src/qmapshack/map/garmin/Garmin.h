/**********************************************************************************************
    Copyright (C) 2008 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef GARMIN_H
#define GARMIN_H

#include <QtGlobal>

#ifdef __MINGW32__
#undef LP
#endif

#define GARMIN_DEG(x) ((x) < 0x800000 ? (qreal)(x) * 360.0 / 16777216.0 : (qreal)((x) - 0x1000000) * 360.0 / 16777216.0)
#define GARMIN_RAD(x) \
  ((x) < 0x800000 ? (qreal)(x) * (2 * M_PI) / 16777216.0 : (qreal)((x) - 0x1000000) * (2 * M_PI) / 16777216.0)
typedef quint8 quint24[3];

/**
   @brief The size of an extended object's extra data, from its first byte

   The forms mkgmap writes (ExtTypeAttributes): 0x00-0x7F one byte, 0x80-0x9F two, 0xA0-0xBF three,
   0xE0-0xFF with a second byte (n << 1) | 1 followed by n bytes.

   @return the size in bytes; 0 for any other form or if it runs past @p pEnd
 */
inline quint32 garminExtraDataSize(const quint8* pData, const quint8* pEnd) {
  if (pData >= pEnd) {
    return 0;
  }
  const quint8 b = pData[0];
  quint32 size = 0;
  if (b < 0x80) {
    size = 1;
  } else if (b < 0xA0) {
    size = 2;
  } else if (b < 0xC0) {
    size = 3;
  } else if (b >= 0xE0 && pEnd - pData >= 2 && (pData[1] & 0x01)) {
    size = (pData[1] >> 1) + 2;
  }
  return size <= quint32(pEnd - pData) ? size : 0;
}
#endif  // GARMIN_H
