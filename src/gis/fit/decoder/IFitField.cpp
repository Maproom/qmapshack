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

#include "gis/fit/decoder/CFitDefinitionMessage.h"
#include "gis/fit/decoder/IFitField.h"
#include "gis/fit/defs/CFitProfileLockup.h"
#include "gis/fit/defs/fit_const.h"


IFitField::IFitField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, bool valid)
    :  fieldProfile(profile), globalMesgNr(fieldDefinition.parent().getGlobalMesgNr()), fieldDefNr(fieldDefinition.getDefNr()),
    baseType(&fieldDefinition.getBaseType()), valid(valid)
{
}


IFitField::IFitField(const IFitField & copy)
    : fieldProfile(copy.fieldProfile), globalMesgNr(copy.globalMesgNr), fieldDefNr(copy.fieldDefNr), baseType(copy.baseType), valid(copy.valid)
{
}

IFitField::IFitField(uint16_t globalMesgNr, uint8_t fieldDefNr, const CFitFieldProfile* profile, bool valid)
    : fieldProfile(profile), globalMesgNr(globalMesgNr), fieldDefNr(fieldDefNr),baseType(&profile->getBaseType()), valid(valid)
{
}

IFitField::IFitField() : fieldProfile(CFitProfileLockup::getFieldForProfile(fitGlobalMesgNrInvalid,
                                                                            fitFieldDefNrInvalid))
{
    globalMesgNr = fitGlobalMesgNrInvalid;
    fieldDefNr = fitFieldDefNrInvalid;
    baseType = &InvalidType;
    valid = false;
}

QString IFitField::fieldInfo() const
{
    QString name = profile().getName();
    QString str = QString("%1 %2 (%3): %4 %5 %6 %7")
                  .arg(profile().getTyp())
                  .arg(name)
                  .arg(getFieldDefNr())
                  .arg(getString())
                  .arg(profile().getUnits())
                  .arg(getBaseType().name())
                  .arg(valid ? "" : "<invalid>");
    return str;
}

uint16_t IFitField::getGlobalMesgNr() const
{
    return globalMesgNr;
}

uint8_t IFitField::getFieldDefNr() const
{
    return fieldDefNr;
}
bool IFitField::isValidValue() const
{
    return valid;
}


const CFitBaseType&IFitField::getBaseType() const
{
    return *baseType;
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
}

unsigned int CFitByteField::getUIntValue() const
{
    return value.toUInt();
}

double CFitByteField::getDoubleValue() const
{
    return value.toDouble();
}