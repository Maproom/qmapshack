/**********************************************************************************************
    Copyright (C) 2026 Robert Siebeck

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
#include "helpers/CShortcutConfig.h"

#include <QAction>

#include "CSettings.h"

CShortcutConfig::CShortcutConfig(QObject* const& parent, const QList<QAction*>& availableActions) : QObject(parent) {
  for (QAction* const& action : availableActions) {
    if (action->isSeparator() || action->objectName().isEmpty()) {
      continue;
    }
    actions << action;
    defaults[action->objectName()] = action->shortcut();
  }
}

CShortcutConfig::~CShortcutConfig() {}

void CShortcutConfig::loadSettings() {
  SETTINGS;
  cfg.beginGroup("Shortcuts");
  for (QAction* const& action : std::as_const(actions)) {
    const QString& name = action->objectName();
    if (cfg.contains(name)) {
      action->setShortcut(QKeySequence(cfg.value(name).toString(), QKeySequence::PortableText));
    }
  }
  cfg.endGroup();
}

void CShortcutConfig::saveSettings() const {
  SETTINGS;
  cfg.beginGroup("Shortcuts");
  for (QAction* const& action : actions) {
    const QString& name = action->objectName();
    if (action->shortcut() == defaults.value(name)) {
      cfg.remove(name);
    } else {
      cfg.setValue(name, action->shortcut().toString(QKeySequence::PortableText));
    }
  }
  cfg.endGroup();
}

const QList<QAction*>& CShortcutConfig::configurableActions() const { return actions; }

QKeySequence CShortcutConfig::defaultShortcut(const QAction* const action) const {
  return defaults.value(action->objectName());
}
