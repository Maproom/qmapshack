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
#include "CAppSetupWin.h"
#include "version.h"


void CAppSetupWin::initQMapShack()
{
    // setup environment variables for GDAL/Proj4
    QString apppath = QCoreApplication::applicationDirPath();
    apppath = apppath.replace("/", "\\");
    QString gdalDir = QString("%1\\data").arg(apppath);
    QString projDir = QString("%1\\share\\proj").arg(apppath);

    qunsetenv("GDAL_DRIVER_PATH");
    IAppSetup::prepareGdal(gdalDir, projDir);

    QString appResourceDir = QString("%1\\translations").arg(apppath).toUtf8();
    prepareTranslator(appResourceDir, "qtbase_");
    prepareTranslator(appResourceDir, "qmapshack_");

    // limit PATH to application directory in order to avoid that wrong .dll's are loaded
    path = apppath.toUtf8();
    qputenv("PATH", path);

    // create directories
    IAppSetup::path(defaultCachePath(), 0, true, "CACHE");
    IAppSetup::path(userDataPath("WaypointIcons"), 0, true, "USER DATA");
    IAppSetup::path(logDir(), 0, true, "LOG");
}


QString CAppSetupWin::routinoPath(QString xmlFile)
{
    QString apppath = QCoreApplication::applicationDirPath();
    apppath = apppath.replace("/", "\\");
    QDir dirXml(QString("%1\\routino-xml").arg(apppath).toUtf8());
    return IAppSetup::path(dirXml.absolutePath(), xmlFile, false, "ROUTINO");
}


QString CAppSetupWin::defaultCachePath()
{
    return IAppSetup::path(QDir::home().absolutePath(), ".QMapShack/", false, 0);
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
    QDir dirApp(QCoreApplication::applicationDirPath());
    QDir dirHelp(dirApp.absoluteFilePath(_MKSTR(HELPPATH)));
    return dirHelp.absoluteFilePath("QMSHelp.qhc");
}

