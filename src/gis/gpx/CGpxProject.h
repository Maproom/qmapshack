/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include <QDateTime>
#include <QDomElement>
#include <QUrl>

class CGisListWks;
class CGisDraw;

class CGpxProject : public IGisProject
{
public:
    CGpxProject(const QString &filename, CGisListWks * parent);
    CGpxProject(const QString &filename, IDevice * parent);
    CGpxProject(const QString &filename, const IGisProject * project, IDevice * parent);
    virtual ~CGpxProject();

    virtual const QString getFileDialogFilter() override
    {
        return IGisProject::filedialogFilterGPX;
    }

    virtual const QString getFileExtension() override
    {
        return "gpx";
    }

    virtual bool canSave() override
    {
        return true;
    }

    static bool saveAs(const QString& fn, IGisProject& project);

    static void loadGpx(const QString &filename, CGpxProject *project);

private:
    void loadGpx(const QString& filename);
};

#endif //CGPXPROJECT_H

