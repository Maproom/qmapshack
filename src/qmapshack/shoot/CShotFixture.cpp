/**********************************************************************************************
   Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "shoot/CShotFixture.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QPointer>
#include <QStringList>
#include <functional>

#include "device/CDeviceGarmin.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWorkspace.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSettings.h"
#include "shoot/CShotContext.h"
#include "shoot/CShotWait.h"

namespace {
/** The project to load; `shots.py compose` writes the page fixture's. */
const QString kProjectKey = "Shoot/fixtureProject";
/** The folder shown as a USB mass storage device; `shots.py compose` writes a scratch copy of the page fixture's. */
const QString kDeviceKey = "Shoot/fixtureDevice";
constexpr qint32 kLoadTimeoutMs = 10000;
constexpr qint32 kPollMs = 50;

/** @return the workspace project loaded from @p filename, nullptr if there is none */
IGisProject* projectOf(const QString& filename) {
  const CGisListWks& wks = CGisWorkspace::self().getWksList();
  for (int i = 0; i < wks.topLevelItemCount(); i++) {
    IGisProject* project = dynamic_cast<IGisProject*>(wks.topLevelItem(i));
    if (nullptr != project && project->getFilename() == filename) {
      return project;
    }
  }
  return nullptr;
}

/** @return the first item of this type in @p project, nullptr if there is none */
template <typename T>
T* firstItem(IGisProject* project) {
  for (int i = 0; i < project->childCount(); i++) {
    if (T* item = dynamic_cast<T*>(project->child(i)); nullptr != item) {
      return item;
    }
  }
  return nullptr;
}

/**
   @brief Add @p folder to the workspace as a Garmin device, as the device watcher does for a mounted one

   @return the failures
 */
qint32 loadDevice(const QString& folder) {
  if (folder.isEmpty()) {
    return 0;
  }
  if (!QFileInfo::exists(QDir(folder).filePath("Garmin/GarminDevice.xml"))) {
    qWarning() << "shoot: the fixture device" << folder << "has no Garmin/GarminDevice.xml";
    return 1;
  }

  CGisListWks& wks = const_cast<CGisListWks&>(CGisWorkspace::self().getWksList());
  // Key and model as IDeviceWatcher::probeForDevice() passes them: the path and the folder's name.
  QPointer<CDeviceGarmin> device =
      new CDeviceGarmin(folder, folder, QFileInfo(folder).fileName(), "Garmin/GarminDevice.xml", &wks);
  // What IDeviceWatcher::sigChanged triggers.
  if (!QMetaObject::invokeMethod(&wks, "sigChanged") || !QMetaObject::invokeMethod(&wks, "slotNewDevice")) {
    qWarning() << "shoot: the workspace was not told about the fixture device. "
                  "CGisListWks::sigChanged() or slotNewDevice() was renamed; update CShotFixture.";
    return 1;
  }

  // An archive loads only when it is expanded.
  if (!CShotWait::waitFor([&device]() { return device.isNull() || !device->isLoading(); }, kLoadTimeoutMs, kPollMs)) {
    qWarning() << "shoot: the fixture device" << folder << "is still loading after" << kLoadTimeoutMs << "ms";
    return 1;
  }
  if (device.isNull()) {
    qWarning() << "shoot: the fixture device" << folder << "was removed while it loaded";
    return 1;
  }
  qDebug() << "shoot: fixture device" << folder << "with" << device->childCount() << "items";
  return 0;
}
}  // namespace

qint32 CShotFixture::load(CShotContext& ctx) {
  QString filename;
  QString device;
  bool noProject = false;
  {
    SETTINGS;
    filename = cfg.value(kProjectKey).toString();
    // Present and empty: the page's projects part is marked EMPTY.
    noProject = cfg.contains(kProjectKey) && filename.isEmpty();
    device = cfg.value(kDeviceKey).toString();
  }
  if (!noProject && (filename.isEmpty() || !QFileInfo::exists(filename))) {
    qWarning() << "shoot: there is no fixture project" << filename << "- the configuration names it as" << kProjectKey
               << "- the fixture's projects folder needs a .qms or .gpx file";
    return 1;
  }

  // The workspace restores saved projects on its own timer.
  const CGisListWks& wks = CGisWorkspace::self().getWksList();
  if (!CShotWait::waitFor([&wks]() { return wks.isWorkspaceLoaded(); }, kLoadTimeoutMs, kPollMs)) {
    qWarning() << "shoot: the workspace is still not restored after" << kLoadTimeoutMs << "ms";
    return 1;
  }
  if (0 != loadDevice(device)) {
    return 1;
  }
  if (noProject) {
    ctx.setFixture(nullptr, nullptr, nullptr, nullptr, nullptr);
    return 0;
  }
  // loadGisProject() shows a message box for a project already loaded.
  if (nullptr != projectOf(filename)) {
    qWarning() << "shoot: the workspace already holds" << filename << "- is Database/saveOnExit false?";
    return 1;
  }

  CGisWorkspace::self().loadGisProject(filename);
  IGisProject* project = projectOf(filename);
  if (nullptr == project || !project->isValid()) {
    qWarning() << "shoot: the fixture project" << filename << "did not load";
    return 1;
  }

  // Items arrive from a load thread through the event loop, which can also close the project: looked up each time.
  if (!CShotWait::waitFor(
          [&filename]() {
            const IGisProject* loading = projectOf(filename);
            return nullptr == loading || !loading->isLoading();
          },
          kLoadTimeoutMs, kPollMs)) {
    qWarning() << "shoot: the fixture project" << filename << "is still loading after" << kLoadTimeoutMs << "ms";
    return 1;
  }
  project = projectOf(filename);
  if (nullptr == project) {
    qWarning() << "shoot: the fixture project" << filename << "was closed while it loaded";
    return 1;
  }

  CGisItemTrk* trk = firstItem<CGisItemTrk>(project);
  CGisItemWpt* wpt = firstItem<CGisItemWpt>(project);
  CGisItemRte* rte = firstItem<CGisItemRte>(project);
  CGisItemOvlArea* area = firstItem<CGisItemOvlArea>(project);
  ctx.setFixture(project, trk, wpt, rte, area);

  // An image that needs a missing item fails on its own; the others can still be taken.
  QStringList missing;
  if (nullptr == trk) {
    missing << "track";
  }
  if (nullptr == wpt) {
    missing << "waypoint";
  }
  if (nullptr == rte) {
    missing << "route";
  }
  if (nullptr == area) {
    missing << "area";
  }
  if (!missing.isEmpty()) {
    qWarning().noquote() << "shoot: the fixture project" << filename << "has no" << missing.join(", ")
                         << "- images of a dialog for one cannot be taken";
  }
  qDebug() << "shoot: fixture project" << project->getName() << "with" << project->childCount() << "items";
  return 0;
}
