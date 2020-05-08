/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CMainWindow.h"
#include "dem/CDemDraw.h"
#include "dem/CDemList.h"
#include "dem/CDemPathSetup.h"

#include <QtWidgets>

CDemPathSetup::CDemPathSetup(QStringList &paths)
    : QDialog(CMainWindow::getBestWidgetForParent())
    , paths(paths)
{
    setupUi(this);

    connect(toolAdd,    &QToolButton::clicked,              this, &CDemPathSetup::slotAddPath);
    connect(toolDelete, &QToolButton::clicked,              this, &CDemPathSetup::slotDelPath);
    connect(listWidget, &QListWidget::itemSelectionChanged, this, &CDemPathSetup::slotItemSelectionChanged);
    connect(pushDemHonk, &QPushButton::clicked,             this, &CDemPathSetup::slotDemHonk);

    for(const QString &path : paths)
    {
        QListWidgetItem * item = new QListWidgetItem(listWidget);
        item->setText(path);
    }

    labelHelp->setText(tr("Add or remove paths containing DEM data. There can be multiple files in a path but no sub-path is parsed. Supported formats are: %1").arg(CDemDraw::getSupportedFormats().join(", ")));
}

CDemPathSetup::~CDemPathSetup()
{
}

void CDemPathSetup::slotItemSelectionChanged()
{
    QList<QListWidgetItem*> items = listWidget->selectedItems();
    toolDelete->setEnabled(!items.isEmpty());
}

void CDemPathSetup::slotAddPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select DEM file path..."), QDir::homePath(), 0);
    if(!path.isEmpty())
    {
        if(!paths.contains(path))
        {
            QListWidgetItem * item = new QListWidgetItem(listWidget);
            item->setText(path);
        }
    }
}

void CDemPathSetup::slotDelPath()
{
    QList<QListWidgetItem *> items = listWidget->selectedItems();
    qDeleteAll(items);
}


void CDemPathSetup::slotDemHonk()
{
    CDemList::slotDemHonk();
    close();
}

void CDemPathSetup::accept()
{
    paths.clear();
    for(int i = 0; i < listWidget->count(); i++)
    {
        QListWidgetItem *item = listWidget->item(i);
        paths << item->text();
    }

    QDialog::accept();
}
