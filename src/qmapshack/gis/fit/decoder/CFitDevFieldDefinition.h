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

#ifndef CFITDEVFIELDDEFINITION_H
#define CFITDEVFIELDDEFINITION_H

#include <QtCore>

// content of the field description messages
class CFitDevFieldDefinition final
{
public:
    CFitDevFieldDefinition(quint8 fieldNr, quint8 size, quint8 devDataIndex);
    CFitDevFieldDefinition() = default;

    QString fieldInfo() const;

    quint8 getFieldNr() const;
    quint8 getSize() const;
    quint8 getDevDataIndex() const;

private:
    quint8 fieldNr = 0;
    quint8 size = 0;
    quint8 devDataIndex = 0;
};

#endif // CFITDEVFIELDDEFINITION_H
