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
#include <QPointF>
#include <QRect>
#include <QString>
#include <QStringList>
#include <functional>

class QImage;
class QJsonArray;
class QSlider;
class QDockWidget;
class QTabBar;
class QWidget;

/** @brief Marks on documentation images: as the shot file holds them, where they land, how they are drawn. */
namespace CShotMarks {
/** @brief One mark of a shot's `mark` list. */
struct mark_t {
  QString root;        /**< `main`, `menu`, `window` or `dock:<address>` */
  QString widget;      /**< address below the root; empty is the root */
  QString part;        /**< empty, `handle`, `branch`, `title`, `action`, `tab`, `button`, `splitter` or `point` */
  QString row;         /**< the tree row of a `branch` or `button`, as CShotAddress::rowPathOf() */
  QString action;      /**< the objectName of an `action` */
  QString button;      /**< a row's `button`, named as a step names it */
  QStringList between; /**< the two addresses a `splitter` divides */
  QPointF point;       /**< a map `point`: longitude, latitude [°] */
  QString shape;  /**< `frame`, `disc`, `arrow` or `cursor`; empty is a disc for a knob, a tree arrow, a splitter or a
                     point, else a frame */
  QPoint tail;    /**< an arrow's tail from its target's centre, image pixels, snapped to 45° */
  QString cursor; /**< `pointer`, `hand` or `resize` */
  QString name;   /**< what the writer reads */
};

/** @brief Whether a mark's target is on screen. */
enum class found_e {
  eFound,   /**< placed */
  eClosed,  /**< there, but not on screen now: a closed menu, a hidden page, a collapsed row */
  eMissing, /**< no such widget, entry or row */
  eWrong    /**< the mark does not fit its target */
};

/** @brief Where a mark lands. */
struct placed_t {
  mark_t mark;
  found_e found = found_e::eWrong;
  QRect area;                              /**< global coordinates */
  Qt::Orientation arrows = Qt::Horizontal; /**< a resize cursor's direction */
  QWidget* window = nullptr;               /**< the window the target lies in */
};

/** @return why @p marks cannot be read, empty when @p out holds them; else @p out holds those before the first bad one
 */
QString parse(const QJsonArray& marks, QList<mark_t>& out);

/**
   @param shot  the photographed widget: the `window` root, and the menu when no popup is open
   @return why @p mark is not on screen, empty when @p out holds where it lands; @p out says which kind either way
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

/** @return where @p slider's knob is, in its own coordinates */
QRect sliderHandle(const QSlider& slider);

/** @return false when a disc on @p area would be cut apart by the hole its target leaves: a tab, a field */
bool discFits(const QRect& area);

/**
   @brief @p image of @p w with @p placed drawn, cut to @p keep, invalid for all of it.

   A part whose marks reach past it is cut from its window's image instead, with room for them.
   @param windowImage  renders @p w's window; called only when the marks need that room
   @return why it cannot be drawn, empty when @p out holds the image
 */
QString framed(const QImage& image, QWidget* w, const QRect& keep, const QList<placed_t>& placed,
               const std::function<QImage()>& windowImage, QImage& out);

/** @return the index of @p dock's tab in the main window's dock tab bar @p bar, -1 with @p bar null for none */
qint32 dockTab(const QDockWidget* dock, const QTabBar*& bar);

/** @return the docker of tab @p index of a main window's dock tab bar @p bar, nullptr for none */
QDockWidget* dockOfTab(const QTabBar* bar, qint32 index);

/** @return @p mark's name, or a description built from it */
QString describe(const mark_t& mark);

/** @return what @p placed draws, in @p w's coordinates, a frame drawn around its target */
QRect extent(const QList<placed_t>& placed, const QWidget* w);

/** @return the marker @p kind (frame, disc, arrow, pointer, hand, resize) drawn as it is drawn on an image */
QImage icon(const QString& kind, qint32 side);

/** @return @p tail turned to the nearest multiple of 45°, its length kept */
QPoint snap(const QPoint& tail);
}  // namespace CShotMarks

#endif  // CSHOTMARKS_H
