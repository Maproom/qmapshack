/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/

#include "helpers/CToolBarSetupDialog.h"

#include <QAction>

#include "helpers/CToolBarConfig.h"

bool CToolBarSetupDialog::CItemFilter::shouldBeMoved(QListWidgetItem* item) {
  CDialogItem* dialogItem = dynamic_cast<CDialogItem*>(item);
  if (dialogItem != nullptr) {
    return dialogItem->actionName != "separator";
  }
  return true;
}

CToolBarSetupDialog::CToolBarSetupDialog(QWidget* const& parent, CToolBarConfig* const& config)
    : QDialog(parent), config(config) {
  setupUi(this);

  selectActionsWidget->setFilter(new CItemFilter(this));

  connect(buttonBox, &QDialogButtonBox::clicked, this, &CToolBarSetupDialog::slotButtonClicked);

  configure();

  selectActionsWidget->setLabelAvailable(tr("Available Actions"));
  selectActionsWidget->setLabelSelected(tr("Selected Actions"));
}

CToolBarSetupDialog::~CToolBarSetupDialog() { selectActionsWidget->clear(); }

void CToolBarSetupDialog::accept() {
  QStringList actionNames;
  for (const QListWidgetItem* const selectedItem : selectActionsWidget->selected()) {
    const CDialogItem* const setupDialogItem = dynamic_cast<const CDialogItem* const>(selectedItem);
    if (setupDialogItem != nullptr) {
      actionNames << setupDialogItem->actionName;
    }
  }
  config->setConfiguredActionsByName(actionNames);
  config->setVisibleInFullscreen(checkFullscreen->isChecked());
  QDialog::accept();
}

void CToolBarSetupDialog::slotButtonClicked(QAbstractButton* button) const {
  if (buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
    config->setDefaultConfiguredActions();
    configure();
  }
}

void CToolBarSetupDialog::configure() const {
  QList<QListWidgetItem*> availableItems;
  QList<QListWidgetItem*> selectedItems;

  for (QAction* const& action : config->availableActions()) {
    availableItems << new CDialogItem(action->icon(), action->iconText(), action->objectName());
  }
  for (QAction* const& action : config->configuredActions()) {
    if (action->isSeparator()) {
      selectedItems << new CDialogItem(action->icon(), "---------------", action->objectName());
    } else {
      QString configuredName = action->objectName();
      for (QListWidgetItem* const& item : std::as_const(availableItems)) {
        if (configuredName == dynamic_cast<CDialogItem* const>(item)->actionName) {
          selectedItems << item;
          break;
        }
      }
    }
  }
  selectActionsWidget->setSelected(selectedItems);
  selectActionsWidget->setAvailable(availableItems);
  selectActionsWidget->sortAvailable();
  checkFullscreen->setChecked(config->visibleInFullscreen());
}
