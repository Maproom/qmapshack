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

#include "map/CMapList.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "map/CMapDraw.h"
#include "map/CMapItem.h"
#include "misc.h"

CMapTreeWidget::CMapTreeWidget(QWidget* parent) : QTreeWidget(parent) {}

void CMapTreeWidget::dragEnterEvent(QDragEnterEvent* e) {
  collapseAll();
  QTreeWidget::dragEnterEvent(e);
}

void CMapTreeWidget::dropEvent(QDropEvent* e) {
  CMapItem* item = dynamic_cast<CMapItem*>(currentItem());
  if (item) {
    item->showChildren(false);
  }

  QTreeWidget::dropEvent(e);

  if (item) {
    item->showChildren(true);
  }

  setCurrentItem(nullptr);
  emit sigChanged();
}

CMapList::CMapList(QWidget* parent) : QWidget(parent) {
  setupUi(this);
  lineFilter->addAction(actionClearFilter, QLineEdit::TrailingPosition);

  connect(treeWidget, &CMapTreeWidget::customContextMenuRequested, this, &CMapList::slotContextMenu);
  connect(treeWidget, &CMapTreeWidget::sigChanged, this, &CMapList::sigChanged);
  connect(actionMoveUp, &QAction::triggered, this, &CMapList::slotMoveUp);
  connect(actionMoveDown, &QAction::triggered, this, &CMapList::slotMoveDown);
  connect(actionRemove, &QAction::triggered, this, &CMapList::slotRemove);
  connect(actionReloadMaps, &QAction::triggered, this, &CMapList::slotReloadMaps);
  connect(labelHelpFillMapList, &QLabel::linkActivated, &CMainWindow::self(),
          static_cast<void (CMainWindow::*)(const QString&)>(&CMainWindow::slotLinkActivated));
  connect(lineFilter, &QLineEdit::textChanged, this, &CMapList::slotFilter);

  menu = new QMenu(this);
  menu->addAction(actionMoveUp);
  menu->addAction(actionMoveDown);
  menu->addSeparator();
  menu->addAction(actionRemove);
  menu->addSeparator();
  menu->addAction(actionReloadMaps);
  menu->addAction(CMainWindow::self().getMapSetupAction());
}

CMapList::~CMapList() {}

void CMapList::addMap(CMapItem* map) {
  treeWidget->addTopLevelItem(map);
  treeWidget->setItemWidget(map, 0, map->itemWidget());
  connect(map, &CMapItem::sigChanged, this, &CMapList::sigChanged);
  connect(map, &CMapItem::sigUpdateWidget, this, [this](CMapItem* map) {
    // Evil hack ahead:
    // When moving a map item in the list the item widget gets destroyed. There it has to be
    // set again. However if you do that immediately Qt crashes internally :(
    // Giving it a 100ms grace time seems to solve the problem.
    // (Yes I know, a QTreeView and delgates would be the right way to do it)
    QPointer<CMapItem> pMap(map);
    QTimer::singleShot(500, this, [this, pMap]() {
      if (!pMap.isNull()) treeWidget->setItemWidget(pMap, 0, pMap->itemWidget());
    });
  });
}

void CMapList::clear() { treeWidget->clear(); }

int CMapList::count() { return treeWidget->topLevelItemCount(); }

CMapItem* CMapList::item(int i) { return dynamic_cast<CMapItem*>(treeWidget->topLevelItem(i)); }

void CMapList::updateHelpText() {
  bool haveMaps = (treeWidget->topLevelItemCount() > 0);

  labelHelpFillMapList->setVisible(!haveMaps);

  if (!haveMaps) {
    labelIcon->show();
    labelHelpActivateMap->hide();
  } else {
    bool showHelp = true;

    const int N = treeWidget->topLevelItemCount();
    for (int n = 0; n < N; n++) {
      CMapItem* map = dynamic_cast<CMapItem*>(treeWidget->topLevelItem(n));
      if (map == nullptr) {
        continue;
      }
      if (map->isActivated()) {
        showHelp = false;
      }
    }
    labelIcon->setVisible(showHelp);
    labelHelpActivateMap->setVisible(showHelp);
  }
}

void CMapList::moveMapToTop(CMapItem* map) {
  const int index = treeWidget->indexOfTopLevelItem(map);
  if (index == NOIDX) {
    return;
  }
  map->showChildren(false);
  treeWidget->takeTopLevelItem(index);
  treeWidget->insertTopLevelItem(0, map);
  map->showChildren(true);
}

