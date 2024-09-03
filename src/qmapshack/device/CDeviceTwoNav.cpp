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

#include "device/CDeviceTwoNav.h"

#include <QtWidgets>

#include "gis/CGisListWks.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/tnv/CTwoNavProject.h"

CDeviceTwoNav::CDeviceTwoNav(const QString& path, const QString& key, const QString& model, QTreeWidget* parent)
    : IDevice(path, eTypeTwoNav, key, parent) {
  setText(CGisListWks::eColumnName, QString("TwoNav (%1)").arg(model));
  setToolTip(CGisListWks::eColumnName, QString("TwoNav (%1)").arg(model));

  if (QFile::exists(dir.absoluteFilePath("RegInfo.ini"))) {
    readReginfo(dir.absoluteFilePath("RegInfo.ini"));
  } else if (QFile::exists(dir.absoluteFilePath("AppData/RegInfo.ini"))) {
    readReginfo(dir.absoluteFilePath("AppData/RegInfo.ini"));
  }

  pathData = "Data/";
  QDir dirData(dir.absoluteFilePath(pathData));

  if (!dirData.exists()) {
    pathData = "TwoNavData/Data/";
    dirData.setPath(dir.absoluteFilePath(pathData));
  }

  {
    IGisProject* project = new CTwoNavProject(dirData.absolutePath(), this);
    if (!project->isValid()) {
      delete project;
    }
  }

  const QStringList& entriesGpx = dirData.entryList(QStringList("*.gpx"));
  for (const QString& entry : entriesGpx) {
    IGisProject* project = new CGpxProject(dirData.absoluteFilePath(entry), this);
    if (!project->isValid()) {
      delete project;
    }
  }

  const QStringList& entriesDir = dirData.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
  for (const QString& entry : entriesDir) {
    IGisProject* project = new CTwoNavProject(dirData.absoluteFilePath(entry), this);
    if (!project->isValid()) {
      delete project;
    }
  }

  // special case: read the gpx files in the track log directory.
  dirData.setPath(dir.absoluteFilePath(pathData + "Tracklog"));
  const QStringList& entriesLog = dirData.entryList(QStringList("*.gpx"));
  for (const QString& entry : entriesLog) {
    IGisProject* project = new CGpxProject(dirData.absoluteFilePath(entry), this);
    if (!project->isValid()) {
      delete project;
    }
  }
}

CDeviceTwoNav::~CDeviceTwoNav() {}

void CDeviceTwoNav::readReginfo(const QString& filename) {
  QString product, unittype;
  static const QRegularExpression re(QRegularExpression::anchoredPattern("(.*)=(.*)"));
  QFile file(filename);
  file.open(QIODevice::ReadOnly);

  while (!file.atEnd()) {
    QString line = file.readLine().simplified();

    const QRegularExpressionMatch& match = re.match(line);
    if (match.hasMatch()) {
      QString tok = match.captured(1);
      QString val = match.captured(2);

      if (tok == "product") {
        product = val;
      } else if (tok == "unittype") {
        unittype = val;
      }
    }
  }

  if (!product.isEmpty() && !unittype.isEmpty()) {
    setText(CGisListWks::eColumnName, QString("%1 (%2)").arg(product, unittype));
  }
}

void CDeviceTwoNav::insertCopyOfProject(IGisProject* project) {
  static const QRegularExpression re("[^A-Za-z0-9_]");
  QString name = project->getName();
  name = name.remove(re);

  QDir dirData = dir.absoluteFilePath(pathData);
  QString filename = dirData.absoluteFilePath(name);

  if (testForExternalProject(filename)) {
    return;
  }

  CTwoNavProject* proj = new CTwoNavProject(filename, project, this);
  if (!proj->isValid()) {
    delete proj;
    return;
  }

  if (!proj->save()) {
    proj->remove();
    delete proj;
    return;
  }
}
