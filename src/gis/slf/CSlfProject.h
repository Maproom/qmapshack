/**********************************************************************************************
    Copyright (C) 2015 Christian Eichler code@christian-eichler.de

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

#ifndef CSLFPROJECT_H
#define CSLFPROJECT_H

#include "gis/prj/IGisProject.h"

#include <QDateTime>
#include <QDomElement>
#include <QUrl>

class CGisListWks;
class CGisDraw;

class CSlfProject : public IGisProject
{
public:
    CSlfProject(const QString &filename, CGisListWks * parent, bool readFile = true);
    virtual ~CSlfProject();

    bool save();
    bool saveAs();

    static bool saveAs(const QString& fn, IGisProject& project);

private:
    void loadSlf(const QString &filename);
};

#endif // CSLFPROJECT_H

