/**********************************************************************************************
    Copyright (C) 2017 Michel Durand <zero@cms123.fr>

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

#ifndef CLOGPROJECT_H
#define CLOGPROJECT_H

#include "gis/prj/IGisProject.h"
#include "gis/suunto/ISuuntoProject.h"

class CLogProject : public ISuuntoProject
{
    Q_DECLARE_TR_FUNCTIONS(CLogProject)
public:
    CLogProject(const QString &filename, CGisListWks * parent);
    virtual ~CLogProject() = default;

    const QString getFileDialogFilter() const override
    {
        return IGisProject::filedialogFilterLOG;
    }

    const QString getFileExtension() const override
    {
        return "log";
    }

    static void loadLog(const QString &filename, CLogProject *project);

private:
    void loadLog(const QString& filename);
};
#endif //CLOGPROJECT_H

