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


CFitFieldProfile::CFitFieldProfile(CFitProfile* parent, QString name, CFitBaseType& baseType, quint8 fieldDefNr, qreal scale, quint16 offset, QString units)
    : name(name), fieldDefNr(fieldDefNr), scale(scale), offset(offset), units(units),
    baseType(&baseType), profile(parent), subfields(), components() {}

CFitFieldProfile::CFitFieldProfile() : CFitFieldProfile(nullptr, "unknown", InvalidType, fitFieldDefNrInvalid, 0, 0, "") {}

CFitFieldProfile::CFitFieldProfile(const CFitFieldProfile& copy)
    : name(copy.name), fieldDefNr(copy.fieldDefNr), scale(copy.scale), offset(copy.offset), units(copy.units),
    baseType(copy.baseType), profile(copy.profile), subfields(copy.subfields), components(copy.components) {}

CFitFieldProfile::~CFitFieldProfile()
{
    qDeleteAll(subfields);
    qDeleteAll(components);
}

void CFitFieldProfile::addSubfield(CFitSubfieldProfile* subfield)
{
    subfields.append(subfield);
}

void CFitFieldProfile::addComponent(CFitComponentfieldProfile* component)
{
    components.append(component);
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

quint8 CFitFieldProfile::getFieldDefNum() const
{
    return fieldDefNr;
}
qreal CFitFieldProfile::getScale() const
{
    if (scale == 0)
    {
        return 1;
    }
    return scale;
}
quint16 CFitFieldProfile::getOffset() const
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


CFitBaseType& CFitFieldProfile::getBaseType() const
{
    return *baseType;
}

const CFitProfile* CFitFieldProfile::getProfile() const
{
    return profile;
}

const QList<CFitSubfieldProfile*> CFitFieldProfile::getSubfields() const
{
    return subfields;
}

 QList<CFitComponentfieldProfile*> CFitFieldProfile::getComponents() const
{
    return components;
}


CFitSubfieldProfile::CFitSubfieldProfile(CFitProfile* parent, QString name, CFitBaseType& baseType, quint8 fieldDefNr, qreal scale, quint16
                                         offset, QString units, quint8 subRefFieldDefNr, quint8 subRefFieldValue)
    : CFitFieldProfile(parent, name, baseType, fieldDefNr, scale, offset, units),
    refFieldDefNr(subRefFieldDefNr), refFieldValue(subRefFieldValue)
{
}

CFitSubfieldProfile::CFitSubfieldProfile(const CFitSubfieldProfile& copy)
        : CFitFieldProfile(copy), refFieldDefNr(copy.refFieldDefNr), refFieldValue(copy.refFieldValue) {}

quint8 CFitSubfieldProfile::getReferencedFieldDefNr() const
{
    return refFieldDefNr;
}
quint8 CFitSubfieldProfile::getReferencedFieldValue() const
{
    return refFieldValue;
}


CFitComponentfieldProfile::CFitComponentfieldProfile(CFitProfile* parent, QString name, CFitBaseType& baseType, quint8 fieldDefNr,
                                                     qreal scale, quint16 offset, QString units, quint8 componentFieldDefNr, quint8 bits)
    : CFitFieldProfile(parent, name, baseType, fieldDefNr, scale, offset, units), nrOfBits(bits), componentFieldDefNr(componentFieldDefNr)
{
}

CFitComponentfieldProfile::CFitComponentfieldProfile(const CFitComponentfieldProfile& copy)
    : CFitFieldProfile(copy), nrOfBits(copy.nrOfBits), componentFieldDefNr(copy.componentFieldDefNr) {}


QString CFitComponentfieldProfile::getName() const  {
    return getProfile()->getField(componentFieldDefNr)->getName();
}
 quint8 CFitComponentfieldProfile::getFieldDefNum() const  {
    return getProfile()->getField(componentFieldDefNr)->getFieldDefNum();
}
 qreal CFitComponentfieldProfile::getScale() const
 {
     return CFitFieldProfile::getScale();
 }

 quint16 CFitComponentfieldProfile::getOffset() const
 {
     return CFitFieldProfile::getOffset();
 }
 bool CFitComponentfieldProfile::hasScaleAndOffset() const
 {
     return CFitFieldProfile::hasScaleAndOffset();
 }
 QString CFitComponentfieldProfile::getUnits() const
 {
     return CFitFieldProfile::getUnits();
 }
 CFitBaseType& CFitComponentfieldProfile::getBaseType() const
 {
     return getProfile()->getField(componentFieldDefNr)->getBaseType();
 }

quint8 CFitComponentfieldProfile::getBits() const
{
    return nrOfBits;
}
quint32 CFitComponentfieldProfile::getBitmask() const
{
    quint32 bitmask = 0;
    for (int i = 0; i < nrOfBits; i++)
    {
        bitmask |= (1 << i);
    }
    return bitmask;
}