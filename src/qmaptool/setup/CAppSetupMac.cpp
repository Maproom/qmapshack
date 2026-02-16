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

#include <QtSystemDetection>
#if defined(Q_OS_MAC)

#include "setup/CAppSetupMac.h"

#include <QWindow>

#include "signal.h"
#include "unistd.h"

#include "misc.h"

const QString CAppSetupMac::relTranslationDir = "Resources/translations";  // app
const QString CAppSetupMac::relGdalDataDir    = "Resources/gdal";          // app
const QString CAppSetupMac::relGdalPluginsDir = "Resources/gdalplugins";   // app
const QString CAppSetupMac::relProjDataDir    = "Resources/proj";          // app
const QString CAppSetupMac::relHelpDir        = "Resources/help";          // app
const QString CAppSetupMac::relBinDir         = "Tools";                   // app
const QString CAppSetupMac::relLogDir         = "Library/Logs";            // home

void CAppSetupMac::extendPath() {
  const QProcessEnvironment& env = QProcessEnvironment::systemEnvironment();
  const QStringList& envlist = env.toStringList();
  QString value = "";
  for (int i = 0; i < envlist.size(); i++) {
    QString entry = envlist[i];
    if (entry.startsWith("PATH=")) {
      int index = entry.indexOf("=");

      if (index != -1) {
        value = entry.right(entry.length() - (index + 1)) + ":";
      }
      break;
    }
  }
  const QString& binDir = getApplicationDir(relBinDir).absolutePath();
  qDebug() << "BIN" << binDir;
  value += binDir;
  qputenv("PATH", value.toLatin1().constData());

  prepareToolPaths();
}

void CAppSetupMac::initQMapTool() {
  extendPath();
  // setup gdal
  const QString& gdalDataDir = getApplicationDir(relGdalDataDir).absolutePath();
  const QString& gdalPluginsDir = getApplicationDir(relGdalPluginsDir).absolutePath();
  const QString& projDataDir = getApplicationDir(relProjDataDir).absolutePath();
  prepareGdal(gdalDataDir, gdalPluginsDir, projDataDir);

  // setup translators
  const QString& translationPath = getApplicationDir(relTranslationDir).absolutePath();
  prepareTranslator(translationPath, "qtbase_");
  prepareTranslator(translationPath, "qmaptool_");

  // create directories
  IAppSetup::path(defaultCachePath(), 0, true, "CACHE");
  IAppSetup::path(userDataPath(), 0, true, "USER DATA");
  IAppSetup::path(logDir(), 0, false, "LOG");

  migrateDirContent(defaultCachePath());
  migrateDirContent(userDataPath());

  // catch signal SIGTERM
  closeOnSIGTERM();
}

QString CAppSetupMac::defaultCachePath() {
  const QString& cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
  return IAppSetup::path(cachePath, 0, false, 0);
}

QString CAppSetupMac::userDataPath(QString subdir) {
  const QString& dataDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  return IAppSetup::path(dataDir, subdir, false, 0);
}

QString CAppSetupMac::logDir() {
  const QString& home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  return IAppSetup::path(home, relLogDir, false, 0);
}

QString CAppSetupMac::findExecutable(const QString &name) {
  const QStringList& bundlePath = QStringList(QApplication::applicationDirPath() + "/../Tools");
  QString path = QStandardPaths::findExecutable(name, bundlePath);
  if (path.isEmpty()) path = QStandardPaths::findExecutable(name);
  return path;
}

QDir CAppSetupMac::getApplicationDir(QString subdir) {
  QDir appDir(QCoreApplication::applicationDirPath());
  appDir.cdUp();
  appDir.cd(subdir);
  return appDir;
}

void CAppSetupMac::migrateDirContent(QString dest) {
  QString src = dest;
  src.replace("/QLandkarte/", "/");
  QDir dirDest = QDir(dest);
  QDir dirSource = QDir(src);

  if (!dirDest.exists() && dirSource.exists()) {
    qDebug() << "src directory for migration" << src;
    qDebug() << "dst directory for migration" << dest;

    QDir wdir;
    QString newdir = dest;
    newdir.remove("/QMapTool");
    wdir.mkdir(newdir);
    qDebug() << "directory created" << newdir;

    qDebug() << "migrate data from " << dirSource.absolutePath() << "to" << dirDest.absolutePath();
    QDir mvDir;
    if (!mvDir.rename(dirSource.absolutePath(), dirDest.absolutePath())) {
      qDebug() << "error migrating directory" << dirSource;
    }
  }
}

QString CAppSetupMac::helpFile() {
  QDir dirHelp(getApplicationDir(relHelpDir));
  return dirHelp.absoluteFilePath("QMTHelp.qhc");
}

void CAppSetupMac::closeOnSIGTERM() {
  sig_t handler = [](int sig)->void {
    for (auto const item : qApp->topLevelWindows()) {
      // Close application gracefully on signal SIGTERM
      if (item->objectName() == "IMainWindowWindow") {
        qDebug() << "closing on SIGTERM";
        item->close();
        break;
      }
    }
  };

  signal(SIGTERM, handler);
}

#endif // defined(Q_OS_MAC)
