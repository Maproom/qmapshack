/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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
#include "helpers/CToolBarConfig.h"
#include "CMainWindow.h"

CToolBarSetupDialog::CToolBarSetupDialog(QWidget * parent, CToolBarConfig *config) : QDialog(parent), config(config)
{
    setupUi(this);

    QList<QListWidgetItem *> availableItems;
    QList<QListWidgetItem *> selectedItems;

    QList<QAction *> configuredActions = config->configuredActions();

    for(QAction * action : config->availableActions())
    {
        CToolBarSetupDialogItem * item = new CToolBarSetupDialogItem(action->icon(),action->iconText(),action->objectName());
        availableItems << item;
        if (configuredActions.contains(action))
        {
            selectedItems << item;
        }
    }

    selectActionsWidget->setSelected(selectedItems);
    selectActionsWidget->setAvailable(availableItems);
    selectActionsWidget->setLabelAvailable(tr("available Actions"));
    selectActionsWidget->setLabelSelected(tr("selected Actions"));
}

CToolBarSetupDialog::~CToolBarSetupDialog()
{

}

void CToolBarSetupDialog::accept()
{
    QStringList actionNames;
    for (const QListWidgetItem * const selectedItem : selectActionsWidget->selected())
    {
        const CToolBarSetupDialogItem * const setupDialogItem = dynamic_cast<const CToolBarSetupDialogItem * const>(selectedItem);
        if (setupDialogItem != nullptr)
        {
            actionNames << setupDialogItem->actionName;
        }
    }
    config->setConfiguredActionsByName(actionNames);
    QDialog::accept();
}