void CMapList::slotMoveUp() {
  CMapItem* item = dynamic_cast<CMapItem*>(treeWidget->currentItem());
  if (item == nullptr) {
    return;
  }

  int index = treeWidget->indexOfTopLevelItem(item);
  if (index == NOIDX) {
    return;
  }

  item->showChildren(false);
  treeWidget->takeTopLevelItem(index);
  treeWidget->insertTopLevelItem(index - 1, item);
  item->showChildren(true);
  treeWidget->setCurrentItem(0);
  emit treeWidget->sigChanged();
}

void CMapList::slotMoveDown() {
  CMapItem* item = dynamic_cast<CMapItem*>(treeWidget->currentItem());
  if (item == nullptr) {
    return;
  }

  int index = treeWidget->indexOfTopLevelItem(item);
  if (index == NOIDX) {
    return;
  }

  item->showChildren(false);
  treeWidget->takeTopLevelItem(index);
  treeWidget->insertTopLevelItem(index + 1, item);
  item->showChildren(true);
  treeWidget->setCurrentItem(0);
  emit treeWidget->sigChanged();
}

void CMapList::slotRemove() {
  CMapItem* item = dynamic_cast<CMapItem*>(treeWidget->currentItem());
  if (item == nullptr) {
    return;
  }

  int index = treeWidget->indexOfTopLevelItem(item);
  if (index == NOIDX) {
    return;
  }

  delete treeWidget->takeTopLevelItem(index);
}

void CMapList::slotContextMenu(const QPoint& point) {
  CMapItem* item = dynamic_cast<CMapItem*>(treeWidget->currentItem());

  bool itemIsSelected = nullptr != item;

  actionMoveUp->setEnabled(itemIsSelected && (treeWidget->itemAbove(item) != 0));
  actionMoveDown->setEnabled(itemIsSelected && (treeWidget->itemBelow(item) != 0));
  actionRemove->setVisible(itemIsSelected && item->getStatus() == CMapItemWidget::eStatus::Missing);

  QPoint p = treeWidget->mapToGlobal(point);
  menu->exec(p);
}

static void saveResource(const QString& name, QDir& dir) {
  QFile resource1(QString("://map/%1").arg(name));
  openFileCheckSuccess(QIODevice::ReadOnly, resource1);

  QFile file(dir.absoluteFilePath(name));
  openFileCheckSuccess(QIODevice::WriteOnly, file);
  file.write(resource1.readAll());
  file.close();
}

void CMapList::slotMapHonk() {
  QString mapPath = CMainWindow::self().getMapsPath();
  if (mapPath.isEmpty()) {
    mapPath = QDir::homePath();
  }

  mapPath = QFileDialog::getExistingDirectory(CMainWindow::getBestWidgetForParent(),
                                              tr("Where do you want to store maps?"), mapPath);
  if (mapPath.isEmpty()) {
    return;
  }

  QDir dir(mapPath);

  saveResource("WorldSat.wmts", dir);
  saveResource("WorldTopo.wmts", dir);
  saveResource("OpenStreetMap.tms", dir);
  saveResource("OSM_Topo.tms", dir);
  saveResource("OpenCycleMap.tms", dir);

  CMapDraw::setupMapPath(mapPath);

  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  if (canvas) {
    canvas->setScales(CCanvas::eScalesSquare);
  }
}

void CMapList::slotReloadMaps() { CMapDraw::setupMapPath(CMapDraw::getMapPaths()); }

void CMapList::slotFilter(const QString& str) {
  actionClearFilter->setIcon(str.isEmpty() ? QIcon("://icons/32x32/Filter.png") : QIcon("://icons/32x32/Cancel.png"));

  const int N = treeWidget->topLevelItemCount();

  if (str.isEmpty()) {
    for (int n = 0; n < N; n++) {
      CMapItem* map = dynamic_cast<CMapItem*>(treeWidget->topLevelItem(n));
      if (map == nullptr) {
        continue;
      }
      map->setHidden(false);
    }
  } else {
    const QString& tmp = str.toUpper();
    for (int n = 0; n < N; n++) {
      CMapItem* map = dynamic_cast<CMapItem*>(treeWidget->topLevelItem(n));
      if (map == nullptr) {
        continue;
      }

      map->setHidden(!map->getName().toUpper().contains(tmp));
    }
  }
}
