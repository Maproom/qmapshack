/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CQLBPROJECT_H
#define CQLBPROJECT_H

#include "gis/prj/IGisProject.h"

class CQlbProject : public IGisProject
{
    Q_DECLARE_TR_FUNCTIONS(CQlbProject)
public:
    CQlbProject(const QString& filename, CGisListWks* parent);
    virtual ~CQlbProject() = default;

    const QString getFileDialogFilter() const override
    {
        return IGisProject::filedialogFilterQLB;
    }

    const QString getFileExtension() const override
    {
        return "qlb";
    }

private:
    void load(const QString& filename);
    void loadWpts(QByteArray& array);
    void loadTrks(QByteArray& array);
    void loadRtes(QByteArray& array);
    void loadOvls(QByteArray& array);
};

#endif //CQLBPROJECT_H

