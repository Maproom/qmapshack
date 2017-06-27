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

#include "CToolBarSetupDialog.h"
#include "CToolBarConfig.h"
#include "CMainWindow.h"
#include <QDebug>

CToolBarSetupDialog::CToolBarSetupDialog(QWidget * parent, CToolBarConfig *config) : QDialog(parent), config(config)
{
    setupUi(this);

    init();
}

CToolBarSetupDialog::~CToolBarSetupDialog()
{

}

void CToolBarSetupDialog::init()
{
    QList<CSelectDoubleListWidget::sItem> availableActions;
    QList<CSelectDoubleListWidget::sItem> selectedActions;

    for(const QAction * action : config->availableActions())
    {
        availableActions << CSelectDoubleListWidget::sItem(action->icon(),action->iconText(),action->objectName());
    }

    for (const QAction * action : config->configuredActions())
    {
        selectedActions << CSelectDoubleListWidget::sItem(action->icon(),action->iconText(),action->objectName());
    }

    selectActionsWidget->setSelected(selectedActions);
    selectActionsWidget->setAvailable(availableActions);
    selectActionsWidget->setLabelAvailable(tr("available Actions"));
    selectActionsWidget->setLabelSelected(tr("selected Actions"));
}

void CToolBarSetupDialog::accept()
{
    QStringList actionNames;
    for (const CSelectDoubleListWidget::sItem & item : selectActionsWidget->selected())
    {
        actionNames << item.data.toString();
    }
    config->setConfiguredActionsByName(actionNames);
    QDialog::accept();
}
