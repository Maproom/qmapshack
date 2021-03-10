/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "helpers/CSettings.h"
#include "items/CItemListWidget.h"
#include "items/IItem.h"


#include <QtWidgets>

CItemListWidget::CItemListWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(toolFiles, &QToolButton::clicked, this, &CItemListWidget::slotFiles);
    connect(toolDelFile, &QToolButton::clicked, this, &CItemListWidget::slotDeleteFile);
    connect(toolDelFiles, &QToolButton::clicked, this, &CItemListWidget::slotDeleteFiles);
    connect(toolReloadFile, &QToolButton::clicked, this, &CItemListWidget::slotLoadCurrentMap);
    connect(listFiles, &QListWidget::itemSelectionChanged, this, &CItemListWidget::slotSelectionChanged);
}

void CItemListWidget::saveSettings(QSettings& cfg)
{
    QStringList files;
    const int N = listFiles->count();
    for(int n = 0; n < N; n++)
    {
        IItem * item = dynamic_cast<IItem*>(listFiles->item(n));
        if(item != nullptr)
        {
            files << item->getFilename();
            cfg.beginGroup(QString("%1").arg(n));
            item->saveSettings(cfg);
            cfg.endGroup();
        }
    }
    cfg.setValue("files", files);
    cfg.setValue("lastFile", listFiles->currentRow());
}

void CItemListWidget::loadSettings(QSettings& cfg)
{
    QStringList files = cfg.value("files", QStringList()).toStringList();
    addFiles(files);

    const int N = listFiles->count();
    for(int n = 0; n < N; n++)
    {
        IItem * item = dynamic_cast<IItem*>(listFiles->item(n));
        if(item != nullptr)
        {
            cfg.beginGroup(QString("%1").arg(n));
            item->loadSettings(cfg);
            cfg.endGroup();
        }
    }

    listFiles->setCurrentRow(cfg.value("lastFile", 0).toInt());
}

IItem * CItemListWidget::currentItem()
{
    return dynamic_cast<IItem*>(listFiles->currentItem());
}

IItem * CItemListWidget::item(int n)
{
    return dynamic_cast<IItem*>(listFiles->item(n));
}

void CItemListWidget::addFiles(const QStringList& files)
{
    for(const QString& file : files)
    {
        emit sigAddItem(file, listFiles);
    }

    listFiles->setCurrentRow(listFiles->count() - 1);
    slotSelectionChanged();
}

void CItemListWidget::slotFiles()
{
    SETTINGS;
    QString path = cfg.value("Path/mapInput", QDir::homePath()).toString();

    const QStringList& files = QFileDialog::getOpenFileNames(this, tr("Select files..."), path);
    if(files.isEmpty())
    {
        return;
    }

    cfg.setValue("Path/mapInput", QFileInfo(files.first()).absolutePath());
    addFiles(files);

    if(listFiles->count() && (listFiles->currentItem() == nullptr))
    {
        listFiles->setCurrentRow(0);
    }
}

void CItemListWidget::slotLoadCurrentMap()
{
    IItem * item = dynamic_cast<IItem*>(listFiles->currentItem());
    if(item != nullptr)
    {
        item->reload();
    }
}

void CItemListWidget::slotDeleteFiles()
{
    listFiles->clear();
    slotSelectionChanged();
}

void CItemListWidget::slotDeleteFile()
{
    QListWidgetItem * item = listFiles->takeItem(listFiles->currentRow());
    delete item;
    slotSelectionChanged();
}

void CItemListWidget::slotSelectionChanged()
{
    IItem * item = dynamic_cast<IItem*>(listFiles->currentItem());
    if(item != nullptr)
    {
        item->toFront();
    }

    bool isNotEmpty = listFiles->count() != 0;
    int row        = listFiles->currentRow();

    toolDelFile->setEnabled(row != -1);
    toolDelFiles->setEnabled(isNotEmpty);
    toolReloadFile->setEnabled(row != -1);

    emit sigSelectionChanged();
}

