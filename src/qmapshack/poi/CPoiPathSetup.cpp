/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "poi/CPoiDraw.h"
#include "poi/CPoiPathSetup.h"

#include <QtWidgets>

CPoiPathSetup::CPoiPathSetup(QStringList& paths)
    : QDialog(CMainWindow::getBestWidgetForParent())
    , paths(paths)

{
    setupUi(this);

    connect(toolAdd, &QToolButton::clicked, this, &CPoiPathSetup::slotAddPath);
    connect(toolDelete, &QToolButton::clicked, this, &CPoiPathSetup::slotDelPath);
    connect(listWidget, &QListWidget::itemSelectionChanged, this, &CPoiPathSetup::slotItemSelectionChanged);

    for(const QString& path : paths)
    {
        QListWidgetItem* item = new QListWidgetItem(listWidget);
        item->setText(path);
    }

    labelHelp->setText(tr("Add or remove paths containing POI data. There can be multiple files in a path but no sub-path is parsed. Supported formats are: %1").arg(CPoiDraw::getSupportedFormats().join(", ")));
}

void CPoiPathSetup::slotItemSelectionChanged()
{
    QList<QListWidgetItem*> items = listWidget->selectedItems();
    toolDelete->setEnabled(!items.isEmpty());
}

void CPoiPathSetup::accept()
{
    paths.clear();
    for(int i = 0; i < listWidget->count(); i++)
    {
        QListWidgetItem* item = listWidget->item(i);
        paths << item->text();
    }

    QDialog::accept();
}

void CPoiPathSetup::slotAddPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select POI file path..."), QDir::homePath(), 0);
    if(!path.isEmpty())
    {
        if(!paths.contains(path))
        {
            QListWidgetItem* item = new QListWidgetItem(listWidget);
            item->setText(path);
        }
    }
}

void CPoiPathSetup::slotDelPath()
{
    QList<QListWidgetItem*> items = listWidget->selectedItems();
    qDeleteAll(items);
}

