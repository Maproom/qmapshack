/**********************************************************************************************
    Copyright (C) 2025 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "device/CDeviceGarminArchiveMtp.h"

#include "canvas/CCanvas.h"
#include "device/CDeviceGarminMtp.h"
#include "device/IDeviceAccess.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWorkspace.h"
#include "gis/gpx/CGpxProject.h"

CDeviceGarminArchiveMtp::CDeviceGarminArchiveMtp(const QString& path, IDeviceAccess* device, CDeviceGarminMtp* parent)
    : IDevice(path, eTypeGarminMtp, parent->getKey(), parent), device(device) {
  setText(CGisListWks::eColumnName, tr("Archive - expand to load"));
  setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
  connect(treeWidget(), &QTreeWidget::itemExpanded, this, &CDeviceGarminArchiveMtp::slotExpanded);
  connect(treeWidget(), &QTreeWidget::itemCollapsed, this, &CDeviceGarminArchiveMtp::slotCollapsed);
}

void CDeviceGarminArchiveMtp::slotExpanded(QTreeWidgetItem* item) {
  if ((item != this) || (childCount() != 0)) {
    return;
  }

  setText(CGisListWks::eColumnName, tr("Archive - loaded"));
  QMutexLocker lock(&IGisItem::mutexItems);
  CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
  qDebug() << "reading files from device: " << dir.path();
  const QStringList& entries = device->listFilesOnStorage(dir.path());
  for (const QString& entry : entries) {
    if (!entry.endsWith(".gpx")) {
      continue;
    }

    const QString& filename = dir.filePath(entry);
    QTemporaryFile tempFile;
    if (!device->readFileFromStorage(filename, tempFile)) {
      continue;
    }
    tempFile.open();
    IGisProject* project = new CGpxProject(tempFile, filename, this);
    if (!project->isValid()) {
      delete project;
    }
  }
}

void CDeviceGarminArchiveMtp::slotCollapsed(QTreeWidgetItem* item) {
  if ((item != this) || (childCount() == 0)) {
    return;
  }

  QMutexLocker lock(&IGisItem::mutexItems);
  CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);

  qDeleteAll(takeChildren());

  setText(CGisListWks::eColumnName, tr("Archive - expand to load"));
  emit CGisWorkspace::self().sigChanged();
}
