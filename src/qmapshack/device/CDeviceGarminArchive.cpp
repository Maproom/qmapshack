/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "device/CDeviceGarminArchive.h"

#include <QtWidgets>

#include "device/CDeviceGarmin.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWorkspace.h"
#include "gis/gpx/CGpxProject.h"
#include "helpers/CThread.h"

CDeviceGarminArchive::CDeviceGarminArchive(const QString& path, CDeviceGarmin* parent)
    : QObject(parent), IDevice(path, parent->getKey(), parent) {
  name = tr("Archive - expand to load");
  setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
  connect(treeWidget(), &QTreeWidget::itemExpanded, this, &CDeviceGarminArchive::slotExpanded);
  connect(treeWidget(), &QTreeWidget::itemCollapsed, this, &CDeviceGarminArchive::slotCollapsed);
}

CDeviceGarminArchive::~CDeviceGarminArchive() {
  if (threadLoadData != nullptr) {
    threadLoadData->cancel();
  }
};

void CDeviceGarminArchive::slotExpanded(QTreeWidgetItem* item) {
  if ((item != this) || (childCount() != 0)) {
    return;
  }

  name = tr("Archive - loading");
  qDebug() << "reading files from device: " << dir.path();

  threadLoadData = new CThread([this]() {
    CDeviceMountLock mountLock(*this);
    const QStringList& entries = dir.entryList(QStringList("*.gpx"));

    quint32 total = entries.count();
    quint32 count = 0;

    for (const QString& entry : entries) {
      if (threadLoadData->isInterruptionRequested()) {
        break;
      }
      setProgress(++count, total);
      const QString& filename = dir.absoluteFilePath(entry);
      QMetaObject::invokeMethod(
          this,
          [this, filename]() {
            QMutexLocker lock(&IGisItem::mutexItems);
            try {
              IGisProject* project = new CGpxProject(filename, this);
              if (project) {
                if (!project->isValid()) {
                  project->destroyLater();
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

    name = tr("Archive - loaded");
    setProgress(total, total);
  });

  threadLoadData->start();
}

void CDeviceGarminArchive::slotCollapsed(QTreeWidgetItem* item) {
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
