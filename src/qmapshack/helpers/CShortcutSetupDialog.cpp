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
#include "helpers/CShortcutSetupDialog.h"

#include <QAction>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QSignalBlocker>

#include "helpers/CShortcutConfig.h"

CShortcutSetupDialog::CShortcutSetupDialog(QWidget* const& parent, CShortcutConfig* const& config)
    : QDialog(parent), config(config) {
  setupUi(this);

  keySequenceEdit->setMaximumSequenceLength(1);
  keySequenceEdit->setClearButtonEnabled(true);
  keySequenceEdit->setEnabled(false);
  pushDefault->setEnabled(false);

  for (QAction* const& action : config->configurableActions()) {
    QTreeWidgetItem* item = new QTreeWidgetItem(treeActions);
    item->setIcon(eColumnAction, action->icon());
    item->setText(eColumnAction, action->iconText());
    item->setText(eColumnCategory, actionCategory(action));
    item->setToolTip(eColumnAction, action->toolTip());
    item->setData(eColumnAction, Qt::UserRole, QVariant::fromValue(action));
    setItemShortcut(item, action->shortcut());
  }
  treeActions->sortItems(eColumnAction, Qt::AscendingOrder);
  treeActions->sortItems(eColumnCategory, Qt::AscendingOrder);
  treeActions->setSortingEnabled(true);
  treeActions->header()->setSectionResizeMode(eColumnCategory, QHeaderView::ResizeToContents);
  treeActions->header()->setSectionResizeMode(eColumnAction, QHeaderView::Stretch);
  treeActions->header()->setSectionResizeMode(eColumnShortcut, QHeaderView::ResizeToContents);
  treeActions->header()->setStretchLastSection(false);

  connect(buttonBox, &QDialogButtonBox::clicked, this, &CShortcutSetupDialog::slotButtonClicked);
  connect(lineFilter, &QLineEdit::textChanged, this, &CShortcutSetupDialog::slotFilterChanged);
  connect(treeActions, &QTreeWidget::currentItemChanged, this, &CShortcutSetupDialog::slotCurrentItemChanged);
  connect(keySequenceEdit, &QKeySequenceEdit::editingFinished, this, &CShortcutSetupDialog::slotApplyEditor);
  // the clear button emits keySequenceChanged only, no editingFinished
  connect(keySequenceEdit, &QKeySequenceEdit::keySequenceChanged, this, [this](const QKeySequence& shortcut) {
    if (shortcut.isEmpty()) {
      slotApplyEditor();
    }
  });
  connect(pushDefault, &QPushButton::clicked, this, [this]() {
    QTreeWidgetItem* item = treeActions->currentItem();
    if (item != nullptr) {
      keySequenceEdit->setKeySequence(this->config->defaultShortcut(itemAction(item)));
      slotApplyEditor();
    }
  });
}

CShortcutSetupDialog::~CShortcutSetupDialog() {}

void CShortcutSetupDialog::accept() {
  for (qint32 i = 0; i < treeActions->topLevelItemCount(); i++) {
    QTreeWidgetItem* item = treeActions->topLevelItem(i);
    itemAction(item)->setShortcut(itemShortcut(item));
  }
  config->saveSettings();
  QDialog::accept();
}

void CShortcutSetupDialog::slotButtonClicked(QAbstractButton* button) {
  if (buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
    for (qint32 i = 0; i < treeActions->topLevelItemCount(); i++) {
      QTreeWidgetItem* item = treeActions->topLevelItem(i);
      setItemShortcut(item, config->defaultShortcut(itemAction(item)));
    }
    updateEditor(treeActions->currentItem());
  }
}

void CShortcutSetupDialog::slotFilterChanged(const QString& text) {
  for (qint32 i = 0; i < treeActions->topLevelItemCount(); i++) {
    QTreeWidgetItem* item = treeActions->topLevelItem(i);
    const bool match = item->text(eColumnAction).contains(text, Qt::CaseInsensitive) ||
                       item->text(eColumnCategory).contains(text, Qt::CaseInsensitive) ||
                       item->text(eColumnShortcut).contains(text, Qt::CaseInsensitive);
    item->setHidden(!match);
  }

  QTreeWidgetItem* current = treeActions->currentItem();
  if (current != nullptr && current->isHidden()) {
    treeActions->setCurrentItem(nullptr);
  }
}

