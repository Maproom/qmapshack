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
#include "map/CMapItemDelegate.h"

CDemTreeWidget::CDemTreeWidget(QWidget* parent) : QTreeWidget(parent) {
  CMapItemDelegate* delegate = new CMapItemDelegate(this);
  setItemDelegate(delegate);
  QObject::connect(delegate, &CMapItemDelegate::sigUpdateItem, this, &CDemTreeWidget::slotUpdateItem);
}

void CDemTreeWidget::slotUpdateItem(const QString& key) {
  const int N = topLevelItemCount();
  for (int n = 0; n < N; n++) {
    CDemItem* dem = dynamic_cast<CDemItem*>(topLevelItem(n));
    if (dem == nullptr || dem->getKey() != key) {
      continue;
    }
    viewport()->update(visualItemRect(dem));
    break;
  }
}

void CDemTreeWidget::keyPressEvent(QKeyEvent* e) {
  const Qt::Key key = e->keyCombination().key();
  const Qt::KeyboardModifiers mod = e->keyCombination().keyboardModifiers();
  if ((mod != QFlags<Qt::KeyboardModifier>(Qt::ShiftModifier)) ||
      (key != Qt::Key_Home && key != Qt::Key_End && key != Qt::Key_Up && key != Qt::Key_Down)) {
    QTreeWidget::keyPressEvent(e);
    return;
  }

  CDemItem* dem = dynamic_cast<CDemItem*>(currentItem());
  if (dem == nullptr) {
    return;
  }

  const int from = currentIndex().row();
  const int last = topLevelItemCount() - 1;
  int to = -1;
  if (key == Qt::Key_Home && from > 0) {
    to = 0;
  } else if (key == Qt::Key_Up && from > 0) {
    to = from - 1;
  } else if (key == Qt::Key_Down && from < last) {
    to = from + 1;
  } else if (key == Qt::Key_End && from < last) {
    to = last;
  }

  if (to != -1) {
    emit sigMoveItem(dem, from, to);
  }
}

void CDemTreeWidget::dragEnterEvent(QDragEnterEvent* e) {
  collapseAll();
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
  }

  setCurrentItem(nullptr);
  emit sigChanged();
}

CDemList::CDemList(CCanvas* parent) : QWidget(parent), canvas(parent) {
  setupUi(this);
  lineFilter->addAction(actionClearFilter, QLineEdit::TrailingPosition);

  connect(treeWidget, &CDemTreeWidget::customContextMenuRequested, this, &CDemList::slotContextMenu);
  connect(treeWidget, &CDemTreeWidget::sigChanged, this, &CDemList::sigChanged);
  connect(treeWidget, &CDemTreeWidget::sigMoveItem, this, &CDemList::moveDemItem);
  connect(actionMoveHome, &QAction::triggered, this, &CDemList::slotMoveHome);
  connect(actionMoveUp, &QAction::triggered, this, &CDemList::slotMoveUp);
  connect(actionMoveDown, &QAction::triggered, this, &CDemList::slotMoveDown);
  connect(actionMoveEnd, &QAction::triggered, this, &CDemList::slotMoveEnd);
  connect(actionRemove, &QAction::triggered, this, &CDemList::slotRemove);
  connect(actionReloadDem, &QAction::triggered, this, &CDemList::slotReloadDem);
  connect(labelHelpFillMapList, &QLabel::linkActivated, &CMainWindow::self(),
          static_cast<void (CMainWindow::*)(const QString&)>(&CMainWindow::slotLinkActivated));
  connect(lineFilter, &QLineEdit::textChanged, this, &CDemList::slotFilter);

  menu = new QMenu(this);
  menu->addAction(actionMoveHome);
  menu->addAction(actionMoveUp);
  menu->addAction(actionMoveDown);
  menu->addAction(actionMoveEnd);
  menu->addSeparator();
  menu->addAction(actionRemove);
  menu->addSeparator();
  menu->addAction(actionReloadDem);
  menu->addAction(CMainWindow::self().getDemSetupAction());
  menu->setToolTipsVisible(true);
}

