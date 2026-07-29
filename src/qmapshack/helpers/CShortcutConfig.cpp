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