void CShortcutSetupDialog::slotCurrentItemChanged(QTreeWidgetItem* current) { updateEditor(current); }

void CShortcutSetupDialog::slotApplyEditor() {
  if (applyingShortcut) {
    return;
  }
  applyingShortcut = true;

  QTreeWidgetItem* item = treeActions->currentItem();
  if (item == nullptr) {
    applyingShortcut = false;
    return;
  }

  const QKeySequence shortcut = keySequenceEdit->keySequence();
  if (shortcut == itemShortcut(item)) {
    applyingShortcut = false;
    return;
  }

  QTreeWidgetItem* conflict = findConflict(shortcut, item);
  if (conflict != nullptr) {
    const QString conflictLabel =
        conflict->text(eColumnCategory).isEmpty()
            ? conflict->text(eColumnAction)
            : tr("%1 (%2)").arg(conflict->text(eColumnAction), conflict->text(eColumnCategory));
    const QString itemLabel = item->text(eColumnCategory).isEmpty()
                                  ? item->text(eColumnAction)
                                  : tr("%1 (%2)").arg(item->text(eColumnAction), item->text(eColumnCategory));
    const QMessageBox::StandardButton answer = QMessageBox::question(
        this, tr("Shortcut already in use"),
        tr("The shortcut '%1' is already assigned to '%2'. Do you want to reassign it to '%3'?")
            .arg(shortcut.toString(QKeySequence::NativeText), conflictLabel, itemLabel),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::Yes) {
      setItemShortcut(conflict, QKeySequence());
    } else {
      updateEditor(item);
      applyingShortcut = false;
      return;
    }
  }

  setItemShortcut(item, shortcut);
  applyingShortcut = false;
}

QAction* CShortcutSetupDialog::itemAction(const QTreeWidgetItem* const item) const {
  return item->data(eColumnAction, Qt::UserRole).value<QAction*>();
}

QKeySequence CShortcutSetupDialog::itemShortcut(const QTreeWidgetItem* const item) const {
  return item->data(eColumnShortcut, Qt::UserRole).value<QKeySequence>();
}

void CShortcutSetupDialog::setItemShortcut(QTreeWidgetItem* const item, const QKeySequence& shortcut) {
  item->setData(eColumnShortcut, Qt::UserRole, QVariant::fromValue(shortcut));
  item->setText(eColumnShortcut, shortcut.toString(QKeySequence::NativeText));

  QFont font = item->font(eColumnShortcut);
  font.setBold(shortcut != config->defaultShortcut(itemAction(item)));
  item->setFont(eColumnShortcut, font);
}

QTreeWidgetItem* CShortcutSetupDialog::findConflict(const QKeySequence& shortcut,
                                                    const QTreeWidgetItem* const skip) const {
  if (shortcut.isEmpty()) {
    return nullptr;
  }
  for (qint32 i = 0; i < treeActions->topLevelItemCount(); i++) {
    QTreeWidgetItem* item = treeActions->topLevelItem(i);
    if (item != skip && itemShortcut(item) == shortcut) {
      return item;
    }
  }
  return nullptr;
}

void CShortcutSetupDialog::updateEditor(const QTreeWidgetItem* const item) {
  const QSignalBlocker blocker(keySequenceEdit);
  keySequenceEdit->setEnabled(item != nullptr);
  pushDefault->setEnabled(item != nullptr);
  keySequenceEdit->setKeySequence(item != nullptr ? itemShortcut(item) : QKeySequence());
}

QString CShortcutSetupDialog::actionCategory(const QAction* const action) const {
  const QVariant tagged = action->property("shortcutCategory");
  if (tagged.isValid()) {
    return tagged.toString();
  }
  // fall back to the title of the QMenu the action was added to via the .ui file
  for (QObject* const object : action->associatedObjects()) {
    const QMenu* const menu = qobject_cast<const QMenu*>(object);
    if (menu != nullptr) {
      QString title = menu->title();
      title.remove('&');
      return title;
    }
  }
  return QString();
}
