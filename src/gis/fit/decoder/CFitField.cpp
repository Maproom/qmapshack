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

#include "gis/fit/defs/fit_const.h"
#include "gis/fit/defs/CFitProfileLockup.h"
#include "gis/fit/decoder/CFitField.h"
#include "gis/fit/decoder/CFitDefinitionMessage.h"


CFitField::CFitField(CFitFieldDefinition* fieldDefinition, bool valid)
: valid(valid)
{
    globalMesgNr = fieldDefinition->parent()->getGlobalMesgNr();
    fieldDefNr = fieldDefinition->getDefNr();
    baseType = fieldDefinition->getBaseType();
    fieldProfile = fieldDefinition->profile();
    //fieldProfile = CFitProfileLockup::getFieldForProfile(fieldDefinition->parent()->getGlobalMesgNr(), fieldDefinition->getDefNr());

}


CFitField::CFitField(const CFitField& copy)
: globalMesgNr(copy.globalMesgNr), fieldDefNr(copy.fieldDefNr), baseType(copy.baseType), valid(copy.valid)
{
    fieldProfile = CFitProfileLockup::getFieldForProfile(globalMesgNr, fieldDefNr);
}

CFitField::CFitField(uint16_t globalMesgNr, uint8_t fieldDefNr, CFitBaseType* baseType, bool valid)
: globalMesgNr(globalMesgNr), fieldDefNr(fieldDefNr), baseType(baseType), valid(valid)
{
    fieldProfile = CFitProfileLockup::getFieldForProfile(globalMesgNr, fieldDefNr);
}

CFitField::CFitField() : CFitField(GlobalMesgNrInvalid, FieldDefNrInvalid, &InvalidType, false) {}

QString CFitField::fieldInfo()
{
        QString name = profile()->getName();
        QString str = QString("field %1 (%2): %4 %5 %3")
                .arg(name)
                .arg(getFieldDefNr())
                .arg(getBaseType()->str())
                .arg(getString())
                .arg(profile()->getUnits());
    return str;
}

uint8_t CFitField::getFieldDefNr() const
{
    return fieldDefNr;
}
bool CFitField::isValidValue() {
    return valid;
}

bool CFitField::isValidBaseType() const {
    return baseType != 0 && baseType->nr() != TypeInvalid;
}

CFitBaseType* CFitField::getBaseType() const
{
    return baseType;
}

bool CFitField::isBaseType(BaseTypeNr type) const
{
    return isValidBaseType() && baseType->nr() == type;
}


QString CFitDoubleField::getString()
{
    return QString("%1").arg(value, 0, 'f', 3);
}

uint8_t* CFitDoubleField::getBytes()
{
    // TODO
}

int CFitDoubleField::getSIntValue()
{
    return (value + 0.5);
}

unsigned int CFitDoubleField::getUIntValue()
{
    return (value + 0.5);
}



uint8_t* CFitStringField::getBytes()
{
    // TODO
}

int CFitStringField::getSIntValue()
{
    return value.toInt();
}

unsigned int CFitStringField::getUIntValue()
{
    return value.toUInt();
}

double CFitStringField::getDoubleValue()
{
    return value.toDouble();
}



QString CFitByteField::getString()
{
    return ""; // TODO
}

uint8_t* CFitByteField::getBytes()
{
    // TODO
    return value;
}

int CFitByteField::getSIntValue()
{
// TODO
}

unsigned int CFitByteField::getUIntValue()
{
    // TODO
}

double CFitByteField::getDoubleValue()
{
    // TODO
}