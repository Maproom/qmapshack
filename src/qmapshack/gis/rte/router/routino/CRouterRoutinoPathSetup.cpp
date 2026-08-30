/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/rte/router/routino/CRouterRoutinoPathSetup.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "misc.h"
#include "svgticon/CSvgtIcon.h"

CRouterRoutinoPathSetup::CRouterRoutinoPathSetup(QStringList& paths)
    : QDialog(CMainWindow::getBestWidgetForParent()), paths(paths) {
  setupUi(this);
  CSvgtIcon::load(label, "://icons/Help.svgt");

  connect(toolAdd, &QToolButton::clicked, this, &CRouterRoutinoPathSetup::slotAddPath);
  connect(toolDelete, &QToolButton::clicked, this, &CRouterRoutinoPathSetup::slotDelPath);
  connect(listWidget, &QListWidget::itemSelectionChanged, this, &CRouterRoutinoPathSetup::slotItemSelectionChanged);

  for (const QString& path : paths) {
    QListWidgetItem* item = new QListWidgetItem(listWidget);
    item->setText(path);
  }

  labelHelp->setText(
      tr("Add or remove paths containing Routino data. There can be multiple databases in a path but no sub-path is "
         "parsed."));
}

CRouterRoutinoPathSetup::~CRouterRoutinoPathSetup() {}

void CRouterRoutinoPathSetup::slotItemSelectionChanged() {
  QList<QListWidgetItem*> items = listWidget->selectedItems();
  toolDelete->setEnabled(!items.isEmpty());
}

QStringList CRouterRoutinoPathSetup::currentPaths() const {
  QStringList paths;
  for (int i = 0; i < listWidget->count(); i++) {
    paths << listWidget->item(i)->text();
  }
  return paths;
}

void CRouterRoutinoPathSetup::slotAddPath() {
  // keep the path as the user picked it, only cleaned. Comparing resolves it later on.
  const QString& path =
      cleanPath(QFileDialog::getExistingDirectory(this, tr("Select routing data file path..."), QDir::homePath()));
  if (path.isEmpty() || containsPath(currentPaths(), path)) {
    return;
  }

  QListWidgetItem* item = new QListWidgetItem(listWidget);
  item->setText(path);
}

void CRouterRoutinoPathSetup::slotDelPath() {
  QList<QListWidgetItem*> items = listWidget->selectedItems();
  qDeleteAll(items);
}

void CRouterRoutinoPathSetup::accept() {
  paths = currentPaths();

  QDialog::accept();
}
