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

#include "device/CDeviceGarminMtp.h"
#include "device/IDeviceAccess.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWorkspace.h"
#include "gis/gpx/CGpxProject.h"
#include "helpers/CThread.h"
#include "misc.h"

CDeviceGarminArchiveMtp::CDeviceGarminArchiveMtp(const QString& path, IDeviceAccess* device, CDeviceGarminMtp* parent)
    : QObject(parent), IDevice(path, parent->getKey(), parent), device(device) {
  name = tr("Archive - expand to load");
  setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
  connect(treeWidget(), &QTreeWidget::itemExpanded, this, &CDeviceGarminArchiveMtp::slotExpanded);
  connect(treeWidget(), &QTreeWidget::itemCollapsed, this, &CDeviceGarminArchiveMtp::slotCollapsed);
}

CDeviceGarminArchiveMtp::~CDeviceGarminArchiveMtp() {
  if (threadLoadData != nullptr) {
    threadLoadData->cancel();
  }
};

void CDeviceGarminArchiveMtp::slotExpanded(QTreeWidgetItem* item) {
  if ((item != this) || (childCount() != 0)) {
    return;
  }

  name = tr("Archive - loading");

  threadLoadData = new CThread([this]() {
    qDebug() << "reading files from device: " << dir.path();
    const QStringList& entries = device->listFilesOnStorage(dir.path());
    const quint32 total = entries.count();
    quint32 count = 0;
    for (const QString& entry : entries) {
      if (threadLoadData->isInterruptionRequested()) {
        break;
      }

      setProgress(++count, total);
      if (!entry.endsWith(".gpx")) {
        continue;
      }

      QMetaObject::invokeMethod(
          this,
          [this, entry]() {
            const QString& filename = dir.filePath(entry);
            QTemporaryFile tempFile;
            if (!device->readFileFromStorage(filename, tempFile)) {
              return;
            }
            openFileCheckSuccess(QIODevice::ReadWrite, tempFile);

            QMutexLocker lock(&IGisItem::mutexItems);
            try {
              IGisProject* project = new CGpxProject(tempFile, filename, this);
              if (project) {
                if (!project->isValid()) {
                  delete project;
                } else {
                  project->setVisibility(isVisible());
                }
              }
            } catch (const QString& msg) {
              qWarning() << msg;
            }
          },
          Qt::BlockingQueuedConnection);
    }

    QMutexLocker lock(&IGisItem::mutexItems);
    name = tr("Archive - loaded");
    setProgress(total, total);
  });

  threadLoadData->start();
}

void CDeviceGarminArchiveMtp::slotCollapsed(QTreeWidgetItem* item) {
  if ((item != this) || (childCount() == 0)) {
    return;
  }

  if (threadLoadData != nullptr) {
    threadLoadData->cancel();
  }

  QMutexLocker lock(&IGisItem::mutexItems);
  qDeleteAll(takeChildren());
  name = tr("Archive - expand to load");
  emit CGisWorkspace::self().sigChanged();
}
