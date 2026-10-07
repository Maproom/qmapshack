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

#include "shoot/CShotAddress.h"

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QList>
#include <QPoint>
#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWidget>

#include "gis/CGisListDB.h"
#include "gis/CGisListWks.h"
#include "gis/IDBItem.h"
#include "gis/IWksItem.h"
#include "gis/prj/IGisProject.h"
#include "map/CMapList.h"
#include "map/IMapItem.h"

namespace {
/** @return the direct children of @p parent of exactly this class, in construction order */
QList<QWidget*> childrenOfClass(const QWidget* parent, const QString& className) {
  QList<QWidget*> matches;
  const QList<QWidget*>& children = parent->findChildren<QWidget*>(Qt::FindDirectChildrenOnly);
  for (QWidget* child : children) {
    if (QString::fromLatin1(child->metaObject()->className()) == className) {
      matches << child;
    }
  }
  return matches;
}

/** @return true for a name usable as an address part; `key_` names are clock-derived map views, unstable per run */
bool isAddressName(const QString& name) {
  return !name.isEmpty() && !name.contains('/') && !name.contains('#') && !name.startsWith("key_");
}

/** @return `trk` for a track and so on */
QString typeTag(const QTreeWidgetItem* item) {
  switch (item->type()) {
    case IWksItem::eTypeWpt:
      return "wpt";
    case IWksItem::eTypeTrk:
      return "trk";
    case IWksItem::eTypeRte:
      return "rte";
    case IWksItem::eTypeOvl:
      return "area";
    default:
      return QString();
  }
}

QString nameOf(const QTreeWidgetItem* item) {
  const IWksItem* wks = dynamic_cast<const IWksItem*>(item);
  return (nullptr == wks) ? QString() : wks->getName();
}

QString rowNameOf(const QTreeWidgetItem* item) {
  if (const IDBItem* db = dynamic_cast<const IDBItem*>(item); nullptr != db) {
    return db->getName();
  }
  const IMapItem* map = dynamic_cast<const IMapItem*>(item);
  return (nullptr == map) ? QString() : map->getName();
}

/** @return the first item below @p parent, depth first, with this type tag and name */
QTreeWidgetItem* findItem(QTreeWidgetItem* parent, const QString& tag, const QString& name) {
  for (int i = 0; i < parent->childCount(); i++) {
    QTreeWidgetItem* child = parent->child(i);
    if (typeTag(child) == tag && nameOf(child) == name) {
      return child;
    }
    if (QTreeWidgetItem* found = findItem(child, tag, name); nullptr != found) {
      return found;
    }
  }
  return nullptr;
}
}  // namespace

std::optional<QString> CShotAddress::addressOf(const QWidget* root, const QWidget* widget) {
  if (nullptr == root || nullptr == widget) {
    return std::nullopt;
  }
  if (widget == root) {
    return QString();
  }

  const QString& name = widget->objectName();
  if (isAddressName(name) && root->findChild<QWidget*>(name) == widget) {
    return name;
  }

  const QWidget* parent = widget->parentWidget();
  if (nullptr == parent) {
    return std::nullopt;
  }
  const std::optional<QString>& prefix = addressOf(root, parent);
  if (!prefix.has_value()) {
    return std::nullopt;
  }

  const QString& className = QString::fromLatin1(widget->metaObject()->className());
  const qsizetype index = childrenOfClass(parent, className).indexOf(const_cast<QWidget*>(widget));
  const QString& part = QString("%1#%2").arg(className).arg(index);
  return prefix->isEmpty() ? part : (*prefix + "/" + part);
}

QWidget* CShotAddress::resolve(QWidget* root, const QString& address) {
  QWidget* current = root;
  if (nullptr == root || address.isEmpty()) {
    return current;
  }

  const QStringList& parts = address.split('/');
  for (const QString& part : parts) {
    if (nullptr == current) {
      break;
    }
    if (part.contains('#')) {
      bool ok = false;
      const qint32 index = part.section('#', 1).toInt(&ok);
      current = ok ? childrenOfClass(current, part.section('#', 0, 0)).value(index, nullptr) : nullptr;
    } else {
      current = current->findChild<QWidget*>(part);
    }
  }
  return current;
}

QString CShotAddress::itemPathOf(const QTreeWidgetItem* item) {
  const CGisListWks* list = (nullptr == item) ? nullptr : dynamic_cast<const CGisListWks*>(item->treeWidget());
  if (nullptr == list) {
    return QString();
  }

  QString path;
  if (nullptr != dynamic_cast<const IGisProject*>(item)) {
    path = nameOf(item);
  } else if (!typeTag(item).isEmpty()) {
    const QTreeWidgetItem* project = item;
    while (nullptr != project->parent()) {
      project = project->parent();
    }
    path = QString("%1/%2:%3").arg(nameOf(project), typeTag(item), nameOf(item));
  }
  // A '/' in a project's name, or a name used twice, finds another item.
  return (!path.isEmpty() && resolveItemPath(*list, path) == item) ? path : QString();
}

