#ifndef CSHORTCUTCONFIG_H
#define CSHORTCUTCONFIG_H

#include <QKeySequence>
#include <QMap>
#include <QObject>

class QAction;

class CShortcutConfig : public QObject {
  Q_OBJECT
 public:
  CShortcutConfig(QObject* const& parent, const QList<QAction*>& availableActions);
  virtual ~CShortcutConfig();

  void loadSettings();
  void saveSettings() const;
  const QList<QAction*>& configurableActions() const;
  QKeySequence defaultShortcut(const QAction* const action) const;

 private:
  QList<QAction*> actions;
  QMap<QString, QKeySequence> defaults;
};
#endif  // CSHORTCUTCONFIG_H
