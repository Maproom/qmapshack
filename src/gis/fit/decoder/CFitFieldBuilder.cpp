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


#include "gis/fit/decoder/CFitByteDataTransformer.h"
#include "gis/fit/decoder/CFitFieldBuilder.h"
#include "gis/fit/defs/CFitProfileLookup.h"


void CFitFieldBuilder::evaluateSubfieldsAndExpandComponents(CFitMessage& mesg)
{
    for (const CFitField & field : mesg.getFields())
    {
        CFitFieldBuilder::evaluateFieldProfile(mesg, field);
    }
    for (const CFitField & field : mesg.getFields())
    {
        CFitFieldBuilder::expandComponents(mesg, field);
    }
}

CFitField CFitFieldBuilder::buildField(const CFitFieldDefinition &def, quint8 *fieldData, const CFitMessage& message)
{
    CFitByteDataTransformer::swapFieldData(def, fieldData);
    const CFitBaseType& baseType = def.getBaseType();
    const CFitFieldProfile* fieldProfile = CFitProfileLookup::getFieldForProfile(message.getGlobalMesgNr(), def.getDefNr());

    QVariant value;
    if (baseType.isSignedInt())
    {
        value = QVariant(CFitByteDataTransformer::getSIntValue(baseType, fieldData));
    }
    else if (baseType.isUnsignedInt())
    {
        value = QVariant(CFitByteDataTransformer::getUIntValue(baseType, fieldData));
    }
    else if (baseType.isFloat())
    {
        value = QVariant(CFitByteDataTransformer::getFloatValue(baseType, fieldData));
    }
    else if (baseType.isString())
    {
        value = QVariant(CFitByteDataTransformer::getString(fieldData, def.getSize()));
    }
    else if (baseType.isByte())
    {
        value = QVariant(CFitByteDataTransformer::getBytes(fieldData, def.getSize()));
    }
    else
    {
        // should not be possible
        throw QString("FIT unknown base type");
    }
    bool valid = isValueValid(def, fieldData);
    return CFitField(def, fieldProfile, value, valid);
}


bool CFitFieldBuilder::isValueValid(const CFitFieldDefinition &def, quint8 *fieldData)
{
    const CFitBaseType &baseType = def.getBaseType();
    const quint8 *invalidBytes = baseType.invalidValueBytes();
    quint8 invalidCount = 0;

    if(!baseType.isSizeUndefined() && def.getSize() != baseType.size())
    {
        return false;
    }
    for (quint8 i = 0; i < def.getSize(); i++)
    {
        quint8 b = baseType.isSizeUndefined() ? invalidBytes[0] : invalidBytes[i];
        if (fieldData[i] == b)
        {
            invalidCount++;
        }
    }
    return invalidCount < def.getSize();
}

void CFitFieldBuilder::evaluateFieldProfile(CFitMessage& mesg,  const CFitField & field)
{
    const CFitFieldProfile& fieldProfile = field.profile();
    // case subfield
    if (fieldProfile.hasSubfields())
    {
        for (const CFitSubfieldProfile* subfieldProfile : fieldProfile.getSubfields())
        {
            // the referenced field is for all subfields the same
            for (const CFitField & referencedField : mesg.getFields())
            {
                if (referencedField.getFieldDefNr() == subfieldProfile->getReferencedFieldDefNr() &&
                    referencedField.getValue().toUInt() == subfieldProfile->getReferencedFieldValue())
                {
                    // the value of the referenced field matches with the field profile reference-value
                    mesg.updateFieldProfile(field.getFieldDefNr(), subfieldProfile);
                }
            }
        }
    }
}

void CFitFieldBuilder::expandComponents(CFitMessage& mesg, const CFitField & field)
{
    // TODO accumulated fields are not implemented (no need so far)
    const CFitFieldProfile& fieldProfile = field.profile();
    // case component
    if (fieldProfile.hasComponents())
    {
        int offset = 0;
        for (const CFitComponentfieldProfile* compProfile : fieldProfile.getComponents())
        {
            if (field.getBaseType().isSignedInt())
            {
                qint32 value = (field.getValue().toInt() >> offset) & compProfile->getBitmask();
                CFitField compField = CFitField(field.getGlobalMesgNr(), compProfile->getFieldDefNum(), compProfile, QVariant(value), true);
                mesg.addField(compField);
            }
            else
            {
                quint32 value = (field.getValue().toUInt() >> offset) & compProfile->getBitmask();
                CFitField compField = CFitField(field.getGlobalMesgNr(), compProfile->getFieldDefNum(), compProfile, QVariant(value), true);
                mesg.addField(compField);
            }
            offset += compProfile->getBits();
        }
    }
}
