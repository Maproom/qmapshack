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
#include "CFitByteDataTransformer.h"


CFitField::CFitField(const CFitFieldDefinition& fieldDefinition, CFitFieldProfile* profile, bool valid)
: valid(valid)
{
    globalMesgNr = fieldDefinition.parent()->getGlobalMesgNr();
    fieldDefNr = fieldDefinition.getDefNr();
    baseType = fieldDefinition.getBaseType();
    fieldProfile = profile;
}


CFitField::CFitField(const CFitField& copy)
: globalMesgNr(copy.globalMesgNr), fieldProfile(copy.fieldProfile), fieldDefNr(copy.fieldDefNr), baseType(copy.baseType),  valid(copy.valid)
{ }

CFitField::CFitField(uint16_t globalMesgNr, uint8_t fieldDefNr, CFitFieldProfile* profile, bool valid)
: globalMesgNr(globalMesgNr), fieldProfile(profile), fieldDefNr(fieldDefNr),baseType(profile->getBaseType()), valid(valid) { }

CFitField::CFitField()
{
    globalMesgNr = GlobalMesgNrInvalid;
    fieldDefNr = FieldDefNrInvalid;
    baseType = &InvalidType;
    fieldProfile = CFitProfileLockup::getFieldForProfile(globalMesgNr, fieldDefNr);
    valid = false;
}

QString CFitField::fieldInfo() const
{
        QString name = profile()->getName();
        QString str = QString("%1 %2 (%3): %4 %5 %6 %7")
                .arg(profile()->getTyp())
                .arg(name)
                .arg(getFieldDefNr())
                .arg(getString())
                .arg(profile()->getUnits())
                .arg(getBaseType()->str())
                .arg(valid ? "" : "<invalid>");
    return str;
}

uint16_t CFitField::getGlobalMesgNr() const
{
    return globalMesgNr;
}

uint8_t CFitField::getFieldDefNr() const
{
    return fieldDefNr;
}
bool CFitField::isValidValue() const {
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


QString CFitFloatField::getString() const
{
    return QString("%1").arg(value, 0, 'f', 3);
}

QByteArray CFitFloatField::getBytes() const
{
    return QByteArray((const char*)&value, sizeof(double));
}

int CFitFloatField::getSIntValue() const
{
    return (int)(value + 0.5);
}

unsigned int CFitFloatField::getUIntValue() const
{
    return (unsigned int) (value + 0.5);
}



QByteArray CFitStringField::getBytes() const
{
    return value.toLatin1();
}

int CFitStringField::getSIntValue() const
{
    return value.toInt();
}

unsigned int CFitStringField::getUIntValue() const
{
    return value.toUInt();
}

double CFitStringField::getDoubleValue() const
{
    return value.toDouble();
}



QString CFitByteField::getString() const
{
    return QString(value);
}

QByteArray CFitByteField::getBytes() const
{
    return value;
}

int CFitByteField::getSIntValue() const
{
    return value.toInt();
    /*
    switch(size)
    {
        case 0:
            return 0;
        case 1:
            return CFitByteDataTransformer::getSint8(value.t);
        case 2:
            return CFitByteDataTransformer::getSint16(value);
        default:
            return CFitByteDataTransformer::getSint32(value);
    }
     */
}

unsigned int CFitByteField::getUIntValue() const
{
     return value.toUInt();
     /*
    switch(size)
    {
        case 0:
            return 0;
        case 1:
            return CFitByteDataTransformer::getUint8(value);
        case 2:
            return CFitByteDataTransformer::getUint16(value);
        default:
            return CFitByteDataTransformer::getUint32(value);
    }
     */
}

double CFitByteField::getDoubleValue() const
{
    return value.toDouble();
    /*
    if(size == 4)
        return CFitByteDataTransformer::getFloat32(value);
    if(size== 8)
        return CFitByteDataTransformer::getFloat64(value);

    return 0;
     */
}