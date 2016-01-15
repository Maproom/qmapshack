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
    for (IFitField *field : mesg.getFields())
    {
        CFitFieldBuilder::evaluateFieldProfile(mesg, field);
    }
    for (IFitField *field : mesg.getFields())
    {
        CFitFieldBuilder::expandComponents(mesg, field);
    }
}

IFitField *CFitFieldBuilder::buildField(const CFitFieldDefinition &def, quint8 *fieldData, const CFitMessage& message)
{
    CFitByteDataTransformer::swapFieldData(def, fieldData);
    const CFitBaseType& baseType = def.getBaseType();
    const CFitFieldProfile* fieldProfile = CFitProfileLookup::getFieldForProfile(message.getGlobalMesgNr(), def.getDefNr());
    if (baseType.isSignedInt())
    {
        return buildSIntField(def, fieldProfile, fieldData);
    }
    else if (baseType.isUnsignedInt())
    {
        return buildUIntField(def, fieldProfile, fieldData);
    }
    else if (baseType.isFloat())
    {
        return buildFloatField(def, fieldProfile, fieldData);
    }
    else if (baseType.isString())
    {
        return buildStringField(def, fieldProfile, fieldData);
    }
    else if (baseType.isByte())
    {
        return buildByteField(def, fieldProfile, fieldData);
    }
    else
    {
        // should not be possible
        throw QString("FIT unknown base type");
    }
}


IFitField *CFitFieldBuilder::buildSIntField(const CFitFieldDefinition &def, const CFitFieldProfile* fieldProfile, quint8 *fieldData)
{
    const CFitBaseType &baseType = def.getBaseType();
    const quint8 *invalidBytes = baseType.invalidValueBytes();
    bool valid = false;
    if (def.getSize() == baseType.size())
    {
        for (quint8 i = 0; i < def.getSize(); i++)
        {
            if (fieldData[i] != invalidBytes[i])
            {
                valid = true;
                break;
            }
        }
    }
    int ival = CFitByteDataTransformer::getSIntValue(baseType, fieldData);
    return new CFitIntField<qint32>(def, fieldProfile, ival, valid);
}

IFitField *CFitFieldBuilder::buildUIntField(const CFitFieldDefinition &def, const CFitFieldProfile* fieldProfile, quint8 *fieldData)
{
    const CFitBaseType& baseType = def.getBaseType();
    const quint8 *invalidBytes = baseType.invalidValueBytes();
    bool valid = false;
    if (def.getSize() == baseType.size())
    {
        for (quint8 i = 0; i < def.getSize(); i++)
        {
            if (fieldData[i] != invalidBytes[i])
            {
                valid = true;
                break;
            }
        }
    }
    unsigned int uval = CFitByteDataTransformer::getUIntValue(baseType, fieldData);
    return new CFitIntField<quint32>(def, fieldProfile, uval, valid);
}


IFitField *CFitFieldBuilder::buildFloatField(const CFitFieldDefinition &def, const CFitFieldProfile* fieldProfile, quint8 *fieldData)
{
    const CFitBaseType& baseType = def.getBaseType();
    const quint8 *invalidBytes = baseType.invalidValueBytes();
    bool valid = false;
    if (def.getSize() == baseType.size())
    {
        for (quint8 i = 0; i < def.getSize(); i++)
        {
            if (fieldData[i] != invalidBytes[i])
            {
                valid = true;
                break;
            }
        }
    }
    qreal dval = CFitByteDataTransformer::getFloatValue(baseType, fieldData);
    return new CFitFloatField(def, fieldProfile, dval, valid);
}


IFitField *CFitFieldBuilder::buildStringField(const CFitFieldDefinition &def, const CFitFieldProfile* fieldProfile, quint8 *fieldData)
{
    const quint8 *invalidBytes = def.getBaseType().invalidValueBytes();
    QString str = CFitByteDataTransformer::getString(fieldData, def.getSize());
    // all byts set to invalid value
    quint8 invalidCount = 0;
    for (quint8 i = 0; i < def.getSize(); i++)
    {
        if (fieldData[i] == invalidBytes[0])
        {
            invalidCount++;
        }
    }
    bool valid = (invalidCount < def.getSize());
    return new CFitStringField(def, fieldProfile, str, valid);
}


IFitField *CFitFieldBuilder::buildByteField(const CFitFieldDefinition &def, const CFitFieldProfile* fieldProfile, quint8 *fieldData)
{
    const quint8 *invalidBytes = def.getBaseType().invalidValueBytes();
    // all byts set to invalid value
    quint8 invalidCount = 0;
    for (quint8 i = 0; i < def.getSize(); i++)
    {
        if (fieldData[i] == invalidBytes[0])
        {
            invalidCount++;
        }
    }
    bool valid = (invalidCount < def.getSize());
    return new CFitByteField(def, fieldProfile, CFitByteDataTransformer::getBytes(fieldData, def.getSize()), valid);
}


void CFitFieldBuilder::evaluateFieldProfile(CFitMessage& mesg, IFitField* field)
{
    const CFitFieldProfile& fieldProfile = field->profile();
    // case subfield
    if (fieldProfile.hasSubfields())
    {
        for (const CFitSubfieldProfile* subfieldProfile : fieldProfile.getSubfields())
        {
            // the referenced field is for all subfields the same
            for (IFitField *referencedField : mesg.getFields())
            {
                if (referencedField->getFieldDefNr() == subfieldProfile->getReferencedFieldDefNr() &&
                    referencedField->getUIntValue() == subfieldProfile->getReferencedFieldValue())
                {
                    // the value of the referenced field matches with the field profile reference-value
                    field->setProfile(subfieldProfile);
                }
            }
        }
    }
}

void CFitFieldBuilder::expandComponents(CFitMessage& mesg, const IFitField* field)
{
    // TODO accumulated fields are not implemented (no need so far)
    const CFitFieldProfile& fieldProfile = field->profile();
    // case component
    if (fieldProfile.hasComponents())
    {
        int offset = 0;
        for (const CFitComponentfieldProfile* compProfile : fieldProfile.getComponents())
        {
            if (field->getBaseType().isSignedInt())
            {
                qint32 value = (field->getSIntValue() >> offset) & compProfile->getBitmask();
                mesg.addField(new CFitIntField<qint32>(field->getGlobalMesgNr(), compProfile->getFieldDefNum(), compProfile, value, true));
            }
            else
            {
                quint32 value = (field->getUIntValue() >> offset) & compProfile->getBitmask();
                mesg.addField(new CFitIntField<quint32>(field->getGlobalMesgNr(), compProfile->getFieldDefNum(), compProfile, value, true));
            }
            offset += compProfile->getBits();
        }
    }
}
