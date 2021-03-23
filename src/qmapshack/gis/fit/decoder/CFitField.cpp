/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg <>

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
#include "gis/fit/decoder/CFitField.h"
#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/defs/CFitFieldProfile.h"
#include "gis/fit/defs/CFitProfileLookup.h"
#include "gis/fit/defs/fit_const.h"

CFitField::CFitField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, QVariant value, bool valid)
    :  fieldProfile(profile), globalMesgNr(fieldDefinition.parent().getGlobalMesgNr()),
    fieldDefNr(fieldDefinition.getDefNr()), baseType(&fieldDefinition.getBaseType()), valid(valid), value(value), rawValue(value)
{
    applyScaleAndOffset();
}

CFitField::CFitField(quint16 globalMesgNr, quint8 fieldDefNr, const CFitFieldProfile* profile, QVariant value, bool valid)
    : fieldProfile(profile), globalMesgNr(globalMesgNr), fieldDefNr(fieldDefNr),
    baseType(&profile->getBaseType()), valid(valid), value(value), rawValue(value)
{
    applyScaleAndOffset();
}

CFitField::CFitField(const CFitField & copy)
    : fieldProfile(copy.fieldProfile), globalMesgNr(copy.globalMesgNr), fieldDefNr(copy.fieldDefNr),
    baseType(copy.baseType), valid(copy.valid), value(copy.value), rawValue(copy.rawValue)
{
}

CFitField::CFitField()
    : fieldProfile(CFitProfileLookup::getFieldForProfile(fitGlobalMesgNrInvalid, fitFieldDefNrInvalid)),
    globalMesgNr(fitGlobalMesgNrInvalid), fieldDefNr(fitFieldDefNrInvalid), baseType(&fit::InvalidType), valid(false), value(), rawValue()
{
}

void CFitField::applyScaleAndOffset()
{
    if(fieldProfile->hasScaleAndOffset())
    {
        // scale and offset is only for int / sint types
        if(baseType->isUnsignedInt())
        {
            value = QVariant(rawValue.toUInt() / profile().getScale() - profile().getOffset());
        }
        if(baseType->isSignedInt())
        {
            value = QVariant(rawValue.toInt() / profile().getScale() - profile().getOffset());
        }
    }
    else
    {
        value = rawValue;
    }
}

void CFitField::setProfile(const CFitFieldProfile* profile)
{
    fieldProfile = profile;
    applyScaleAndOffset();
}

QString CFitField::fieldInfo() const
{
    QString str = QString("%1 %2%3 (%4): %5 %6 %7 %8")
                  .arg(profile().getTyp(),
                       profile().getName(),
                       profile().getFieldType()  == eFieldTypeDevelopment ? " DEV" : "")
                  .arg(getFieldDefNr())
                  .arg(value.toString(),
                       profile().getUnits(),
                       getBaseType().name(),
                       (valid ? "" : "<invalid>"));

    if(getBaseType().isNumber())
    {
        str += QString(" (%1/%2-%3)")
               .arg(rawValue.toString(),
                    profile().getScale(),
                    profile().getOffset());
    }
    return str;
}
