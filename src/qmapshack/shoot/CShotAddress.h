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

#ifndef CSHOTADDRESS_H
#define CSHOTADDRESS_H

#include <QModelIndex>
#include <QString>
#include <optional>

class CGisListWks;
class QAbstractItemModel;
class QAbstractItemView;
class QPoint;
class QTreeWidget;
class QTreeWidgetItem;
class QWidget;

/** @brief Names for what a recording acted on, and their resolvers. */
namespace CShotAddress {
/**
   @brief The address of @p widget below @p root.

   The objectName when unique from @p root, else parent address, class and index among siblings: `IMapList/QMenu#0`.

   @return empty for @p root itself, nothing for a widget that cannot be addressed
 */
std::optional<QString> addressOf(const QWidget* root, const QWidget* widget);

/** @return the widget @p address names below @p root, or nullptr */
QWidget* resolve(QWidget* root, const QString& address);

/**
   @brief The path of a workspace item: `Example/trk:Track`, a project by its name alone; folders are skipped.

   @return empty for anything but a project or GIS item, or when the path would not resolve to it
 */
QString itemPathOf(const QTreeWidgetItem* item);

/** @return the workspace item @p path names, or nullptr */
QTreeWidgetItem* resolveItemPath(const CGisListWks& list, const QString& path);

/**
   @brief The path of a database or map tree row: names from the top level down, `Example/Projects`.

   @return empty for a row of another tree, or when the path would not resolve to it
 */
QString namePathOf(const QTreeWidgetItem* item);

/** @return the row of @p tree @p path names, or nullptr */
QTreeWidgetItem* resolveNamePath(const QTreeWidget& tree, const QString& path);

/** @return the row numbers from the top level down and the column, `0/2:1` */
QString rowPathOf(const QModelIndex& index);

/** @return the index @p path names in @p model, invalid when there is none */
QModelIndex resolveRowPath(const QAbstractItemModel& model, const QString& path);

/**
   @brief What sits at @p pos of @p widget, for a replay to compare.

   @return a header section, a workspace item path, a row path or a child's address; empty for @p widget itself
 */
QString hitAt(const QWidget* widget, const QPoint& pos);

/** @return the item view @p widget is, or whose viewport it is; nullptr otherwise */
const QAbstractItemView* itemViewOf(const QWidget* widget);
}  // namespace CShotAddress

#endif  // CSHOTADDRESS_H
