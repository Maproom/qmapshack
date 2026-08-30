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

#include "poi/CPoiPathSetup.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "misc.h"
#include "poi/CPoiDraw.h"
#include "svgticon/CSvgtIcon.h"

CPoiPathSetup::CPoiPathSetup(QStringList& paths)
    : QDialog(CMainWindow::getBestWidgetForParent()),
      paths(paths)

{
  setupUi(this);
  CSvgtIcon::load(label_3, "://icons/Help.svgt");

  connect(toolAdd, &QToolButton::clicked, this, &CPoiPathSetup::slotAddPath);
  connect(toolDelete, &QToolButton::clicked, this, &CPoiPathSetup::slotDelPath);
  connect(listWidget, &QListWidget::itemSelectionChanged, this, &CPoiPathSetup::slotItemSelectionChanged);

  for (const QString& path : paths) {
    QListWidgetItem* item = new QListWidgetItem(listWidget);
    item->setText(path);
  }

  labelHelp->setText(tr("Add or remove paths containing POI data. There can be multiple files in a path but no "
                        "sub-path is parsed. Supported formats are: %1")
                         .arg(CPoiDraw::getSupportedFormats().join(", ")));
}

void CPoiPathSetup::slotItemSelectionChanged() {
  QList<QListWidgetItem*> items = listWidget->selectedItems();
  toolDelete->setEnabled(!items.isEmpty());
}

void CPoiPathSetup::accept() {
  paths = currentPaths();

  QDialog::accept();
}

QStringList CPoiPathSetup::currentPaths() const {
  QStringList paths;
  for (int i = 0; i < listWidget->count(); i++) {
    paths << listWidget->item(i)->text();
  }
  return paths;
}

void CPoiPathSetup::slotAddPath() {
  // keep the path as the user picked it, only cleaned. Comparing resolves it later on.
  const QString& path =
      cleanPath(QFileDialog::getExistingDirectory(this, tr("Select POI file path..."), QDir::homePath()));
  if (path.isEmpty() || containsPath(currentPaths(), path)) {
    return;
  }

  QListWidgetItem* item = new QListWidgetItem(listWidget);
  item->setText(path);
}

void CPoiPathSetup::slotDelPath() {
  QList<QListWidgetItem*> items = listWidget->selectedItems();
  qDeleteAll(items);
}
