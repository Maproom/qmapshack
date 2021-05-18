/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "canvas/CDrawContextProj.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "items/CItemMap.h"
#include "items/CItemMapLayer.h"
#include "items/CItemTreeWidget.h"

#include <QtWidgets>

CItemTreeWidget::CItemTreeWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(toolFiles, &QToolButton::clicked, this, &CItemTreeWidget::slotFiles);
    connect(toolDelFile, &QToolButton::clicked, this, &CItemTreeWidget::slotDeleteFile);
    connect(toolDelFiles, &QToolButton::clicked, this, &CItemTreeWidget::slotDeleteFiles);
    connect(treeFiles, &QTreeWidget::itemSelectionChanged, this, &CItemTreeWidget::slotSelectionChanged);
}

void CItemTreeWidget::saveSettings(QSettings& cfg)
{
    QStringList files;
    const int N = treeFiles->topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        CItemMapLayer* layer = dynamic_cast<CItemMapLayer*>(treeFiles->topLevelItem(n));
        if(layer == nullptr)
        {
            continue;
        }

        const int M = layer->childCount();
        for(int m = 0; m < M; m++)
        {
            CItemMap* map = dynamic_cast<CItemMap*>(layer->child(m));
            if(map != nullptr)
            {
                files << map->getFilename();
                cfg.beginGroup(QString("%1_%2").arg(n).arg(m));
                map->saveSettings(cfg);
                cfg.endGroup();
            }
        }
    }

    cfg.setValue("files", files);
}

void CItemTreeWidget::loadSettings(QSettings& cfg)
{
    QStringList files = cfg.value("files", QStringList()).toStringList();
    addFiles(files);

    const int N = treeFiles->topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        CItemMapLayer* layer = dynamic_cast<CItemMapLayer*>(treeFiles->topLevelItem(n));
        if(layer == nullptr)
        {
            continue;
        }

        const int M = layer->childCount();
        for(int m = 0; m < M; m++)
        {
            CItemMap* map = dynamic_cast<CItemMap*>(layer->child(m));
            if(map != nullptr)
            {
                cfg.beginGroup(QString("%1_%2").arg(n).arg(m));
                map->loadSettings(cfg);
                cfg.endGroup();
            }
        }
    }
}

ITool* CItemTreeWidget::currentItem()
{
    CItemMapLayer* layer = dynamic_cast<CItemMapLayer*>(treeFiles->currentItem());
    if(layer != nullptr)
    {
        return layer;
    }

    CItemMap* map = dynamic_cast<CItemMap*>(treeFiles->currentItem());
    if(map != nullptr)
    {
        return dynamic_cast<CItemMapLayer*>(map->QTreeWidgetItem::parent());
    }

    return nullptr;
}

bool sortByScale(QTreeWidgetItem* item1, QTreeWidgetItem* item2)
{
    CItemMapLayer* layer1 = dynamic_cast<CItemMapLayer*>(item1);
    CItemMapLayer* layer2 = dynamic_cast<CItemMapLayer*>(item2);
    if(layer1 == nullptr || layer2 == nullptr)
    {
        return false;
    }

    return layer1->drawContext->getTrFwd().m11() < layer2->drawContext->getTrFwd().m11();
}


