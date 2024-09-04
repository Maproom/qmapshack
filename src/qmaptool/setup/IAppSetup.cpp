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

#include "setup/IAppSetup.h"

#include <gdal.h>

#include "helpers/CSettings.h"
#include "setup/CAppSetupLinux.h"
#include "setup/CAppSetupMac.h"
#include "setup/CAppSetupWin.h"
#include "setup/CCommandProcessor.h"
#include "setup/CLogHandler.h"

IAppSetup* IAppSetup::pSelf = nullptr;

IAppSetup& IAppSetup::self() { return *pSelf; }

IAppSetup::~IAppSetup() {
  SETTINGS;
  cfg.setValue("ExtTools/pathGdaladdoOverride", pathGdaladdoOverride);
  cfg.setValue("ExtTools/pathGdaltranslateOverride", pathGdaltranslateOverride);
  cfg.setValue("ExtTools/pathGdalwarpOverride", pathGdalwarpOverride);
  cfg.setValue("ExtTools/pathGdalbuildvrtOverride", pathGdalbuildvrtOverride);
  cfg.setValue("ExtTools/pathQmtrgb2pctOverride", pathQmtrgb2pctOverride);
  cfg.setValue("ExtTools/pathQmtmap2jnxOverride", pathQmtmap2jnxOverride);
}

IAppSetup& IAppSetup::createInstance(QObject* parent) {
  if (pSelf != nullptr) {
    return self();
  }
#if defined(Q_OS_MAC)
  new CAppSetupMac(parent);
#elif defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD) || defined(__FreeBSD_kernel__) || defined(__GNU__) || \
    defined(Q_OS_CYGWIN)
  new CAppSetupLinux(parent);
#elif defined(Q_OS_WIN32)
  new CAppSetupWin(parent);
#else
#error OS not supported
#endif
  return self();
}

void IAppSetup::prepareToolPaths() {
  pathGdaladdo = this->findExecutable("gdaladdo");
  pathGdaltranslate = this->findExecutable("gdal_translate");
  pathGdalwarp = this->findExecutable("gdalwarp");
  pathGdalbuildvrt = this->findExecutable("gdalbuildvrt");
  pathQmtrgb2pct = this->findExecutable("qmt_rgb2pct");
  pathQmtmap2jnx = this->findExecutable("qmt_map2jnx");

  SETTINGS;
  pathGdaladdoOverride = cfg.value("ExtTools/pathGdaladdoOverride", pathGdaladdoOverride).toString();
  pathGdaltranslateOverride = cfg.value("ExtTools/pathGdaltranslateOverride", pathGdaltranslateOverride).toString();
  pathGdalwarpOverride = cfg.value("ExtTools/pathGdalwarpOverride", pathGdalwarpOverride).toString();
  pathGdalbuildvrtOverride = cfg.value("ExtTools/pathGdalbuildvrtOverride", pathGdalbuildvrtOverride).toString();
  pathQmtrgb2pctOverride = cfg.value("ExtTools/pathQmtrgb2pctOverride", pathQmtrgb2pctOverride).toString();
  pathQmtmap2jnxOverride = cfg.value("ExtTools/pathQmtmap2jnxOverride", pathQmtmap2jnxOverride).toString();
}

void IAppSetup::prepareGdal(QString gdalDir, QString projDir) {
  if (!gdalDir.isEmpty()) {
    qputenv("GDAL_DATA", gdalDir.toUtf8());
    qDebug() << "GDAL_DATA directory set to " + gdalDir;
  }

  if (!projDir.isEmpty()) {
    qputenv("PROJ_LIB", projDir.toUtf8());
    qDebug() << "PROJ_LIB directory set to " + projDir;
  }

  GDALAllRegister();
}

QString IAppSetup::path(QString path, QString subdir, bool mkdir, QString debugName) {
  QDir pathDir(path);

  if (!subdir.isNull()) {
    pathDir = QDir(pathDir.absoluteFilePath(subdir));
  }
  if (mkdir && !pathDir.exists()) {
    pathDir.mkpath(pathDir.absolutePath());
    qDebug() << debugName << "path created" << pathDir.absolutePath();
  } else if (!debugName.isNull()) {
    qDebug() << debugName << "path" << pathDir.absolutePath();
  }
  return pathDir.absolutePath();
}

void IAppSetup::prepareTranslator(QString translationPath, QString translationPrefix) {
  QString locale = QLocale::system().name();
  QDir dir(translationPath);
  if (!QFile::exists(dir.absoluteFilePath(translationPrefix + locale))) {
    locale = locale.left(2);
  }
  qDebug() << "locale" << locale;

  QApplication* app = (QApplication*)QCoreApplication::instance();
  QTranslator* qtTranslator = new QTranslator(app);
  if (qtTranslator->load(translationPrefix + locale, translationPath)) {
    app->installTranslator(qtTranslator);
    qDebug() << "using file '" + translationPath + "/" + translationPrefix + locale + ".qm' for translations.";
  } else {
    qWarning() << "no file found for translations '" + translationPath + "/" + translationPrefix + locale +
                      "' (using default).";
  }
}

void IAppSetup::initLogHandler() { CLogHandler::initLogHandler(logDir(), qlOpts->logfile, qlOpts->debug); }

CAppOpts* qlOpts = nullptr;

void IAppSetup::processArguments() {
  CCommandProcessor cmdParse;
  qlOpts = cmdParse.processOptions(QCoreApplication::instance()->arguments());
}
