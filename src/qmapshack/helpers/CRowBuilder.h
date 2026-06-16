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

#ifndef CROWBUILDER_H
#define CROWBUILDER_H

#include <QRect>

/// Margin used throughout the tree-item delegates for spacing icons, tool buttons and text.
constexpr int kMargin = 1;
/// Outer inset applied to all four sides of the item rect before layout.
constexpr int kCellPad = 2 * kMargin;
/// Gap inserted between the icon, text column, and each tool button.
constexpr int kInnerGap = 2 * kMargin;

/**
   @brief Lays out a standard tree-row into icon, button, and text zones.

   Construct once per getRectangles*() call with the item's full opt.rect. Call
   takeLeft() / takeRight() to carve named rects from the working area; then call
   nameSlice() / statusSlice() to derive the text rects from whatever remains.

   All spatial tuning lives in two parameters:
   - @p cellPad  — outer inset on all four sides (= kCellPad)
   - @p innerGap — gap between icon, text column, and each tool button (= kInnerGap)
 */
class CRowBuilder {
 public:
  /**
     @param optRect  Full item rect from QStyleOptionViewItem
     @param cellPad  Outer inset applied to all four sides
     @param innerGap Gap inserted after each takeLeft/takeRight call
   */
  CRowBuilder(const QRect& optRect, int cellPad, int innerGap);

  /** Height of the inner working area (use for square icon / button sizing). */
  int height() const;

  /** Carve @p width pixels from the left edge and advance the working area by innerGap. */
  QRect takeLeft(int width);

  /** Carve @p width pixels from the right edge and retreat the working area by innerGap.
      The returned rect spans the full working height. */
  QRect takeRight(int width);

  /** Same as takeRight(width) but the returned rect is @p height pixels tall (top-aligned).
      Use when the button should not fill the full row height, e.g. a square button sized
      to the name font height in a two-line row. */
  QRect takeRight(int width, int height);

  /** Carve a square tool button sized so that CDraw::drawToolButton renders its icon at
      exactly @p iconSize × @p iconSize pixels (compensates for drawToolButton's icon inset). */
  QRect takeButton(int iconSize);

  /** Top strip of the remaining centre area, @p fontHeight pixels tall. */
  QRect nameSlice(int fontHeight) const;

  /** Bottom strip of the remaining centre area, @p fontHeight pixels tall.
      For rows where the button spans the full row height this gives the correct
      (button-narrowed) width. Use fullStatusSlice() when buttons are shorter. */
  QRect statusSlice(int fontHeight) const;

  /** Snapshot the current remaining rect as the full-width status column.
      Call this after taking the icon and before taking any buttons, so that
      fullStatusSlice() can return a rect that spans under the buttons. */
  void markStatusColumn();

  /** Bottom strip of the status column snapshotted by markStatusColumn(),
      @p fontHeight pixels tall. The width is not narrowed by buttons. */
  QRect fullStatusSlice(int fontHeight) const;

  /** The current remaining centre rect (shrinks with each takeLeft / takeRight call). */
  QRect remaining() const;

  /**
     @brief Compute the matching sizeHint row height from the same parameters.
     @param cellPad  Outer inset (same value passed to the constructor)
     @param nameH    Height of the name font
     @param statusH  Height of the status font, or 0 for single-line rows
   */
  static int rowHeight(int cellPad, int nameH, int statusH = 0);

 private:
  QRect r_;
  QRect statusColumn_;
  int innerGap_;
};

#endif  // CROWBUILDER_H
