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

#include "shoot/CShotRegistry.h"

#include <QDebug>
#include <QWidget>

CShotRegistry& CShotRegistry::self() {
  // Function-local: filled from other translation units' static initializers.
  static CShotRegistry instance;
  return instance;
}

bool CShotRegistry::addExposure(const QString& id, const QMetaObject* metaObject, const exposure_factory_t& factory,
                                const QString& description) {
  if (exposures.contains(id)) {
    duplicateIds << id;
    return false;
  }
  exposures.insert(id, {factory, metaObject, description});
  return true;
}

QString CShotRegistry::exposureOf(const QWidget* widget) const {
  if (nullptr == widget) {
    return QString();
  }
  // A subclass is another dialog.
  for (auto it = exposures.constBegin(); it != exposures.constEnd(); ++it) {
    if (it->metaObject == widget->metaObject()) {
      return it.key();
    }
  }
  return QString();
}

QWidget* CShotRegistry::buildExposure(const QString& id, CShotContext& ctx, QWidget* parent) const {
  const auto it = exposures.constFind(id);
  if (it == exposures.constEnd()) {
    return nullptr;
  }

  QWidget* widget = it->factory(ctx, parent);
  // The compiler checks a factory against QWidget only.
  if (nullptr != widget && widget->metaObject() != it->metaObject) {
    qWarning() << "shoot: the exposure" << id << "declares" << it->metaObject->className() << "and built a"
               << widget->metaObject()->className();
    delete widget;
    return nullptr;
  }
  return widget;
}
