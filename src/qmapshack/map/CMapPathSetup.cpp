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

#include "CMainWindow.h"
#include "map/CMapDraw.h"
#include "map/CMapList.h"
#include "map/CMapPathSetup.h"

#include <QtWidgets>

CMapPathSetup::CMapPathSetup(QStringList &paths, QString& pathCache)
    : QDialog(CMainWindow::getBestWidgetForParent())
    , paths(paths)
    , pathCache(pathCache)
{
    setupUi(this);

    connect(toolAdd,     &QToolButton::clicked,              this, &CMapPathSetup::slotAddPath);
    connect(toolDelete,  &QToolButton::clicked,              this, &CMapPathSetup::slotDelPath);
    connect(listWidget,  &QListWidget::itemSelectionChanged, this, &CMapPathSetup::slotItemSelectionChanged);
    connect(pushMapHonk, &QPushButton::clicked,              this, &CMapPathSetup::slotMapHonk);

    for(const QString &path : paths)
    {
        QListWidgetItem * item = new QListWidgetItem(listWidget);
        item->setText(path);
    }

    labelCacheRoot->setText(pathCache);
    connect(toolCacheRoot, &QToolButton::clicked, this, &CMapPathSetup::slotChangeCachePath);

    labelHelp->setText(tr("Add or remove paths containing maps. There can be multiple maps in a path but no sub-path is parsed. Supported formats are: %1").arg(CMapDraw::getSupportedFormats().join(", ")));
}

CMapPathSetup::~CMapPathSetup()
{
}

void CMapPathSetup::slotItemSelectionChanged()
{
    QList<QListWidgetItem*> items = listWidget->selectedItems();
    toolDelete->setEnabled(!items.isEmpty());
}

void CMapPathSetup::slotAddPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select map path..."), QDir::homePath(), 0);
    if(!path.isEmpty())
    {
        if(!paths.contains(path))
        {
            QListWidgetItem * item = new QListWidgetItem(listWidget);
            item->setText(path);
        }
    }
}

void CMapPathSetup::slotDelPath()
{
    QList<QListWidgetItem *> items = listWidget->selectedItems();
    qDeleteAll(items);
}

void CMapPathSetup::slotChangeCachePath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select root path..."), labelCacheRoot->text());
    if(path.isEmpty())
    {
        return;
    }

    labelCacheRoot->setText(path);
}

void CMapPathSetup::slotMapHonk()
{
    CMapList::slotMapHonk();
    close();
}

void CMapPathSetup::accept()
{
    paths.clear();
    for(int i = 0; i < listWidget->count(); i++)
    {
        QListWidgetItem *item = listWidget->item(i);
        paths << item->text();
    }

    pathCache = QDir(labelCacheRoot->text()).absolutePath();

    QDialog::accept();
}
