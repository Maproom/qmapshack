/**********************************************************************************************
    Copyright (C) 2026 Jens Mölzer <moelzerjens@gmail.com>

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

#ifndef BLEND2DUTIL_H
#define BLEND2DUTIL_H

#include <blend2d/blend2d.h>

#include <QImage>
#include <cstring>

/**
   @brief Deep-copy a QImage into an owning premultiplied BLImage.

   @param img source image in any format; inputs are converted on the fly if necessary
   @return the owning image, or an empty BLImage on failure
 */
inline BLImage toOwnedBLImage(const QImage& img) {
  BLImage out;
  if (img.isNull()) {
    return out;
  }

  QImage storage;
  const QImage& src = (img.format() == QImage::Format_ARGB32_Premultiplied)
                          ? img
                          : (storage = img.convertToFormat(QImage::Format_ARGB32_Premultiplied));

  if (out.create(src.width(), src.height(), BL_FORMAT_PRGB32) != BL_SUCCESS) {
    return out;
  }

  BLImageData dst;
  if (out.make_mutable(&dst) != BL_SUCCESS) {
    return BLImage();
  }

  const qsizetype bpl_src = src.bytesPerLine();
  const qsizetype bpl_dst = static_cast<qsizetype>(dst.stride);
  const qsizetype rowBytes = qMin(bpl_src, bpl_dst);
  const uchar* const s = src.constBits();
  uchar* const d = static_cast<uchar*>(dst.pixel_data);
  for (int y = 0; y < src.height(); ++y) {
    memcpy(d + bpl_dst * y, s + bpl_src * y, static_cast<size_t>(rowBytes));
  }
  return out;
}

#endif  // BLEND2DUTIL_H