CDemList::~CDemList() {}

QString CDemList::getCanvasKey() const {
  if (canvas) {
    return canvas->getKey();
  }
  return "";
}

void CDemList::addToTabWidget(QTabWidget* widget) {
  if (canvas == nullptr) {
    return;
  }
  tabWidget = widget;
  tabWidget->addTab(this, canvas->getName());

  // Those two connection only live as long as tabWidget or canvas.
  // Therefore we do not need to check for valid pointers inside the lambdas
  connect(canvas, &CCanvas::sigCanvasIsCurrent, tabWidget, [this]() { tabWidget->setCurrentWidget(this); });
  connect(canvas, &CCanvas::sigNameChanged, tabWidget, [this](const CCanvas& c) {
    const int idx = tabWidget->indexOf(this);
    tabWidget->setTabText(idx, c.getName());
  });
}

void CDemList::addDem(CDemItem* dem) {
  treeWidget->addTopLevelItem(dem);
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
  treeWidget->takeTopLevelItem(index);
  treeWidget->insertTopLevelItem(0, dem);
  dem->showChildren(true);
}

void CDemList::moveDemItem(CDemItem* dem, const int from, const int to) {
  dem->showChildren(false);
  treeWidget->takeTopLevelItem(from);
  treeWidget->insertTopLevelItem(to, dem);
  dem->showChildren(true);
  treeWidget->setCurrentItem(dem);
  emit treeWidget->sigChanged();
}

void CDemList::slotMoveHome() {
  CDemItem* item = dynamic_cast<CDemItem*>(treeWidget->currentItem());
  if (item != nullptr) {
    int from = treeWidget->currentIndex().row();
    if (from > 0) {
      moveDemItem(item, from, 0);
    }
  }
}

void CDemList::slotMoveUp() {
  CDemItem* item = dynamic_cast<CDemItem*>(treeWidget->currentItem());
  if (item != nullptr) {
    int from = treeWidget->currentIndex().row();
    if (from > 0) {
      moveDemItem(item, from, from - 1);
    }
  }
}

void CDemList::slotMoveDown() {
  CDemItem* item = dynamic_cast<CDemItem*>(treeWidget->currentItem());
  if (item != nullptr) {
    int from = treeWidget->currentIndex().row();
    int last = treeWidget->topLevelItemCount() - 1;
    if (from < last) {
      moveDemItem(item, from, from + 1);
    }
  }
}

void CDemList::slotMoveEnd() {
  CDemItem* item = dynamic_cast<CDemItem*>(treeWidget->currentItem());
  if (item != nullptr) {
    int from = treeWidget->currentIndex().row();
    int last = treeWidget->topLevelItemCount() - 1;
    if (from < last) {
      moveDemItem(item, from, last);
    }
  }
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

  actionMoveHome->setEnabled(itemIsSelected && (treeWidget->itemAbove(item) != 0));
  actionMoveUp->setEnabled(itemIsSelected && (treeWidget->itemAbove(item) != 0));
  actionMoveDown->setEnabled(itemIsSelected && (treeWidget->itemBelow(item) != 0));
  actionMoveEnd->setEnabled(itemIsSelected && (treeWidget->itemBelow(item) != 0));
  actionRemove->setVisible(itemIsSelected && item->getStatus() == IMapItem::eStatus::Missing);

  QPoint p = treeWidget->mapToGlobal(point);
  menu->exec(p);
}

void CDemList::slotReloadDem() {
  CMapItemDelegate* delegate = dynamic_cast<CMapItemDelegate*>(treeWidget->itemDelegate());
  if (delegate != nullptr) {
    delegate->reset();
  }
  CDemDraw::setupDemPath(CDemDraw::getDemPaths());
}

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
