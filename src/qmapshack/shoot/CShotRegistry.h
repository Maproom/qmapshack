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

#ifndef CSHOTREGISTRY_H
#define CSHOTREGISTRY_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <functional>

class CShotContext;
class QWidget;

/** @brief The widget classes a shot builds from nothing, by id; filled by SHOT_EXPOSE statics. */
class CShotRegistry {
 public:
  /** Builds the widget below the parent; nullptr when it cannot be built. */
  using exposure_factory_t = std::function<QWidget*(CShotContext&, QWidget*)>;

  static CShotRegistry& self();

  /** @return false for an id already taken */
  bool addExposure(const QString& id, const QMetaObject* metaObject, const exposure_factory_t& factory,
                   const QString& description);

  QStringList exposureNames() const { return exposures.keys(); }

  /** @return the id of the exposure building exactly @p widget's class, empty when there is none */
  QString exposureOf(const QWidget* widget) const;

  /** @return what the exposure @p id shows, empty for an unknown id */
  QString exposureDescription(const QString& id) const { return exposures.value(id).description; }

  /** @return ids registered more than once; the first registration wins */
  const QStringList& duplicates() const { return duplicateIds; }

  /** @return nullptr for an unknown id, a failed factory, or a widget of another class than declared */
  QWidget* buildExposure(const QString& id, CShotContext& ctx, QWidget* parent) const;

 private:
  CShotRegistry() = default;

  struct exposure_t {
    exposure_factory_t factory;
    const QMetaObject* metaObject = nullptr;
    QString description;
  };

  QMap<QString, exposure_t> exposures;
  QStringList duplicateIds;
};

// Two levels so __COUNTER__ expands before joining.
#define SHOT_JOIN_(a, b) a##b
#define SHOT_JOIN(a, b) SHOT_JOIN_(a, b)

/**
   @brief Register how to build one widget class.

   @param ID       what a shot's `exposure` names
   @param TYPE     the class FACTORY builds; must declare Q_OBJECT
   @param FACTORY  (CShotContext&, QWidget* parent) -> QWidget*; top-level commas must be parenthesised
 */
#define SHOT_EXPOSE(ID, DESC, TYPE, FACTORY)                                               \
  static_assert(QtPrivate::HasQ_OBJECT_Macro<TYPE>::Value, #TYPE " declares no Q_OBJECT"); \
  static const bool SHOT_JOIN(shotExpose_, __COUNTER__) =                                  \
      CShotRegistry::self().addExposure(ID, &TYPE::staticMetaObject, FACTORY, DESC)

#endif  // CSHOTREGISTRY_H
