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
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFileInfo>
#include <QTimer>
#include <functional>

#include "gis/CGisListWks.h"
#include "gis/CGisWorkspace.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSettings.h"
#include "shoot/CShotContext.h"

namespace {
/** The project to load; `shots.py compose` writes the page fixture's. */
const QString kProjectKey = "Shoot/fixtureProject";
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

/** @return false when @p done is still false after kLoadTimeoutMs; the event loop runs meanwhile */
bool waitFor(const std::function<bool()>& done) {
  QElapsedTimer timer;
  timer.start();
  while (!done()) {
    if (timer.hasExpired(kLoadTimeoutMs)) {
      return false;
    }
    QEventLoop loop;
    QTimer::singleShot(kPollMs, &loop, &QEventLoop::quit);
    loop.exec(QEventLoop::ExcludeUserInputEvents);
  }
  return true;
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
}  // namespace

qint32 CShotFixture::load(CShotContext& ctx) {
  QString filename;
  {
    SETTINGS;
    filename = cfg.value(kProjectKey).toString();
  }
  if (filename.isEmpty() || !QFileInfo::exists(filename)) {
    qWarning() << "shoot: there is no fixture project" << filename << "- the configuration names it as" << kProjectKey;
    return 1;
  }

  // The workspace restores saved projects on its own timer.
  const CGisListWks& wks = CGisWorkspace::self().getWksList();
  if (!waitFor([&wks]() { return wks.isWorkspaceLoaded(); })) {
    qWarning() << "shoot: the workspace is still not restored after" << kLoadTimeoutMs << "ms";
    return 1;
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

  // Items arrive from a load thread through the event loop.
  if (!waitFor([project]() { return !project->isLoading(); })) {
    qWarning() << "shoot: the fixture project" << filename << "is still loading after" << kLoadTimeoutMs << "ms";
    return 1;
  }

  CGisItemTrk* trk = firstItem<CGisItemTrk>(project);
  CGisItemWpt* wpt = firstItem<CGisItemWpt>(project);
  CGisItemRte* rte = firstItem<CGisItemRte>(project);
  CGisItemOvlArea* area = firstItem<CGisItemOvlArea>(project);
  ctx.setFixture(project, trk, wpt, rte, area);

  if (nullptr == trk || nullptr == wpt || nullptr == rte || nullptr == area) {
    qWarning() << "shoot: the fixture project" << filename << "needs a track, a waypoint, a route and an area";
    return 1;
  }
  qDebug() << "shoot: fixture project" << project->getName() << "with" << project->childCount() << "items";
  return 0;
}
