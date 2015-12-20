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

#include "gis/fit/defs/CFitFieldProfile.h"
#include "gis/fit/defs/CFitProfile.h"


CFitFieldProfile::CFitFieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr, float scale, uint16_t offset, QString units)
        : name(name), fieldDefNr(fieldDefNr), scale(scale), offset(offset), units(units),
          baseType(baseType), profile(nullptr), subfields(), components() {}

CFitFieldProfile::CFitFieldProfile(): CFitFieldProfile("unknown", &InvalidType, FieldDefNrInvalid, 0, 0, "") {}

CFitFieldProfile::CFitFieldProfile(const CFitFieldProfile& copy)
        : name(copy.name), fieldDefNr(copy.fieldDefNr), scale(copy.scale), offset(copy.offset), units(copy.units),
          baseType(copy.baseType), profile(copy.profile), subfields(copy.subfields), components(copy.components) { }

void CFitFieldProfile::addSubfield(CFitSubfieldProfile* subfield)
{
    subfields.append(subfield);
    subfield->setParent(profile);
}

void CFitFieldProfile::addComponent(CFitComponentfieldProfile* component)
{
    components.append(component);
    component->setParent(profile);
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


CFitBaseType* CFitFieldProfile::getBaseType()
{
    return baseType;
}

CFitProfile* CFitFieldProfile::getProfile()
{
    return profile;
}

void CFitFieldProfile::setParent(CFitProfile* parent)
{
    this->profile = parent;
}

CFitSubfieldProfile* CFitFieldProfile::getSubfieldByIndex(int idx)
{
    return subfields.at(idx);
}

QList<CFitSubfieldProfile*> CFitFieldProfile::getSubfields()
{
    return subfields;
}

QList<CFitComponentfieldProfile*> CFitFieldProfile::getComponents()
{
    return components;
}


CFitSubfieldProfile::CFitSubfieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr, float scale, uint16_t
offset, QString units, uint8_t subRefFieldDefNr, uint8_t subRefFieldValue)
        : CFitFieldProfile(name, baseType, fieldDefNr, scale, offset, units),
          refFieldDefNr(subRefFieldDefNr), refFieldValue(subRefFieldValue) { }

uint8_t CFitSubfieldProfile::getReferencedFieldDefNr()
{
    return refFieldDefNr;
}
uint8_t CFitSubfieldProfile::getReferencedFieldValue()
{
    return refFieldValue;
}



CFitComponentfieldProfile::CFitComponentfieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr,
                                                     float scale, uint16_t  offset, QString units, uint8_t componentFieldDefNr, uint8_t bits)
        : CFitFieldProfile(name, baseType, fieldDefNr, scale, offset, units), nrOfBits(bits), componentFieldDefNr(componentFieldDefNr) {}


CFitFieldProfile* CFitComponentfieldProfile::getComponentField()
{
    return getProfile()->getField(componentFieldDefNr);
}

uint8_t CFitComponentfieldProfile::getBits()
{
    return nrOfBits;
}
uint32_t CFitComponentfieldProfile::getBitmask()
{
    uint32_t bitmask = 0;
    for (int i = 0; i < nrOfBits; i++)
    {
        bitmask |= (1 << i);
    }
    return bitmask;
}