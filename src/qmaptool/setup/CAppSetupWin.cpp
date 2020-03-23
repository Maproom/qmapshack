/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg

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

#include "config.h"
#include "setup/CAppSetupWin.h"
#include "version.h"


void CAppSetupWin::initQMapTool()
{
    // setup environment variables for GDAL/Proj4
    QString apppath = QCoreApplication::applicationDirPath();
    apppath = apppath.replace("/", "\\");
    QString gdalDir = QString("%1\\data").arg(apppath);
    QString projDir = QString("%1\\share\\proj").arg(apppath);

    qunsetenv("GDAL_DRIVER_PATH");
    prepareGdal(gdalDir, projDir);

    QString appResourceDir = QString("%1\\translations").arg(apppath).toUtf8();
    prepareTranslator(appResourceDir, "qtbase_");
    prepareTranslator(appResourceDir, "qmaptool_");

    // limit PATH to application directory in order to avoid that wrong .dll's are loaded
    path = apppath.toUtf8();
    qputenv("PATH", path);

    // create directories
    IAppSetup::path(logDir(), 0, true, "LOG");

    prepareToolPaths();
}

QString CAppSetupWin::defaultCachePath()
{
    return IAppSetup::path(QDir::home().absolutePath(), ".QMapTool/", false, 0);
}


QString CAppSetupWin::userDataPath(QString subdir)
{
    QString path = QDir::home().absoluteFilePath(CONFIGDIR);
    return IAppSetup::path(path, subdir, false, 0);
}


QString CAppSetupWin::logDir()
{
    return QDir::temp().absolutePath();
}

QString CAppSetupWin::findExecutable(const QString &name)
{
    return QStandardPaths::findExecutable(name);
}

QString CAppSetupWin::helpFile()
{
    QDir dirApp = QDir(QCoreApplication::applicationDirPath());
    QDir dirHelp = QDir(dirApp.absoluteFilePath(_MKSTR(HELPPATH)));
    return dirHelp.absoluteFilePath("QMTHelp.qhc");
}
