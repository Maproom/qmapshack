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

#include "gis/IGisProject.h"

class CQmsProject : public IGisProject
{
    public:
        CQmsProject(const QString &name,  CGisListWks * parent, const QString& key);
        CQmsProject(const QString& filename, const QString &key, CGisListWks * parent);
        virtual ~CQmsProject();

        void save();
        void saveAs();

        static void saveAs(const QString& fn, IGisProject& project);

};

#endif //CQMSPROJECT_H

