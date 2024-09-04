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

#include "gis/tnv/CTwoNavProject.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/qms/CQmsProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSelectCopyAction.h"
#include "helpers/CSettings.h"

CTwoNavProject::CTwoNavProject(const QString& filename, IDevice* parent) : IGisProject(eTypeTwoNav, filename, parent) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/2NavProject.png"));

  load(filename);

  sortItems();
  setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
  setToolTip(CGisListWks::eColumnName, getInfo());
  updateItems();
  valid = true;
}

CTwoNavProject::CTwoNavProject(const QString& filename, const IGisProject* project, IDevice* parent)
    : IGisProject(eTypeTwoNav, filename, parent) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/2NavProject.png"));
  *(IGisProject*)this = *project;

  CSelectCopyAction::result_e res = CSelectCopyAction::eResultNone;
  const int N = project->childCount();
  for (int n = 0; n < N; n++) {
    IGisItem* item = dynamic_cast<IGisItem*>(project->child(n));
    if (item) {
      insertCopyOfItem(item, NOIDX, res);
    }
  }

  sortItems();
  setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
  setToolTip(CGisListWks::eColumnName, getInfo());
  updateItems();
  valid = true;
}

CTwoNavProject::~CTwoNavProject() {}

bool CTwoNavProject::save() {
  bool res = true;
  CProjectMountLock mountLock(*this);
  QDir().mkpath(filename);
  QDir dir(filename);

  try {
    QFile fileKey(dir.absoluteFilePath(QString("%1.key").arg(getKey())));
    if (!fileKey.open(QIODevice::WriteOnly)) {
      QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                            tr("Failed to open %1.").arg(fileKey.fileName()), QMessageBox::Abort);
      throw -1;
    }
    fileKey.close();

    QList<CGisItemWpt*> wpts;
    QList<CGisItemWpt*> geocaches;
    const int N = childCount();
    for (int n = 0; n < N; n++) {
      QTreeWidgetItem* item = child(n);
      CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(item);
      if (trk) {
        QString fn = trk->getName();
        static const QRegularExpression re("[^A-Za-z0-9_]");
        fn = fn.remove(re);
        fn = dir.absoluteFilePath(fn + ".trk");

        if (!trk->saveTwoNav(fn)) {
          throw -1;
        }
      }
      CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(item);
      if (wpt) {
        if (wpt->isGeocache()) {
          geocaches << wpt;
        } else {
          wpts << wpt;
        }
      }
    }

    QString fn = getName();
    static const QRegularExpression re("[^A-Za-z0-9_]");
    fn = fn.remove(re);

    if (!wpts.isEmpty()) {
      if (!saveWpts(wpts, dir.absoluteFilePath(fn + ".wpt"), dir)) {
        throw -1;
      }
    }

    if (!geocaches.isEmpty()) {
      if (!saveWpts(geocaches, dir.absoluteFilePath(fn + "_gc.wpt"), dir)) {
        throw -1;
      }
    }
  } catch (int) {
    res = false;
  }

  if (res) {
    markAsSaved();
  }
  return res;
}

bool CTwoNavProject::saveAs() {
  SETTINGS;
  QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();

  QString filter = filedialogFilterGPX;
  QString fn = QFileDialog::getSaveFileName(CMainWindow::getBestWidgetForParent(), tr("Save GIS data to..."), path,
                                            filedialogSaveFilters, &filter);

  if (fn.isEmpty()) {
    return false;
  }

  bool res = false;
  if (filter == filedialogFilterGPX) {
    res = CGpxProject::saveAs(fn, *this, false);
  } else if (filter == filedialogFilterQMS) {
    res = CQmsProject::saveAs(fn, *this);
  } else {
    return false;
  }

  path = QFileInfo(fn).absolutePath();
  cfg.setValue("Paths/lastGisPath", path);
  return res;
}

bool CTwoNavProject::saveWpts(QList<CGisItemWpt*>& wpts, const QString& filename, const QDir& dir) {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to open %1.").arg(filename),
                          QMessageBox::Abort);
    return false;
  }
  QTextStream out(&file);
  out.setEncoding(QStringConverter::Utf8);

  qreal north = -90.0;
  qreal south = 90.0;
  qreal west = 180.0;
  qreal east = -180.0;

  for (CGisItemWpt* wpt : wpts) {
    QPointF pt = wpt->getPosition();

    if (north < pt.y()) {
      north = pt.y();
    }
    if (south > pt.y()) {
      south = pt.y();
    }
    if (west > pt.x()) {
      west = pt.x();
    }
    if (east < pt.x()) {
      east = pt.x();
    }
  }

  out << Qt::bom;
  out << "B  UTF-8" << Qt::endl;
  out << "G  WGS 84" << Qt::endl;
  out << "U  1" << Qt::endl;
  out << "z " << west << ", " << south << ", " << east << ", " << north << Qt::endl;

  for (CGisItemWpt* wpt : wpts) {
    wpt->saveTwoNav(out, dir);
  }

  file.close();

  return true;
}

bool CTwoNavProject::load(const QString& filename) {
  QDir dir(filename);

  const QStringList& entries = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
  for (const QString& entry : entries) {
    QFileInfo fi(entry);

    if (fi.suffix().toLower() == "key") {
      key = fi.completeBaseName();
      break;
    }
  }

  for (const QString& entry : entries) {
    QFileInfo fi(entry);

    if (fi.suffix().toLower() == "trk") {
      try {
        new CGisItemTrk(dir.absoluteFilePath(entry), this);
      } catch (int) {
        return false;
      }
    } else if (fi.suffix().toLower() == "wpt") {
      if (!loadWpts(dir.absoluteFilePath(entry), dir)) {
        return false;
      }
    }
  }

  return true;
}