void CItemTreeWidget::addFiles(const QStringList& files)
{
    QString projstr;

    // preset projstr if there is already a layer with a projection.
    if(treeFiles->topLevelItemCount() != 0)
    {
        CItemMapLayer* layer = dynamic_cast<CItemMapLayer*>(treeFiles->topLevelItem(0));
        if(layer != nullptr)
        {
            projstr = layer->getProjection();
        }
    }


    for(const QString& file : files)
    {
        CItemMap* map = new CItemMap(file);

        // if something failed simply go on.
        if(!map->getIsValid())
        {
            delete map;
            continue;
        }

        // setup/test for common projection
        if(projstr.isEmpty())
        {
            projstr = map->getProjection();
        }
        else if(map->getProjection() != projstr)
        {
            const QString msg = tr("<p>The current map <i>'%1'</i> does not match the projection of previous loaded maps. "
                                   "All map files need to have the same projection.</p>"
                                   "<p><b>This file</b>: %2</p>"
                                   "<p><b>Expected</b>: %3</p>"
                                   ).arg(file).arg(map->getProjection(), projstr);
            QMessageBox::warning(this, tr("Error..."), msg, QMessageBox::Abort);
            delete map;
            continue;
        }

        // if the map is already in the list simply go on
        if(findMapByHash(map->getHash()) != nullptr)
        {
            delete map;
            continue;
        }

        CItemMapLayer* layer = nullptr;
        const int N = treeFiles->topLevelItemCount();
        for(int n = 0; n < N; n++)
        {
            layer = dynamic_cast<CItemMapLayer*>(treeFiles->topLevelItem(n));
            if(layer == nullptr)
            {
                continue;
            }

            if(layer->addMap(map))
            {
                break;
            }
            layer = nullptr;
        }

        if(layer == nullptr)
        {
            layer = new CItemMapLayer(treeFiles);
            layer->addMap(map);
        }
    } // for(const QString& file : files)

    sort(sortByScale);
    treeFiles->expandAll();
    slotSelectionChanged();
}

CItemMap* CItemTreeWidget::findMapByHash(const QString& hash)
{
    const int N = treeFiles->topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        CItemMapLayer* layer = dynamic_cast<CItemMapLayer*>(treeFiles->topLevelItem(n));
        if(layer == nullptr)
        {
            continue;
        }

        const int M = layer->childCount();
        for(int m = 0; m < M; m++)
        {
            CItemMap* map = dynamic_cast<CItemMap*>(layer->child(m));
            if((map != nullptr) && (map->getHash() == hash))
            {
                return map;
            }
        }
    }

    return nullptr;
}

void CItemTreeWidget::slotFiles()
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
}

void CItemTreeWidget::slotDeleteFiles()
{
    treeFiles->clear();
    slotSelectionChanged();
}

void CItemTreeWidget::slotDeleteFile()
{
    CItemMap* map = dynamic_cast<CItemMap*>(treeFiles->currentItem());
    if(map != nullptr)
    {
        QTreeWidgetItem* layer = map->QTreeWidgetItem::parent();
        delete map;
        if(layer->childCount() == 0)
        {
            delete layer;
        }
        slotSelectionChanged();
        return;
    }

    CItemMapLayer* layer = dynamic_cast<CItemMapLayer*>(treeFiles->currentItem());
    if(layer != nullptr)
    {
        delete layer;
        slotSelectionChanged();
        return;
    }
}

void CItemTreeWidget::slotSelectionChanged()
{
    bool isNotEmpty = treeFiles->topLevelItemCount() != 0;
    bool isSelected = !treeFiles->selectedItems().isEmpty();

    toolDelFile->setEnabled(isSelected);
    toolDelFiles->setEnabled(isNotEmpty);
    toolReloadFile->setEnabled(isSelected);

    emit sigSelectionChanged();
}

bool CItemTreeWidget::drawFx(QPainter& p, CCanvas::redraw_e needsRedraw)
{
    bool res = false;

    CItemMapLayer* layer = dynamic_cast<CItemMapLayer*>(currentItem());
    if(layer != nullptr)
    {
        res = layer->drawFx(p, needsRedraw);

        IDrawContext* dc = layer->getDrawContext();

        if(dc == nullptr)
        {
            return res;
        }

        const int N = treeFiles->topLevelItemCount();
        for(int n = 0; n < N; n++)
        {
            CItemMapLayer* layer = dynamic_cast<CItemMapLayer*>(treeFiles->topLevelItem(n));
            if(layer == nullptr)
            {
                continue;
            }

            layer->drawBoundingBoxes(p, dc);
        }
    }

    return res;
}
