/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "CMapPathSetup.h"

#include <QtWidgets>

CMapPathSetup::CMapPathSetup(QStringList &paths)
    : paths(paths)
{
    setupUi(this);

    connect(toolAdd, SIGNAL(clicked()), this, SLOT(slotAddPath()));
    connect(toolDelete, SIGNAL(clicked()), this, SLOT(slotDelPath()));

    foreach(const QString& path, paths)
    {
        QListWidgetItem * item = new QListWidgetItem(listWidget);
        item->setText(path);
    }
}

CMapPathSetup::~CMapPathSetup()
{

}

void CMapPathSetup::slotAddPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select map path..."), QDir::homePath());
    if(!path.isEmpty())
    {
        QListWidgetItem * item = new QListWidgetItem(listWidget);
        item->setText(path);
    }
}

void CMapPathSetup::slotDelPath()
{
    QList<QListWidgetItem *> items = listWidget->selectedItems();
    qDeleteAll(items);
}

void CMapPathSetup::accept()
{
    paths.clear();
    for(int i = 0; i < listWidget->count(); i++)
    {
        QListWidgetItem *item = listWidget->item(i);
        paths << item->text();
    }

    QDialog::accept();
}
