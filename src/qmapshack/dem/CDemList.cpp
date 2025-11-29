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

#include "dem/CDemList.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "dem/CDemDraw.h"
#include "dem/CDemItem.h"

CDemTreeWidget::CDemTreeWidget(QWidget* parent) : QTreeWidget(parent) {}

void CDemTreeWidget::restoreItemWidgetDelayed(CDemItem* map) {
  QPointer<CDemItem> pMap(map);
  QTimer::singleShot(100, this, [this, pMap]() {
    if (!pMap.isNull()) setItemWidget(pMap, 0, pMap->itemWidget());
  });
}

void CDemTreeWidget::dragEnterEvent(QDragEnterEvent* e) {
  collapseAll();
  CDemItem* item = dynamic_cast<CDemItem*>(currentItem());
  if (item) {
    setItemWidget(item, 0, nullptr);
  }
  QTreeWidget::dragEnterEvent(e);
}
void CDemTreeWidget::dragLeaveEvent(QDragLeaveEvent* e) {
  CDemItem* item = dynamic_cast<CDemItem*>(currentItem());
  if (item) {
    item->showChildren(false);
  }

  QTreeWidget::dragLeaveEvent(e);

  if (item) {
    item->showChildren(true);
    QPointer<CDemItem> pMap(item);
    restoreItemWidgetDelayed(item);
  }
}

void CDemTreeWidget::dropEvent(QDropEvent* e) {
  CDemItem* item = dynamic_cast<CDemItem*>(currentItem());
  if (item) {
    item->showChildren(false);
  }

  QTreeWidget::dropEvent(e);

  if (item) {
    item->showChildren(true);
    restoreItemWidgetDelayed(item);
  }

  setCurrentItem(nullptr);
  emit sigChanged();
}

CDemList::CDemList(QWidget* parent) : QWidget(parent) {
  setupUi(this);
  lineFilter->addAction(actionClearFilter, QLineEdit::TrailingPosition);

  connect(treeWidget, &CDemTreeWidget::customContextMenuRequested, this, &CDemList::slotContextMenu);
  connect(treeWidget, &CDemTreeWidget::sigChanged, this, &CDemList::sigChanged);
  connect(actionMoveUp, &QAction::triggered, this, &CDemList::slotMoveUp);
  connect(actionMoveDown, &QAction::triggered, this, &CDemList::slotMoveDown);
  connect(actionRemove, &QAction::triggered, this, &CDemList::slotRemove);
  connect(actionReloadDem, &QAction::triggered, this, &CDemList::slotReloadDem);
  connect(labelHelpFillMapList, &QLabel::linkActivated, &CMainWindow::self(),
          static_cast<void (CMainWindow::*)(const QString&)>(&CMainWindow::slotLinkActivated));
  connect(lineFilter, &QLineEdit::textChanged, this, &CDemList::slotFilter);

  menu = new QMenu(this);
  menu->addAction(actionMoveUp);
  menu->addAction(actionMoveDown);
  menu->addSeparator();
  menu->addAction(actionRemove);
  menu->addSeparator();
  menu->addAction(actionReloadDem);
  menu->addAction(CMainWindow::self().getDemSetupAction());
}

CDemList::~CDemList() {}

void CDemList::addDem(CDemItem* dem) {
  treeWidget->addTopLevelItem(dem);
  treeWidget->setItemWidget(dem, 0, dem->itemWidget());
  connect(dem, &CDemItem::sigChanged, this, &CDemList::sigChanged);
}

void CDemList::clear() { treeWidget->clear(); }

int CDemList::count() { return treeWidget->topLevelItemCount(); }

CDemItem* CDemList::item(int i) { return dynamic_cast<CDemItem*>(treeWidget->topLevelItem(i)); }

void CDemList::updateHelpText() {
  bool haveMaps = (treeWidget->topLevelItemCount() > 0);

  labelHelpFillMapList->setVisible(!haveMaps);

  if (!haveMaps) {
    labelIcon->show();
    labelHelpActivateMap->hide();
  } else {
    bool showHelp = true;

    const int N = treeWidget->topLevelItemCount();
    for (int n = 0; n < N; n++) {
      CDemItem* dem = dynamic_cast<CDemItem*>(treeWidget->topLevelItem(n));
      if (dem == nullptr) {
        continue;
      }
      if (dem->isActivated()) {
        showHelp = false;
      }
    }
    labelIcon->setVisible(showHelp);
    labelHelpActivateMap->setVisible(showHelp);
  }
}

