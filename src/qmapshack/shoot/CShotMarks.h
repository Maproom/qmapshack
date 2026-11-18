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

#ifndef CSHOTMARKS_H
#define CSHOTMARKS_H

#include <QList>
#include <QPoint>
#include <QRect>
#include <QString>
#include <QStringList>

class QImage;
class QJsonArray;
class QWidget;

/** @brief Marks on documentation images: as the shot file holds them, where they land, how they are drawn. */
namespace CShotMarks {
/** @brief One mark of a shot's `mark` list. */
struct mark_t {
  QString root;        /**< `main`, `menu`, `window` or `dock:<address>` */
  QString widget;      /**< address below the root; empty is the root */
  QString part;        /**< empty, `handle`, `branch`, `title`, `action` or `splitter` */
  QString row;         /**< the tree row of a `branch`, as CShotAddress::rowPathOf() */
  QString action;      /**< the objectName of an `action` */
  QStringList between; /**< the two addresses a `splitter` divides */
  QString shape; /**< `frame`, `disc`, `arrow` or `cursor`; empty is a disc for a knob, a tree arrow or a splitter, else
                    a frame */
  QPoint tail;   /**< an arrow's tail from its target's centre, image pixels, snapped to 45° */
  QString cursor; /**< `pointer`, `hand` or `resize` */
  QString name;   /**< what the writer reads */
};

/** @brief Where a mark lands. */
struct placed_t {
  mark_t mark;
  QRect area;                              /**< global coordinates */
  Qt::Orientation arrows = Qt::Horizontal; /**< a resize cursor's direction */
  QWidget* window = nullptr;               /**< the window the target lies in */
};

/** @return why @p marks cannot be read, empty when @p out holds them */
QString parse(const QJsonArray& marks, QList<mark_t>& out);

/**
   @param shot  the photographed widget: the `window` root, and the menu when no popup is open
   @return why @p mark is not on screen, empty when @p out holds where it lands
 */
QString place(const mark_t& mark, QWidget* shot, placed_t& out);

/**
   @brief Draw @p placed onto @p image, rendered from @p w; nothing is drawn when one fails.

   @param keep  the part of the image that is kept, invalid for all of it; a mark must lie inside it
   @param drawn  set to each mark's bounds in image pixels
   @return why a mark cannot be drawn, empty when all are
 */
QString paint(QImage& image, const QList<placed_t>& placed, const QWidget* w, const QRect& keep,
              QList<QRect>* drawn = nullptr);

/** @return @p mark's name, or a description built from it */
QString describe(const mark_t& mark);

/** @return @p tail turned to the nearest multiple of 45°, its length kept */
QPoint snap(const QPoint& tail);
}  // namespace CShotMarks

#endif  // CSHOTMARKS_H
