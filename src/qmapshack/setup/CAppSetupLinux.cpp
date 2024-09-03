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

#include "CAppSetupLinux.h"

#include "config.h"
#include "version.h"

#ifndef _MKSTR_1
#define _MKSTR_1(x) #x
#define _MKSTR(x) _MKSTR_1(x)
#endif

void CAppSetupLinux::initQMapShack() {
  prepareGdal("", "");

  // setup translators
  QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
  QString translationPath = QCoreApplication::applicationDirPath();
  static const QRegularExpression re("bin$");
  translationPath.replace(re, "share/qmapshack/translations");
  prepareTranslator(resourceDir, "qt_");
  prepareTranslator(translationPath, "qmapshack_");

  // create directories
  IAppSetup::path(defaultCachePath(), 0, true, "CACHE");
  IAppSetup::path(userDataPath("WaypointIcons"), 0, true, "USER DATA");
  IAppSetup::path(logDir(), 0, true, "LOG");
}

QString CAppSetupLinux::routinoPath(QString xmlFile) {
  QDir dirXml(_MKSTR(ROUTINO_XML_PATH));
  return IAppSetup::path(dirXml.absolutePath(), xmlFile, false, "ROUTINO");
}

QString CAppSetupLinux::defaultCachePath() {
  return IAppSetup::path(QDir::home().absolutePath(), ".QMapShack/", false, 0);
}

QString CAppSetupLinux::userDataPath(QString subdir) {
  QString path = QDir::home().absoluteFilePath(CONFIGDIR);
  return IAppSetup::path(path, subdir, false, 0);
}

QString CAppSetupLinux::logDir() { return QDir::temp().absolutePath(); }

QString CAppSetupLinux::helpFile() {
  QDir dir(_MKSTR(HELPPATH));
  return dir.absoluteFilePath("QMSHelp.qhc");
}
