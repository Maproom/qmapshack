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
#ifndef CSHORTCUTSETUPDIALOG_H
#define CSHORTCUTSETUPDIALOG_H

#include <QKeySequence>

#include "ui_IShortcutSetupDialog.h"

class CShortcutConfig;
class QAction;

class CShortcutSetupDialog : public QDialog, private Ui::IShortcutSetupDialog {
  Q_OBJECT
 public:
  CShortcutSetupDialog(QWidget* const& parent, CShortcutConfig* const& config);
  virtual ~CShortcutSetupDialog();

 public slots:
  void accept() override;

 private slots:
  void slotButtonClicked(QAbstractButton* button);
  void slotFilterChanged(const QString& text);
  void slotCurrentItemChanged(QTreeWidgetItem* current);
  void slotApplyEditor();

 private:
  enum columns_e { eColumnCategory = 0, eColumnAction = 1, eColumnShortcut = 2 };

  QAction* itemAction(const QTreeWidgetItem* const item) const;
  QString itemLabel(const QTreeWidgetItem* const item) const;
  QKeySequence itemShortcut(const QTreeWidgetItem* const item) const;
  void setItemShortcut(QTreeWidgetItem* const item, const QKeySequence& shortcut);
  QTreeWidgetItem* findConflict(const QKeySequence& shortcut, const QTreeWidgetItem* const skip) const;
  void updateEditor(const QTreeWidgetItem* const item);
  QString actionCategory(const QAction* const action) const;

  CShortcutConfig* config;
  /// guards against slotApplyEditor() re-entering itself
  bool applyingShortcut = false;
};
#endif  // CSHORTCUTSETUPDIALOG_H
