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

#ifndef CFITPROJECT_H
#define CFITPROJECT_H

#include "gis/prj/IGisProject.h"

#include <QtCore>


class CFitProject : public IGisProject
{
public:
    CFitProject(const QString& filename, CGisListWks * parent);
    CFitProject(const QString& filename, IDevice * parent);
    virtual ~CFitProject();


    virtual const QString getFileDialogFilter() const override
    {
        return IGisProject::filedialogFilterFIT;
    }

    virtual const QString getFileExtension() const override
    {
        return "fit";
    }

    virtual bool canSave() const override
    {
        return false;
    }


private:
    void loadFit(const QString & filename);
};

#endif //CFITPROJECT_H

