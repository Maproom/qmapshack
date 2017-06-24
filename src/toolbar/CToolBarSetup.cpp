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

#include <QToolBar>
#include "CToolBarSetup.h"
#include "CToolBar.h"
#include "CMainWindow.h"
#include <QDebug>

CToolBarSetup::CToolBarSetup(QWidget * parent) : QDialog(parent)
{
    setupUi(this);

    init();
}

CToolBarSetup::~CToolBarSetup()
{

}

void CToolBarSetup::init()
{
    QList<CSelectDoubleListWidget::sItem> availableActions;
    QList<CSelectDoubleListWidget::sItem> selectedActions;

    for(const QString & objectName : CToolBar::actionNames)
    {
        QAction * action = (CMainWindow::self().findChild<QAction *>(objectName));
        if (action != nullptr)
        {
            availableActions << CSelectDoubleListWidget::sItem(action->icon(),action->iconText(),objectName);
        }
    }

    QToolBar * toolBar = CMainWindow::self().findChild<QToolBar *>("toolBar");
    for (const QAction * action : toolBar->actions())
    {
        selectedActions << CSelectDoubleListWidget::sItem(action->icon(),action->iconText(),action->objectName());
    }

    selectActionsWidget->setSelected(selectedActions);
    selectActionsWidget->setAvailable(availableActions);
}

void CToolBarSetup::accept()
{
   CToolBar * toolBar = CMainWindow::self().findChild<CToolBar *>("toolBar");
    toolBar->clear();
    for (const CSelectDoubleListWidget::sItem & item : selectActionsWidget->selected())
    {
        QAction * action = (CMainWindow::self().findChild<QAction *>(item.data.toString()));
        if (action != nullptr)
        {
            toolBar->addAction(action);
        }
    }
    QDialog::accept();
}