void CDemList::moveDemToTop(CDemItem* dem) {
  const int index = treeWidget->indexOfTopLevelItem(dem);
  if (index == NOIDX) {
    return;
  }
  dem->showChildren(false);
  treeWidget->setItemWidget(dem, 0, nullptr);
  treeWidget->takeTopLevelItem(index);
  treeWidget->insertTopLevelItem(0, dem);
  dem->showChildren(true);
  treeWidget->restoreItemWidgetDelayed(dem);
}

void CDemList::slotMoveUp() {
  CDemItem* item = dynamic_cast<CDemItem*>(treeWidget->currentItem());
  if (item == nullptr) {
    return;
  }

  int index = treeWidget->indexOfTopLevelItem(item);
  if (index == NOIDX) {
    return;
  }

  item->showChildren(false);
  treeWidget->setItemWidget(item, 0, nullptr);
  treeWidget->takeTopLevelItem(index);
  treeWidget->insertTopLevelItem(index - 1, item);
  item->showChildren(true);
  treeWidget->restoreItemWidgetDelayed(item);
  treeWidget->setCurrentItem(0);
  emit treeWidget->sigChanged();
}

void CDemList::slotMoveDown() {
  CDemItem* item = dynamic_cast<CDemItem*>(treeWidget->currentItem());
  if (item == nullptr) {
    return;
  }

  int index = treeWidget->indexOfTopLevelItem(item);
  if (index == NOIDX) {
    return;
  }

  item->showChildren(false);
  treeWidget->setItemWidget(item, 0, nullptr);
  treeWidget->takeTopLevelItem(index);
  treeWidget->insertTopLevelItem(index + 1, item);
  item->showChildren(true);
  treeWidget->restoreItemWidgetDelayed(item);
  treeWidget->setCurrentItem(0);
  emit treeWidget->sigChanged();
}

void CDemList::slotRemove() {
  CDemItem* item = dynamic_cast<CDemItem*>(treeWidget->currentItem());
  if (item == nullptr) {
    return;
  }

  int index = treeWidget->indexOfTopLevelItem(item);
  if (index == NOIDX) {
    return;
  }

  delete treeWidget->takeTopLevelItem(index);
}

void CDemList::slotContextMenu(const QPoint& point) {
  CDemItem* item = dynamic_cast<CDemItem*>(treeWidget->currentItem());

  bool itemIsSelected = nullptr != item;

  actionMoveUp->setEnabled(itemIsSelected && (treeWidget->itemAbove(item) != 0));
  actionMoveDown->setEnabled(itemIsSelected && (treeWidget->itemBelow(item) != 0));
  actionRemove->setVisible(itemIsSelected && item->getStatus() == CMapItemWidget::eStatus::Missing);

  QPoint p = treeWidget->mapToGlobal(point);
  menu->exec(p);
}

void CDemList::slotReloadDem() { CDemDraw::setupDemPath(CDemDraw::getDemPaths()); }

void CDemList::slotFilter(const QString& str) {
  actionClearFilter->setIcon(str.isEmpty() ? QIcon("://icons/32x32/Filter.png") : QIcon("://icons/32x32/Cancel.png"));

  const int N = treeWidget->topLevelItemCount();

  if (str.isEmpty()) {
    for (int n = 0; n < N; n++) {
      CDemItem* dem = dynamic_cast<CDemItem*>(treeWidget->topLevelItem(n));
      if (dem == nullptr) {
        continue;
      }
      dem->setHidden(false);
    }
  } else {
    const QString& tmp = str.toUpper();
    for (int n = 0; n < N; n++) {
      CDemItem* dem = dynamic_cast<CDemItem*>(treeWidget->topLevelItem(n));
      if (dem == nullptr) {
        continue;
      }

      dem->setHidden(!dem->getName().toUpper().contains(tmp));
    }
  }
}
