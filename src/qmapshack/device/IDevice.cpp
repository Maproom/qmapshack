/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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
#include "device/IDevice.h"

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "device/CDeviceGarmin.h"
#include "gis/CGisListWks.h"
#include "gis/prj/IGisProject.h"
#include "helpers/CSelectCopyAction.h"

#ifdef HAVE_DBUS
#include <QtDBus>
#endif

int IDevice::cnt = 0;

IDevice::IDevice(const QString& path, type_e type, const QString& key, QTreeWidget* parent)
    : QTreeWidgetItem(parent, type), dir(path), key(key) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/Device.png"));
  cnt++;
}

IDevice::IDevice(const QString& path, type_e type, const QString& key, IDevice* parent)
    : QTreeWidgetItem(parent, type), dir(path), key(key) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/PathGreen.png"));
}

IDevice::~IDevice() { cnt--; }

void IDevice::mount(const QString& path) {
#ifdef HAVE_DBUS
  QDBusMessage message =
      QDBusMessage::createMethodCall("org.freedesktop.UDisks2", path, "org.freedesktop.UDisks2.Filesystem", "Mount");
  QVariantMap args;
  args.insert("options", "sync");
  message << args;
#if defined(Q_OS_FREEBSD)
  // XXX Hunc sint race conditions - call bsdisks (UDisks2) too fast,
  // get a malformed reply, crash.
  QThread::sleep(1);
#endif
  QDBusConnection::systemBus().call(message);
#endif
}

void IDevice::umount(const QString& path) {
#ifdef HAVE_DBUS
  QDBusMessage message =
      QDBusMessage::createMethodCall("org.freedesktop.UDisks2", path, "org.freedesktop.UDisks2.Filesystem", "Unmount");
  QVariantMap args;
  message << args;
  QDBusConnection::systemBus().call(message);
#endif
}

QString IDevice::getName() const { return text(CGisListWks::eColumnName); }

void IDevice::getItemsByPos(const QPointF& pos, QList<IGisItem*>& items) {
  const int N = childCount();
  for (int n = 0; n < N; n++) {
    IGisProject* project = dynamic_cast<IGisProject*>(child(n));
    if (project != nullptr) {
      project->getItemsByPos(pos, items);
      continue;
    }

    IDevice* device = dynamic_cast<IDevice*>(child(n));
    if (device != nullptr) {
      device->getItemsByPos(pos, items);
    }
  }
}

void IDevice::getItemsByArea(const QRectF& area, IGisItem::selflags_t flags, QList<IGisItem*>& items) {
  const int N = childCount();
  for (int n = 0; n < N; n++) {
    IGisProject* project = dynamic_cast<IGisProject*>(child(n));
    if (project != nullptr) {
      project->getItemsByArea(area, flags, items);
      continue;
    }

    IDevice* device = dynamic_cast<IDevice*>(child(n));
    if (device != nullptr) {
      device->getItemsByArea(area, flags, items);
    }
  }
}

void IDevice::getNogoAreas(QList<IGisItem*>& nogos) {
  const int N = childCount();
  for (int n = 0; n < N; n++) {
    IGisProject* project = dynamic_cast<IGisProject*>(child(n));
    if (project != nullptr) {
      project->getNogoAreas(nogos);
      continue;
    }

    IDevice* device = dynamic_cast<IDevice*>(child(n));
    if (device != nullptr) {
      device->getNogoAreas(nogos);
    }
  }
}

IGisItem* IDevice::getItemByKey(const IGisItem::key_t& key) {
  IGisItem* item = nullptr;
  const int N = childCount();
  for (int n = 0; n < N; n++) {
    IGisProject* project = dynamic_cast<IGisProject*>(child(n));
    if (project != nullptr) {
      if (project->getKey() != key.project) {
        continue;
      }

      item = project->getItemByKey(key);
      if (item != nullptr) {
        break;
      }
    }

    IDevice* device = dynamic_cast<IDevice*>(child(n));
    if (device != nullptr) {
      item = device->getItemByKey(key);
      if (item != nullptr) {
        break;
      }
    }
  }
  return item;
}

void IDevice::getItemsByKeys(const QList<IGisItem::key_t>& keys, QList<IGisItem*>& items) {
  const int N = childCount();
  for (int n = 0; n < N; n++) {
    IGisProject* project = dynamic_cast<IGisProject*>(child(n));
    if (project != nullptr) {
      project->getItemsByKeys(keys, items);
      continue;
    }

    IDevice* device = dynamic_cast<IDevice*>(child(n));
    if (device != nullptr) {
      device->getItemsByKeys(keys, items);
    }
  }
}

