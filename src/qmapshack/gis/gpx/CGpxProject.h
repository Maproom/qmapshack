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

#ifndef CGPXPROJECT_H
#define CGPXPROJECT_H

#include "gis/prj/IGisProject.h"

class CGisListWks;
class CGisDraw;
class QFile;

class CGpxProject : public IGisProject {
  Q_DECLARE_TR_FUNCTIONS(CGpxProject)
 public:
  CGpxProject(const QString& filename, CGisListWks* parent);
  CGpxProject(const QString& filename, IDevice* parent);
  CGpxProject(QFile& file, const QString& filename, IDevice* parent);
  CGpxProject(const QString& filename, const IGisProject* project, IDevice* parent);
  virtual ~CGpxProject();

  const QString getFileDialogFilter() const override { return IGisProject::filedialogFilterGPX; }

  const QString getFileExtension() const override { return "gpx"; }

  bool canSave() const override { return true; }

  static bool saveAs(const QString& fn, IGisProject& project, bool strictGpx11);
  static bool saveAs(QFile& file, const QString& filename, IGisProject& project, bool strictGpx11);

  static void loadGpx(const QString& filename, CGpxProject* project);

 private:
  void loadGpx(const QString& filename);
  static void loadGpx(QFile& file, const QString& filename, CGpxProject* project);
};

#endif  // CGPXPROJECT_H
