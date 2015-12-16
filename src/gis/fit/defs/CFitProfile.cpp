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

#include "gis/fit/defs/CFitProfile.h"
#include "gis/fit/defs/fit_const.h"


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
CFitFieldProfile dummyFieldProfile = CFitFieldProfile();

CFitFieldProfile * CFitProfile::getField(uint8_t fieldDefNr)
{
    if (fields.contains(fieldDefNr))
    {
        return fields[fieldDefNr];
    }
    return &dummyFieldProfile;
}



CFitFieldProfile::CFitFieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr, float scale, uint16_t offset, QString units) :
name(name), baseType(baseType), fieldDefNr(fieldDefNr), scale(scale), offset(offset), units(units), components(), subfields(), parent(nullptr),
nrOfBits(0) {}

CFitFieldProfile::CFitFieldProfile(): CFitFieldProfile("unknown", &InvalidType, FieldDefNrInvalid, 0, 0, "") {}

CFitFieldProfile::CFitFieldProfile(const CFitFieldProfile& copy)
: name(copy.name), baseType(copy.baseType), fieldDefNr(copy.fieldDefNr), scale(copy.scale), offset(copy.offset), units(copy.units),
nrOfBits(copy.nrOfBits), parent(copy.parent), subfields(copy.subfields), components(copy.components)
{
}

void CFitFieldProfile::addComponent(CFitFieldProfile * component)
{

}

bool CFitFieldProfile::hasSubfields()
{
    return subfields.size() > 0;
}

bool CFitFieldProfile::hasComponents()
{
    return components.size() > 0;
}


QString CFitFieldProfile::getName()
{
    return name;
}

uint8_t CFitFieldProfile::getFieldDefNum()
{
    return fieldDefNr;
}
float CFitFieldProfile::getScale()
{
    if (scale == 0)
        return 1;
    return scale;
}
uint16_t CFitFieldProfile::getOffset()
{
    return offset;
}
bool CFitFieldProfile::hasScaleAndOffset()
{
    return scale != 0 || offset != 0;
}
QString CFitFieldProfile::getUnits()
{
    return units;
}
uint8_t CFitFieldProfile::getBits()
{
    return nrOfBits;
}
uint32_t CFitFieldProfile::getBitmask()
{
    uint32_t bitmask = 0;
    for (int i = 0; i < nrOfBits; i++)
    {
        bitmask |= (1 << i);
    }
    return bitmask;
}
uint8_t CFitFieldProfile::getArrayLength()
{
    return 0;
}
CFitProfile* CFitFieldProfile::getParent()
{
    return parent;
}

void CFitFieldProfile::setParent(CFitProfile* parent)
{
    this->parent = parent;
}
/*
QList<CFitFieldProfile*> CFitFieldProfile::getComponents()
{
    return components;
}

QList<CFitFieldProfile>& CFitFieldProfile::getSubfields()
{
    return subfields;
}
 */