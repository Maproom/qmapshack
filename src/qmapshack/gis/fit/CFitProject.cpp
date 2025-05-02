/**********************************************************************************************
    Copyright (C) 2015 Ivo Kronenberg <>

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

#include "gis/fit/CFitProject.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "gis/fit/CFitStream.h"
#include "gis/fit/defs/fit_enums.h"
#include "gis/fit/defs/fit_fields.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"

CFitProject::CFitProject(const QString& filename, CGisListWks* parent) : IGisProject(eTypeFit, filename, parent) {
  loadFitFromFile(filename, true);
}

CFitProject::CFitProject(const QString& filename, IDevice* parent) : IGisProject(eTypeFit, filename, parent) {
  // this constructor is used when opening files from the garmin device.
  // this means several files are opened at the same time. For that case we do not show an error message if a file
  // can not be opened.
  loadFitFromFile(filename, false);
}

CFitProject::CFitProject(QFile& file, const QString& filename, IDevice* parent)
    : IGisProject(eTypeFit, filename, parent) {
  loadFitFromFile(file, false);
}

void CFitProject::loadFitFromFile(const QString& filename, bool showErrorMsg) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/FitProject.png"));
  blockUpdateItems(true);
  try {
    tryOpeningFitFile(filename);
  } catch (QString& errormsg) {
    if (showErrorMsg) {
      QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Failed to load file %1...").arg(filename),
                            errormsg, QMessageBox::Abort);
    } else {
      qWarning() << "Failed to load FIT file:" << errormsg;
    }
    valid = false;
  }

  sortItems();
  blockUpdateItems(false);
}

void CFitProject::loadFitFromFile(QFile& file, bool showErrorMsg) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/FitProject.png"));
  blockUpdateItems(true);
  try {
    tryOpeningFitFile(file);
  } catch (QString& errormsg) {
    if (showErrorMsg) {
      QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Failed to load file %1...").arg(filename),
                            errormsg, QMessageBox::Abort);
    } else {
      qWarning() << "Failed to load FIT file:" << errormsg;
    }
    valid = false;
  }

  sortItems();
  blockUpdateItems(false);
}

void CFitProject::tryOpeningFitFile(const QString& filename) {
  // create file instance
  QFile file(filename);
  qDebug() << "reading FIT file" << filename;

  // if the file does not exist, the filename is assumed to be a name for a new project
  if (!file.exists()) {
    IGisProject::filename.clear();
    setupName(filename);
    setToolTip(CGisListWks::eColumnName, getInfo());
    valid = true;
    return;
  }
  if (!file.open(QIODevice::ReadOnly)) {
    throw tr("Failed to open FIT file %1.").arg(filename);
  }

  tryOpeningFitFile(file);
}

void CFitProject::tryOpeningFitFile(QFile& file) {
  try {
    createGisItems(file);
  } catch (QString& errormsg) {
    file.close();
    throw errormsg;
  }
  file.close();

  markAsSaved();

  setToolTip(CGisListWks::eColumnName, getInfo());
  valid = true;
}

void CFitProject::createGisItems(QFile& file) {
  CFitStream in(file);
  in.decodeFile();

  QString name = "";

  // remark: we consider activity and course files types. trk is for both types. There is one trk per fit file
  const CFitMessage& mesg = in.firstMesgOf(eMesgNumFileId);
  if (mesg.getFieldValue(eFileIdType).toUInt() == eFileActivity ||
      mesg.getFieldValue(eFileIdType).toUInt() == eFileCourse ||
      mesg.getFieldValue(eFileIdType).toUInt() == eFileSegment) {
    CGisItemTrk* trk = new CGisItemTrk(in, this);
    name = trk->getName();
  }
  // fit does not have routes
  // new CGisItemRte(in, this);

  // Locations file containing waypoints
  if (mesg.getFieldValue(eFileIdType).toUInt() == eFileLocation) {
    while (in.nextMesgOf(eMesgNumLocation).isValid()) {
      new CGisItemWpt(in, this);
    }
  }

  in.reset();
  // course point is a message of a course file. Thus, wpt is only for a course file. There might be n wpt per fit file
  while (in.nextMesgOf(eMesgNumCoursePoint).isValid()) {
    CGisItemWpt* wpt = new CGisItemWpt(in, this);
    if (name.length() == 0) {
      name = wpt->getName();
    }
  }
  // ql:area is not directly available in FIT (could be calculated)

  QString tmp = QFileInfo(filename).completeBaseName().replace("_", " ");
  if (!name.isEmpty()) {
    tmp += QString("(%1)").arg(name);
  }

  setupName(tmp);
}

CFitProject::~CFitProject() {}
