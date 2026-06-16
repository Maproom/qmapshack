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

#include "helpers/CRowBuilder.h"

CRowBuilder::CRowBuilder(const QRect& optRect, int cellPad, int innerGap)
    : r_(optRect.adjusted(cellPad, cellPad, -cellPad, -cellPad)), innerGap_(innerGap) {}

int CRowBuilder::height() const { return r_.height(); }

QRect CRowBuilder::takeLeft(int width) {
  const QRect result(r_.left(), r_.top(), width, r_.height());
  r_.setLeft(r_.left() + width + innerGap_);
  return result;
}

QRect CRowBuilder::takeRight(int width) {
  const QRect result(r_.right() - width, r_.top(), width, r_.height());
  r_.setRight(r_.right() - width - innerGap_);
  return result;
}

QRect CRowBuilder::takeRight(int width, int height) {
  const QRect result(r_.right() - width, r_.top(), width, height);
  r_.setRight(r_.right() - width - innerGap_);
  return result;
}

QRect CRowBuilder::takeButton(int iconSize) {
  // CDraw::drawToolButton insets its icon by kMargin on every side (left+right = 2*kMargin,
  // top+bottom = 2*kMargin), so the button rect must be (iconSize + 4*kMargin) square for
  // the icon to render at exactly iconSize×iconSize pixels.
  const int size = iconSize + 4 * kMargin;
  return takeRight(size, size);
}

QRect CRowBuilder::nameSlice(int fontHeight) const { return QRect(r_.left(), r_.top(), r_.width(), fontHeight); }

QRect CRowBuilder::statusSlice(int fontHeight) const {
  return QRect(r_.left(), r_.bottom() - fontHeight, r_.width(), fontHeight);
}

void CRowBuilder::markStatusColumn() { statusColumn_ = r_; }

QRect CRowBuilder::fullStatusSlice(int fontHeight) const {
  return QRect(statusColumn_.left(), statusColumn_.bottom() - fontHeight, statusColumn_.width(), fontHeight);
}

QRect CRowBuilder::remaining() const { return r_; }

int CRowBuilder::rowHeight(int cellPad, int nameH, int statusH) {
  // Layout: top-cellPad | kMargin (top text gap) | nameH | [2*kMargin (inter-line gap) | statusH] | bottom-cellPad
  return 2 * cellPad + kMargin + nameH + (statusH > 0 ? 2 * kMargin + statusH : 0);
}
