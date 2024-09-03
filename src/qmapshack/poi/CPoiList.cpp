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

#include "poi/CPoiList.h"

#include "CMainWindow.h"
#include "misc.h"
#include "poi/CPoiDraw.h"
#include "poi/CPoiFileItem.h"

CPoiList::CPoiList(QWidget* parent) : QWidget(parent) {
  setupUi(this);

  connect(treeWidget, &CPoiTreeWidget::customContextMenuRequested, this, &CPoiList::slotContextMenu);
  connect(actionActivate, &QAction::triggered, this, &CPoiList::slotActivate);
  connect(actionReloadPoi, &QAction::triggered, this, &CPoiList::slotReloadPoi);
  connect(treeWidget, &CPoiTreeWidget::sigChanged, this, &CPoiList::sigChanged);
  connect(labelHelpFillPoiList, &QLabel::linkActivated, &CMainWindow::self(),
          static_cast<void (CMainWindow::*)(const QString&)>(&CMainWindow::slotLinkActivated));

  menu = new QMenu(this);
  menu->addAction(actionActivate);
  menu->addSeparator();
  menu->addAction(actionReloadPoi);
  menu->addAction(CMainWindow::self().getPoiSetupAction());
}

void CPoiList::clear() { treeWidget->clear(); }

int CPoiList::count() { return treeWidget->topLevelItemCount(); }

CPoiFileItem* CPoiList::item(int i) { return dynamic_cast<CPoiFileItem*>(treeWidget->topLevelItem(i)); }

void CPoiList::updateHelpText() {
  if (treeWidget->topLevelItemCount() == 0) {
    labelIcon->show();
    labelHelpFillPoiList->show();
    labelHelpActivatePoi->hide();
  } else {
    labelHelpFillPoiList->hide();

    CPoiFileItem* item = dynamic_cast<CPoiFileItem*>(treeWidget->topLevelItem(0));
    bool haveActive = item && item->isActivated();
    labelIcon->setVisible(!haveActive);
    labelHelpActivatePoi->setVisible(!haveActive);
  }
}

void CPoiList::sort() {
  QList<CPoiFileItem*> items1;
  while (treeWidget->topLevelItemCount()) {
    CPoiFileItem* item = dynamic_cast<CPoiFileItem*>(treeWidget->takeTopLevelItem(0));
    if (item != nullptr) {
      items1 << item;
    }
  }

  std::sort(items1.begin(), items1.end(), &sortByName<CPoiFileItem>);

  QList<QTreeWidgetItem*> items2;
  for (CPoiFileItem* item : std::as_const(items1)) {
    items2 << item;
  }
  treeWidget->addTopLevelItems(items2);
}

void CPoiList::slotActivate() {
  CPoiFileItem* item = dynamic_cast<CPoiFileItem*>(treeWidget->currentItem());
  if (nullptr == item) {
    return;
  }

  bool activated = item->toggleActivate();
  if (!activated) {
    treeWidget->setCurrentItem(0);
  }

  updateHelpText();
}

void CPoiList::slotReloadPoi() { CPoiDraw::setupPoiPath(CPoiDraw::getPoiPaths()); }

void CPoiList::slotContextMenu(const QPoint& point) {
  CPoiFileItem* item = dynamic_cast<CPoiFileItem*>(treeWidget->currentItem());

  bool itemIsSelected = nullptr != item;
  bool itemIsActivated = item ? item->isActivated() : false;

  actionActivate->setEnabled(itemIsSelected);
  actionActivate->setChecked(itemIsActivated);
  actionActivate->setText(itemIsActivated ? tr("Deactivate") : tr("Activate"));

  QPoint p = treeWidget->mapToGlobal(point);
  menu->exec(p);
}
