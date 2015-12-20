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

#include "gis/fit/defs/CFitProfileLockup.h"
#include "gis/fit/decoder/CFitFieldDataState.h"
#include "gis/fit/defs/fit_const.h"
#include "gis/fit/defs/fit_fields.h"
#include "CFitByteDataTransformer.h"


class CFitFieldBuilder
{
public:
    static void evaluateFieldProfile(CFitMessage* message, CFitField* field)
    {
        CFitFieldProfile* fieldProfile = field->profile();
        // case subfield
        if(fieldProfile->hasSubfields())
        {
            for(CFitSubfieldProfile* subfieldProfile : fieldProfile->getSubfields())
            {
                // the referenced field is for all subfields the same
                CFitField* referencedField = message->getField(subfieldProfile->getReferencedFieldDefNr());
                if(referencedField->getUIntValue() == subfieldProfile->getReferencedFieldValue())
                {
                    // the value of the referenced field matches with the field profile reference-value
                    field->setProfile(subfieldProfile);
                }
            }
        }
    }

    static void expandComponents(CFitMessage* message, CFitField* field)
    {
        // todo accumulated fields
        CFitFieldProfile* fieldProfile = field->profile();
        // case component
        if(fieldProfile->hasComponents())
        {
            int offset = 0;
            for(CFitComponentfieldProfile* compProfile : fieldProfile->getComponents())
            {
                if(field->getBaseType()->isSignedInt())
                {
                    int32_t value = (field->getSIntValue() >> offset) & compProfile->getBitmask();
                    CFitIntField<int32_t>* f = new CFitIntField<int32_t>(field, compProfile, value, true);
                    message->addField(f);

                }
                else
                {
                    uint32_t value = (field->getUIntValue() >> offset) & compProfile->getBitmask();
                    CFitIntField<uint32_t>* f = new CFitIntField<uint32_t>(field, compProfile, value, true);
                    message->addField(f);
                }
                offset += compProfile->getBits();
            }
        }
    }

    static CFitField* buildField(CFitFieldDefinition* def, uint8_t* fieldData, CFitMessage* message)
    {
        swapFieldData(def, fieldData);
        CFitBaseType* baseType = def->getBaseType();
        bool valid = isValidValue(fieldData, def->getSize(), baseType);
        CFitFieldProfile* fieldProfile = CFitProfileLockup::getFieldForProfile(message->getGlobalMesgNr(), def->getDefNr());
        if(baseType->isSignedInt())
        {
            int ival = CFitByteDataTransformer::getIntValue(baseType, fieldData);
            CFitIntField<int32_t>* f = new CFitIntField<int32_t>(def, fieldProfile, ival, valid);
            return f;
        }
        else if(baseType->isUnsignedInt())
        {
            unsigned  int uval = CFitByteDataTransformer::getUIntValue(baseType, fieldData);
            CFitIntField<uint32_t>* f = new CFitIntField<uint32_t>(def, fieldProfile, uval, valid);
            return f;
        }
        else if(baseType->isFloat())
        {
            double dval = CFitByteDataTransformer::getDoubleValue(baseType, fieldData);
            CFitDoubleField* f = new CFitDoubleField(def, fieldProfile, dval, valid);
            return f;
        }
        else if(baseType->nr() == TypeString)
        {
            QString str = CFitByteDataTransformer::getString(fieldData);
            CFitStringField* f = new CFitStringField(def, fieldProfile, str, valid);
            return f;
        }
        else if(baseType->nr() == TypeByte)
        {
            CFitByteField* f = new CFitByteField(def, fieldProfile, (uint8_t*)fieldData, def->getSize(), valid);
            return f;
        }
        else
        {
            // TODO more info
            qWarning() << "FIT unknown base type";
            return nullptr;
        }
    }

private:
    static void swapFieldData(CFitFieldDefinition *fieldDef, uint8_t* fieldData)
    {
        if (fieldDef->getEndianAbilityFlag() && (fieldDef->parent()->getArchitectureBit() != ArchEndianLittle)) {
            // Swap the bytes for each element.
            int typeSize = fieldDef->getBaseType()->size();
            int elements = fieldDef->getSize() / typeSize;

            for (int element = 0; element < elements; element++) {
                for (int i = 0; i < (typeSize / 2); i++) {
                    uint8_t tmp = (uint8_t)fieldData[element * typeSize + i];
                    fieldData[element * typeSize + i] = fieldData[element * typeSize + typeSize - i - 1];
                    fieldData[element * typeSize + typeSize - i - 1] = tmp;
                }
            }
        }
    }

    static bool isValidValue(uint8_t* rawData, uint8_t size, CFitBaseType* baseType)
    {
        uint8_t* invalidBytes = baseType->invalidValueBytes();
        if (baseType->nr() == TypeString || baseType->nr() == TypeByte)
        {
            // all byts set to invalid value
            uint8_t invalidCount = 0;
            for(uint8_t i = 0; i < size; i++)
            {
                // string and enum specify one byte for invalid value
                if(rawData[i] == invalidBytes[0])
                    invalidCount++;
            }
            return (invalidCount == size);
        }
        else if (baseType->isNumber())
        {
            if(size != baseType->size())
                return false;

            for(uint8_t i = 0; i < size; i++)
            {
                if(rawData[i] != invalidBytes[i])
                    return true;
            }
            return false;
        }
        // invalid type
        return false;
    }
};


void CFitFieldDataState::reset() {
    fieldDataIndex = 0;
    fieldIndex = 0;
}


DecodeState CFitFieldDataState::process(uint8_t &dataByte) {
    CFitFieldDefinition *fieldDef = defintion(latestMessage()->getLocalMesgNr())->getFieldByIndex(fieldIndex);

    // add the read byte to the data array
    fieldData[fieldDataIndex++] = dataByte;

    if (fieldDataIndex >= fieldDef->getSize()) {
        // all bytes are read for current field

        // new field with data
        CFitField* field = CFitFieldBuilder::buildField(fieldDef, fieldData, latestMessage());
        if (field->isValidBaseType())
        {
            // Ignore field of unknown base type.
            latestMessage()->addField(field);

            // The special case time record.
            // timestamp has always the same value for all enums. it does not matter againts which we're comparing.
            if (fieldDef->getDefNr() == RecordTimestamp) {
                setTimestamp(field->getUIntValue());
            }
        }
        // new field follows, reset
        fieldDataIndex = 0;
        fieldIndex++;
    }

    if (fieldIndex >= defintion(latestMessage()->getLocalMesgNr())->getNrOfFields()) {
        // Now that the entire message is decoded we may evaluate subfields and expand components
        for (CFitField *field : latestMessage()->getFields())
        {
            CFitFieldBuilder::evaluateFieldProfile(latestMessage(), field);
        }
        for (CFitField *field : latestMessage()->getFields())
        {
            CFitFieldBuilder::expandComponents(latestMessage(), field);
        }
        reset();
        // TODO macro
        // qDebug() << latestMessage()->messageInfo();
        // after all fields read, go to next record header
        return StateRecord;
    }

    // there are more fields to read for the current message
    return StateFieldData;
};
