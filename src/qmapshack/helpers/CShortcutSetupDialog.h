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
  enum columns_e { eColumnAction = 0, eColumnShortcut = 1 };

  QAction* itemAction(const QTreeWidgetItem* const item) const;
  QKeySequence itemShortcut(const QTreeWidgetItem* const item) const;
  void setItemShortcut(QTreeWidgetItem* const item, const QKeySequence& shortcut);
  QTreeWidgetItem* findConflict(const QKeySequence& shortcut, const QTreeWidgetItem* const skip) const;
  void updateEditor(const QTreeWidgetItem* const item);

  CShortcutConfig* config;
  /// guards against slotApplyEditor() re-entering itself
  bool applyingShortcut = false;
};
#endif  // CSHORTCUTSETUPDIALOG_H
