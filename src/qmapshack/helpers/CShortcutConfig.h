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
