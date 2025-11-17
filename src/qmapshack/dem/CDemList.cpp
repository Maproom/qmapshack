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
#include "misc.h"
#include "units/IUnit.h"

CDemTreeWidget::CDemTreeWidget(QWidget* parent) : QTreeWidget(parent) {
  connect(this, &CDemTreeWidget::itemChanged, this, [this](QTreeWidgetItem* item, int) {
    CDemItem* dem = dynamic_cast<CDemItem*>(item);
    if (dem) {
      if (dem->checkState(0) == Qt::Checked && !dem->isActivated()) {
        dem->activate();
      } else if (dem->checkState(0) == Qt::Unchecked && dem->isActivated()) {
        dem->deactivate();
      }
      emit sigChanged();
    }
  });
}

void CDemTreeWidget::dragMoveEvent(QDragMoveEvent* event) {
  CDemItem* item = dynamic_cast<CDemItem*>(itemAt(event->position().toPoint()));

  if (item && item->isActivated()) {
    event->setDropAction(Qt::MoveAction);
    QTreeWidget::dragMoveEvent(event);
  } else {
    event->setDropAction(Qt::IgnoreAction);
  }
}

void CDemTreeWidget::dropEvent(QDropEvent* event) {
  CDemItem* item = dynamic_cast<CDemItem*>(currentItem());
  if (item) {
    item->showChildren(false);
  }

  QTreeWidget::dropEvent(event);

  if (item) {
    item->showChildren(true);
  }

  emit sigChanged();
}

CDemList::CDemList(QWidget* parent) : QWidget(parent) {
  setupUi(this);
  lineFilter->addAction(actionClearFilter, QLineEdit::TrailingPosition);

  connect(treeWidget, &CDemTreeWidget::customContextMenuRequested, this, &CDemList::slotContextMenu);
  connect(actionMoveUp, &QAction::triggered, this, &CDemList::slotMoveUp);
  connect(actionMoveDown, &QAction::triggered, this, &CDemList::slotMoveDown);
  connect(actionReloadDem, &QAction::triggered, this, &CDemList::slotReloadDem);
  connect(treeWidget, &CDemTreeWidget::sigChanged, this, &CDemList::sigChanged);
  connect(labelHelpFillMapList, &QLabel::linkActivated, &CMainWindow::self(),
          static_cast<void (CMainWindow::*)(const QString&)>(&CMainWindow::slotLinkActivated));
  connect(lineFilter, &QLineEdit::textChanged, this, &CDemList::slotFilter);

  menu = new QMenu(this);
  menu->addAction(actionMoveUp);
  menu->addAction(actionMoveDown);
  menu->addSeparator();
  menu->addAction(actionReloadDem);
  menu->addAction(CMainWindow::self().getDemSetupAction());
}

CDemList::~CDemList() {}

void CDemList::clear() { treeWidget->clear(); }

void CDemList::sort() {
  QList<CDemItem*> items1;
  while (treeWidget->topLevelItemCount()) {
    CDemItem* item = dynamic_cast<CDemItem*>(treeWidget->takeTopLevelItem(0));
    if (item != nullptr) {
      items1 << item;
    }
  }

  std::sort(items1.begin(), items1.end(), &sortByName<CDemItem>);

  QList<QTreeWidgetItem*> items2;
  for (CDemItem* item : std::as_const(items1)) {
    items2 << item;
  }
  treeWidget->addTopLevelItems(items2);
}

int CDemList::count() { return treeWidget->topLevelItemCount(); }

CDemItem* CDemList::item(int i) { return dynamic_cast<CDemItem*>(treeWidget->topLevelItem(i)); }

void CDemList::updateHelpText() {
  if (treeWidget->topLevelItemCount() == 0) {
    labelIcon->show();
    labelHelpFillMapList->show();
    labelHelpActivateMap->hide();
  } else {
    labelHelpFillMapList->hide();

    CDemItem* item = dynamic_cast<CDemItem*>(treeWidget->topLevelItem(0));
    bool haveActive = item && item->isActivated();
    labelIcon->setVisible(!haveActive);
    labelHelpActivateMap->setVisible(!haveActive);
  }
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
  treeWidget->takeTopLevelItem(index);
  treeWidget->insertTopLevelItem(index - 1, item);
  item->showChildren(true);
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
  treeWidget->takeTopLevelItem(index);
  treeWidget->insertTopLevelItem(index + 1, item);
  item->showChildren(true);
  treeWidget->setCurrentItem(0);
  emit treeWidget->sigChanged();
}

void CDemList::slotContextMenu(const QPoint& point) {
  CDemItem* item = dynamic_cast<CDemItem*>(treeWidget->currentItem());

  bool itemIsSelected = nullptr != item;
  bool itemIsActivated = item ? item->isActivated() : false;

  actionMoveUp->setEnabled(itemIsSelected);
  actionMoveDown->setEnabled(itemIsSelected);

  if (itemIsSelected) {
    CDemItem* item1 = dynamic_cast<CDemItem*>(treeWidget->itemBelow(item));
    actionMoveUp->setEnabled(itemIsActivated && (treeWidget->itemAbove(item) != nullptr));
    actionMoveDown->setEnabled(itemIsActivated && item1 && item1->isActivated());
  }

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
