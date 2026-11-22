/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CROUTINODATABASEBUILDER_H
#define CROUTINODATABASEBUILDER_H

#include <QTemporaryDir>
#include <memory>

#include "tool/IToolShell.h"
#include "ui_IRoutinoDatabaseBuilder.h"

class CRoutinoDatabaseBuilder : public IToolShell, private Ui::IRoutinoDatabaseBuilder {
  Q_OBJECT
 public:
  CRoutinoDatabaseBuilder(QWidget* parent);
  virtual ~CRoutinoDatabaseBuilder();

 private slots:
  void slotSelectSourceFiles();
  void slotSetupPaths();
  void slotStart();

  void enabelStartButton();
  void slotLinkActivated(const QUrl& url);

 private:
  void finished(int exitCode, QProcess::ExitStatus status) override;
  /** @return false if a built file could not replace the one in the target path */
  bool moveIntoTarget();
  /** @brief Offer Routino's registered paths as targets: a database built elsewhere would not be listed */
  void fillPaths();

  bool first = false;
  bool tainted = false;
  bool last = false;

  QStringList sourceFiles;
  QString targetPrefix;
  QString targetPath;
  /** where planetsplitter writes: Routino and its folder watcher look at the target path's files only */
  std::unique_ptr<QTemporaryDir> buildDir;

  QString planetsplitter;
};

#endif  // CROUTINODATABASEBUILDER_H
