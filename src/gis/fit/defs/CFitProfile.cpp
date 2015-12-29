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


CFitProfile::CFitProfile(QString name, uint16_t globalMesgNr) : name(name), globalMesgNr(globalMesgNr), fields() { }

CFitProfile::CFitProfile() : CFitProfile("unknown", GlobalMesgNrInvalid) {}

CFitProfile::CFitProfile(const CFitProfile& copy)
: name(copy.name), globalMesgNr(copy.globalMesgNr), fields(copy.fields) {}

void CFitProfile::addField(CFitFieldProfile* field)
{
    fields.insert(field->getFieldDefNum(), field);
    field->setParent(this);
}


// dummy field profile for unkown definitions
static CFitFieldProfile dummyFieldProfile = CFitFieldProfile();

const CFitFieldProfile* CFitProfile::getField(uint8_t fieldDefNr) const
{
    if (fields.contains(fieldDefNr))
    {
        return fields[fieldDefNr];
    }
    return &dummyFieldProfile;
}

void CFitProfile::addSubfield(uint8_t fieldDefNr, CFitSubfieldProfile* field)
{
    fields[fieldDefNr]->addSubfield(field);

}

void CFitProfile::addComponent(uint8_t fieldDefNr, CFitComponentfieldProfile* field)
{
    fields[fieldDefNr]->addComponent(field);
}

void CFitProfile::addComponent(uint8_t fieldDefNr, int subfieldIndex, CFitComponentfieldProfile* field)
{
    fields[fieldDefNr]->addComponent(subfieldIndex, field);
}