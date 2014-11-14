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

#ifndef CDBPROJECT_H
#define CDBPROJECT_H

#include "gis/IGisProject.h"

class CDBProject : public IGisProject
{
    public:
        CDBProject(quint64 id, CGisListWks * parent);
        virtual ~CDBProject();

        void save();
        void saveAs();

};

#endif //CDBPROJECT_H