QTreeWidgetItem* CShotAddress::resolveItemPath(const CGisListWks& list, const QString& path) {
  if (path.isEmpty()) {
    return nullptr;
  }

  const QString& projectName = path.section('/', 0, 0);
  QTreeWidgetItem* project = nullptr;
  for (int i = 0; i < list.topLevelItemCount() && nullptr == project; i++) {
    if (nameOf(list.topLevelItem(i)) == projectName) {
      project = list.topLevelItem(i);
    }
  }
  if (nullptr == project || !path.contains('/')) {
    return project;
  }

  const QString& rest = path.section('/', 1);
  return rest.contains(':') ? findItem(project, rest.section(':', 0, 0), rest.section(':', 1)) : nullptr;
}

QString CShotAddress::namePathOf(const QTreeWidgetItem* item) {
  const QTreeWidget* tree = (nullptr == item) ? nullptr : item->treeWidget();
  if (nullptr == dynamic_cast<const CGisListDB*>(tree) && nullptr == dynamic_cast<const CMapTreeWidget*>(tree)) {
    return QString();
  }

  QStringList names;
  for (const QTreeWidgetItem* row = item; nullptr != row; row = row->parent()) {
    const QString& name = rowNameOf(row);
    if (name.isEmpty()) {
      return QString();
    }
    names.prepend(name);
  }
  const QString& path = names.join('/');
  // A '/' in a name, or a name used twice below the same parent, finds another row.
  return (resolveNamePath(*tree, path) == item) ? path : QString();
}

QTreeWidgetItem* CShotAddress::resolveNamePath(const QTreeWidget& tree, const QString& path) {
  if (path.isEmpty()) {
    return nullptr;
  }

  QTreeWidgetItem* row = tree.invisibleRootItem();
  const QStringList& names = path.split('/');
  for (const QString& name : names) {
    QTreeWidgetItem* found = nullptr;
    for (int i = 0; i < row->childCount() && nullptr == found; i++) {
      if (rowNameOf(row->child(i)) == name) {
        found = row->child(i);
      }
    }
    if (nullptr == found) {
      return nullptr;
    }
    row = found;
  }
  return row;
}

QString CShotAddress::rowPathOf(const QModelIndex& index) {
  QStringList rows;
  for (QModelIndex i = index; i.isValid(); i = i.parent()) {
    rows.prepend(QString::number(i.row()));
  }
  return QString("%1:%2").arg(rows.join('/')).arg(index.column());
}

QModelIndex CShotAddress::resolveRowPath(const QAbstractItemModel& model, const QString& path) {
  bool ok = false;
  const int column = path.section(':', 1).toInt(&ok);
  const QStringList& rows = path.section(':', 0, 0).split('/');
  if (!ok || path.section(':', 0, 0).isEmpty()) {
    return QModelIndex();
  }

  QModelIndex index;
  for (qsizetype i = 0; i < rows.size(); i++) {
    const int row = rows[i].toInt(&ok);
    if (!ok) {
      return QModelIndex();
    }
    // Every row above the last is a parent, and a parent is always column 0.
    index = model.index(row, (i + 1 == rows.size()) ? column : 0, index);
    if (!index.isValid()) {
      return QModelIndex();
    }
  }
  return index;
}

QString CShotAddress::hitAt(const QWidget* widget, const QPoint& pos) {
  if (const QAbstractItemView* view = itemViewOf(widget); nullptr != view) {
    const QPoint& inViewport = view->viewport()->mapFromGlobal(widget->mapToGlobal(pos));
    if (const QHeaderView* header = qobject_cast<const QHeaderView*>(view); nullptr != header) {
      return QString::number(header->logicalIndexAt(inViewport));
    }
    const QModelIndex& index = view->indexAt(inViewport);
    if (!index.isValid()) {
      return QString();
    }
    const QTreeWidget* tree = qobject_cast<const QTreeWidget*>(view);
    const QString& path = (nullptr == tree) ? QString() : itemPathOf(tree->itemAt(inViewport));
    return path.isEmpty() ? rowPathOf(index) : path;
  }
  const QWidget* child = widget->childAt(pos);
  return (nullptr == child) ? QString() : addressOf(widget, child).value_or(QString());
}

const QAbstractItemView* CShotAddress::itemViewOf(const QWidget* widget) {
  if (nullptr == widget) {
    return nullptr;
  }
  if (const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>(widget); nullptr != view) {
    return view;
  }
  const QAbstractItemView* parent = qobject_cast<const QAbstractItemView*>(widget->parentWidget());
  return (nullptr != parent && parent->viewport() == widget) ? parent : nullptr;
}
