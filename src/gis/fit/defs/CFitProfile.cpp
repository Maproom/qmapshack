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

#include "gis/fit/defs/CFitProfile.h"


CFitProfile::CFitProfile(QString name, quint16 globalMesgNr) : name(name), globalMesgNr(globalMesgNr), fields()
{
}

CFitProfile::CFitProfile() : CFitProfile("unknown", fitGlobalMesgNrInvalid)
{
}

CFitProfile::CFitProfile(const CFitProfile& copy)
    : name(copy.name), globalMesgNr(copy.globalMesgNr), fields(copy.fields)
{
}

CFitProfile::~CFitProfile()
{
    qDeleteAll(fields);
}



// dummy field profile for unkown definitions
static CFitFieldProfile dummyFieldProfile = CFitFieldProfile();

const CFitFieldProfile* CFitProfile::getField(quint8 fieldDefNr) const
{
    if (fields.contains(fieldDefNr))
    {
        // return fields.constFind(fieldDefNr).operator->();
        return fields[fieldDefNr];
    }
    return &dummyFieldProfile;
}


void CFitProfile::addField(QString name, const CFitBaseType& baseType, quint8 fieldDefNr, qreal scale, quint16 offset, QString units)
{
    CFitFieldProfile* field= new CFitFieldProfile(this, name, baseType, fieldDefNr, scale, offset, units);
    fields.insert(fieldDefNr, field);
}
void CFitProfile::addSubfield(QString name, const CFitBaseType& baseType, quint8 fieldDefNr, qreal
                              scale, quint16 offset, QString units, quint8 subRefFieldDefNr, quint8 subRefFieldValue)
{
    CFitSubfieldProfile* field = new CFitSubfieldProfile(this, name, baseType, fieldDefNr, scale, offset, units, subRefFieldDefNr, subRefFieldValue);
    fields[fieldDefNr]->addSubfield(field);
}
void CFitProfile::addComponent(QString name, const CFitBaseType& baseType, quint8 fieldDefNr, qreal scale, quint16 offset, QString units, quint8 componentFieldDefNr, quint8 bits)
{
    CFitComponentfieldProfile* field = new CFitComponentfieldProfile(this, name, baseType, fieldDefNr, scale, offset, units, componentFieldDefNr, bits);
    fields[fieldDefNr]->addComponent(field);
}
void CFitProfile::addComponent(int subfieldIndex, QString name, const CFitBaseType& baseType, quint8 fieldDefNr, qreal scale, quint16 offset, QString units, quint8 componentFieldDefNr, quint8 bits)
{
    CFitComponentfieldProfile* field = new  CFitComponentfieldProfile(this, name, baseType, fieldDefNr, scale, offset, units, componentFieldDefNr, bits);
    fields[fieldDefNr]->addComponent(subfieldIndex, field);
}