IGisProject* IDevice::getProjectByKey(const QString& key) {
  const int N = childCount();
  for (int n = 0; n < N; n++) {
    IGisProject* project = dynamic_cast<IGisProject*>(child(n));
    if (project != nullptr) {
      if (project->getKey() != key) {
        continue;
      }

      return project;
    }

    IDevice* device = dynamic_cast<IDevice*>(child(n));
    if (device != nullptr) {
      project = device->getProjectByKey(key);
      if (project != nullptr) {
        return project;
      }
    }
  }
  return nullptr;
}

void IDevice::editItemByKey(const IGisItem::key_t& key) {
  const int N = childCount();
  for (int n = 0; n < N; n++) {
    IGisProject* project = dynamic_cast<IGisProject*>(child(n));
    if (project != nullptr) {
      project->editItemByKey(key);
      continue;
    }

    IDevice* device = dynamic_cast<IDevice*>(child(n));
    if (device != nullptr) {
      device->editItemByKey(key);
    }
  }
}

void IDevice::insertCopyOfProject(IGisProject* project, int& lastResult) {
  IGisProject* project2 = getProjectByKey(project->getKey());
  if (project2) {
    int result = lastResult;
    if (lastResult == CSelectCopyAction::eResultNone) {
      CSelectCopyAction dlg(project, project2, CMainWindow::getBestWidgetForParent());
      dlg.exec();
      result = dlg.getResult();
      if (dlg.allOthersToo()) {
        lastResult = result;
      }
    }

    if (result == CSelectCopyAction::eResultSkip) {
      return;
    }
    if (result == CSelectCopyAction::eResultNone) {
      return;
    }

    if (project2->remove()) {
      delete project2;
    } else {
      return;
    }
  }

  insertCopyOfProject(project);
}

void IDevice::updateProject(IGisProject* project) {
  IGisProject* project2 = getProjectByKey(project->getKey());
  if (project2) {
    if (project2->remove()) {
      delete project2;
    } else {
      return;
    }
  }

  project->blockUpdateItems(true);
  insertCopyOfProject(project);
  project->blockUpdateItems(false);
}

bool IDevice::testForExternalProject(const QString& filename) {
  CDeviceMountLock mountLock(*this);

  if (QDir(filename).exists() || QFile::exists(filename)) {
    CCanvasCursorLock cursorLock(Qt::ArrowCursor, __func__);
    QString msg = tr("There is another project with the same name. If you press 'ok' it will be removed and replaced.");
    int res = QMessageBox::warning(CMainWindow::getBestWidgetForParent(), getName(), msg,
                                   QMessageBox::Ok | QMessageBox::Abort, QMessageBox::Ok);
    if (res != QMessageBox::Ok) {
      return true;
    }

    if (QDir(filename).exists()) {
      QDir(filename).removeRecursively();
    } else if (QFile::exists(filename)) {
      QFile(filename).remove();
    }

    QFileInfo fi(filename);

    const int N = childCount();
    for (int n = 0; n < N; n++) {
      QTreeWidgetItem* item = child(n);
      if (item->text(CGisListWks::eColumnName) == fi.baseName()) {
        delete item;
        break;
      }
    }
  }
  return false;
}

void IDevice::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw* gis) {
  const int N = childCount();
  for (int n = 0; n < N; n++) {
    IGisProject* project = dynamic_cast<IGisProject*>(child(n));
    if (project != nullptr) {
      project->drawItem(p, viewport, blockedAreas, gis);
      continue;
    }

    IDevice* device = dynamic_cast<IDevice*>(child(n));
    if (device != nullptr) {
      device->drawItem(p, viewport, blockedAreas, gis);
    }
  }
}

void IDevice::drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm,
                        CGisDraw* gis) {
  const int N = childCount();
  for (int n = 0; n < N; n++) {
    IGisProject* project = dynamic_cast<IGisProject*>(child(n));
    if (project != nullptr) {
      project->drawLabel(p, viewport, blockedAreas, fm, gis);
      continue;
    }

    IDevice* device = dynamic_cast<IDevice*>(child(n));
    if (device != nullptr) {
      device->drawLabel(p, viewport, blockedAreas, fm, gis);
    }
  }
}

void IDevice::drawItem(QPainter& p, const QRectF& viewport, CGisDraw* gis) {
  const int N = childCount();
  for (int n = 0; n < N; n++) {
    IGisProject* project = dynamic_cast<IGisProject*>(child(n));
    if (project != nullptr) {
      project->drawItem(p, viewport, gis);
      continue;
    }

    IDevice* device = dynamic_cast<IDevice*>(child(n));
    if (device != nullptr) {
      device->drawItem(p, viewport, gis);
    }
  }
}
