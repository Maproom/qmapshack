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

#include "gis/fit/defs/CFitFieldProfile.h"
#include "gis/fit/defs/CFitProfile.h"


CFitFieldProfile::CFitFieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr, float scale, uint16_t offset, QString units)
    : name(name), fieldDefNr(fieldDefNr), scale(scale), offset(offset), units(units),
    baseType(baseType), profile(nullptr), subfields(), components()
{
}

CFitFieldProfile::CFitFieldProfile() : CFitFieldProfile("unknown", &InvalidType, FieldDefNrInvalid, 0, 0, "")
{
}

CFitFieldProfile::CFitFieldProfile(const CFitFieldProfile& copy)
    : name(copy.name), fieldDefNr(copy.fieldDefNr), scale(copy.scale), offset(copy.offset), units(copy.units),
    baseType(copy.baseType), profile(copy.profile), subfields(copy.subfields), components(copy.components)
{
}

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

void CFitFieldProfile::addComponent(int subfieldIndex, CFitComponentfieldProfile* field)
{
    subfields[subfieldIndex]->addComponent(field);
}

bool CFitFieldProfile::hasSubfields() const
{
    return subfields.size() > 0;
}

bool CFitFieldProfile::hasComponents() const
{
    return components.size() > 0;
}


QString CFitFieldProfile::getName() const
{
    return name;
}

uint8_t CFitFieldProfile::getFieldDefNum() const
{
    return fieldDefNr;
}
float CFitFieldProfile::getScale() const
{
    if (scale == 0)
    {
        return 1;
    }
    return scale;
}
uint16_t CFitFieldProfile::getOffset() const
{
    return offset;
}
bool CFitFieldProfile::hasScaleAndOffset() const
{
    return scale != 0 || offset != 0;
}
QString CFitFieldProfile::getUnits() const
{
    return units;
}


const CFitBaseType& CFitFieldProfile::getBaseType() const
{
    return *baseType;
}

const CFitProfile* CFitFieldProfile::getProfile() const
{
    return profile;
}

void CFitFieldProfile::setParent(CFitProfile* parent)
{
    this->profile = parent;
}



const QList<CFitSubfieldProfile*>& CFitFieldProfile::getSubfields() const
{
    return subfields;
}

const QList<CFitComponentfieldProfile*>& CFitFieldProfile::getComponents() const
{
    return components;
}


CFitSubfieldProfile::CFitSubfieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr, float scale, uint16_t
                                         offset, QString units, uint8_t subRefFieldDefNr, uint8_t subRefFieldValue)
    : CFitFieldProfile(name, baseType, fieldDefNr, scale, offset, units),
    refFieldDefNr(subRefFieldDefNr), refFieldValue(subRefFieldValue)
{
}

uint8_t CFitSubfieldProfile::getReferencedFieldDefNr() const
{
    return refFieldDefNr;
}
uint8_t CFitSubfieldProfile::getReferencedFieldValue() const
{
    return refFieldValue;
}


CFitComponentfieldProfile::CFitComponentfieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr,
                                                     float scale, uint16_t offset, QString units, uint8_t componentFieldDefNr, uint8_t bits)
    : CFitFieldProfile(name, baseType, fieldDefNr, scale, offset, units), nrOfBits(bits), componentFieldDefNr(componentFieldDefNr)
{
}


const CFitFieldProfile* CFitComponentfieldProfile::getComponentField() const
{
    return getProfile()->getField(componentFieldDefNr);
}

uint8_t CFitComponentfieldProfile::getBits() const
{
    return nrOfBits;
}
uint32_t CFitComponentfieldProfile::getBitmask() const
{
    uint32_t bitmask = 0;
    for (int i = 0; i < nrOfBits; i++)
    {
        bitmask |= (1 << i);
    }
    return bitmask;
}