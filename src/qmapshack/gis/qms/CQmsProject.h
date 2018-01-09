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

#ifndef CQMSPROJECT_H
#define CQMSPROJECT_H

#include "gis/prj/IGisProject.h"

class CQmsProject : public IGisProject
{
    Q_DECLARE_TR_FUNCTIONS(CQmsProject)
public:
    CQmsProject(const QString& filename, CGisListWks * parent);
    virtual ~CQmsProject() = default;

    const QString getFileDialogFilter() const override
    {
        return IGisProject::filedialogFilterQMS;
    }

    const QString getFileExtension() const override
    {
        return "qms";
    }

    bool canSave() const override
    {
        return true;
    }

    static bool saveAs(const QString& fn, IGisProject& project);
};

#endif //CQMSPROJECT_H

