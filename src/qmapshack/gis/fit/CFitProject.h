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

#ifndef CFITPROJECT_H
#define CFITPROJECT_H

#include <QtCore>

#include "gis/prj/IGisProject.h"

class CFitStream;

class CFitProject final : public IGisProject {
  Q_DECLARE_TR_FUNCTIONS(CFitProject)
 public:
  CFitProject(const QString& filename, CGisListWks* parent);
  CFitProject(const QString& filename, IDevice* parent);
  CFitProject(QFile &file, const QString &filename, IDevice* parent);
  virtual ~CFitProject();

  const QString getFileDialogFilter() const override { return IGisProject::filedialogFilterFIT; }

  const QString getFileExtension() const override { return "fit"; }

  bool canSave() const override { return false; }

 private:
  void loadFitFromFile(const QString& filename, bool showErrorMsg);
  void loadFitFromFile(QFile& file, bool showErrorMsg);
  void tryOpeningFitFile(const QString& filename);
  void tryOpeningFitFile(QFile& file);
  void createGisItems(QFile& file);
};

#endif  // CFITPROJECT_H
