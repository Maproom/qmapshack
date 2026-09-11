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

#ifndef IAPPSETUP_H
#define IAPPSETUP_H

#include <QApplication>
#include <QtCore>

class IAppSetup {
 public:
  static IAppSetup* getPlatformInstance();
  virtual void initQMapShack() = 0;
  void initLogHandler();
  void processArguments();

  /**
     @brief Export a `--locale` option to the environment

     Desktop integrations translate their own contributions to the GUI - on KDE the standard button
     labels - through catalogs they pick by `LANGUAGE`, and they load them while the platform plugin
     comes up, inside the `QApplication` constructor. So this has to run before it, from `main()`, or
     `--locale` leaves those strings in the desktop's language.

     @param argc the argument count as passed to `main()`
     @param argv the argument list as passed to `main()`
   */
  static void exportLocaleEnv(int argc, char** argv);

  virtual QString routinoPath(QString xmlFile) = 0;
  virtual QString defaultCachePath() = 0;
  virtual QString userDataPath(QString subdir = 0) = 0;
  virtual QString logDir() = 0;
  virtual QString findExecutable(const QString& name) = 0;
  virtual QString helpFile() = 0;
  virtual bool setLock() = 0;

 protected:
  void prepareGdal(QString gdalDataDir, QString gdalPluginsDir, QString projDataDir);
  /**
     @brief Install the Qt and application translators for the effective locale

     The application catalog decides: without one for the locale Qt's catalog is not loaded either.

     @param appPath the directory holding the application's `.qm` files
     @param appPrefix the application catalog prefix including the trailing underscore
     @param qtPath the directory holding Qt's `.qm` files
   */
  void prepareTranslators(const QString& appPath, const QString& appPrefix, const QString& qtPath);

  static IAppSetup* instance;

  QString path(QString path, QString subdir, bool mkdir, QString debugName);
};

#endif  // IAPPSETUP_H
