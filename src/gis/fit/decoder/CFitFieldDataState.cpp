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
#include "gis/fit/defs/fit_fields.h"
#include "CFitByteDataTransformer.h"


class CFitFieldBuilder
{
public:
    static void evaluateSubfieldsAndExpandComponents(CFitMessage* mesg)
    {
        for (CFitField* field : mesg->getFields())
        {
            CFitFieldBuilder::evaluateFieldProfile(mesg, field);
        }
        for (CFitField* field : mesg->getFields())
        {
            CFitFieldBuilder::expandComponents(mesg, field);
        }
    }

    static CFitField* buildField(const CFitFieldDefinition& def, uint8_t* fieldData, const CFitMessage* message)
    {
        swapFieldData(def, fieldData);
        CFitBaseType* baseType = def.getBaseType();
        CFitFieldProfile* fieldProfile = CFitProfileLockup::getFieldForProfile(message->getGlobalMesgNr(), def.getDefNr());
        if(baseType->isSignedInt())
        {
            return buildSIntField(def, fieldProfile, fieldData);
        }
        else if(baseType->isUnsignedInt())
        {
            return buildUIntField(def, fieldProfile, fieldData);
        }
        else if(baseType->isFloat())
        {
            return buildFloatField(def, fieldProfile, fieldData);
        }
        else if(baseType->isString())
        {
            return buildStringField(def, fieldProfile, fieldData);
        }
        else if(baseType->isByte())
        {
           return buildByteField(def, fieldProfile, fieldData);
        }
        else
        {
            // should not be possible
            throw QString("FIT unknown base type");
        }
    }





private:
    static CFitField* buildSIntField(const CFitFieldDefinition& def, CFitFieldProfile* fieldProfile, uint8_t* fieldData)
    {
        CFitBaseType* baseType = def.getBaseType();
        uint8_t* invalidBytes = baseType->invalidValueBytes();
        bool valid = false;
        if(def.getSize() != baseType->size())
        {
            for(uint8_t i = 0; i < def.getSize(); i++)
            {
                if(fieldData[i] != invalidBytes[i])
                {
                    valid =  true;
                    break;
                }
            }
        }
        int ival = CFitByteDataTransformer::getSIntValue(baseType, fieldData);
        CFitIntField<int32_t>* f = new CFitIntField<int32_t>(def, fieldProfile, ival, valid);
        return f;
    }

    static CFitField* buildUIntField(const CFitFieldDefinition& def, CFitFieldProfile* fieldProfile, uint8_t* fieldData)
    {
        CFitBaseType* baseType = def.getBaseType();
        uint8_t* invalidBytes = baseType->invalidValueBytes();
        bool valid = false;
        if(def.getSize() != baseType->size())
        {
            for(uint8_t i = 0; i < def.getSize(); i++)
            {
                if(fieldData[i] != invalidBytes[i])
                {
                    valid =  true;
                    break;
                }
            }
        }
        unsigned  int uval = CFitByteDataTransformer::getUIntValue(baseType, fieldData);
        CFitIntField<uint32_t>* f = new CFitIntField<uint32_t>(def, fieldProfile, uval, valid);
        return f;
    }


    static CFitField* buildFloatField(const CFitFieldDefinition& def, CFitFieldProfile* fieldProfile, uint8_t* fieldData)
    {
        CFitBaseType* baseType = def.getBaseType();
        uint8_t* invalidBytes = baseType->invalidValueBytes();
        bool valid = false;
        if(def.getSize() != baseType->size())
        {
            for(uint8_t i = 0; i < def.getSize(); i++)
            {
                if(fieldData[i] != invalidBytes[i])
                {
                    valid =  true;
                    break;
                }
            }
        }
        double dval = CFitByteDataTransformer::getFloatValue(baseType, fieldData);
        CFitFloatField * f = new CFitFloatField(def, fieldProfile, dval, valid);
        return f;
    }


    static CFitField* buildStringField(const CFitFieldDefinition& def, CFitFieldProfile* fieldProfile, uint8_t* fieldData)
    {
        CFitBaseType* baseType = def.getBaseType();
        uint8_t* invalidBytes = baseType->invalidValueBytes();
        QString str = CFitByteDataTransformer::getString(fieldData);
        // all byts set to invalid value
        uint8_t invalidCount = 0;
        for(uint8_t i = 0; i < def.getSize(); i++)
        {
            if(fieldData[i] == invalidBytes[0])
                invalidCount++;
        }
        bool valid = (invalidCount < def.getSize());
        CFitStringField* f = new CFitStringField(def, fieldProfile, str, valid);
        return f;
    }


    static CFitField* buildByteField(const CFitFieldDefinition& def, CFitFieldProfile* fieldProfile, uint8_t* fieldData)
    {
        CFitBaseType* baseType = def.getBaseType();
        uint8_t* invalidBytes = baseType->invalidValueBytes();
        // all byts set to invalid value
        uint8_t invalidCount = 0;
        for(uint8_t i = 0; i < def.getSize(); i++)
        {
            if(fieldData[i] == invalidBytes[0])
                invalidCount++;
        }
        bool valid = (invalidCount < def.getSize());
        CFitByteField* f = new CFitByteField(def, fieldProfile, CFitByteDataTransformer::getBytes(fieldData, def.getSize()), valid);
        return f;
    }


    static void evaluateFieldProfile(CFitMessage* mesg, CFitField* field)
    {
        const CFitFieldProfile* fieldProfile = field->profile();
        // case subfield
        if(fieldProfile->hasSubfields())
        {
            for(CFitSubfieldProfile* subfieldProfile : fieldProfile->getSubfields())
            {
                // the referenced field is for all subfields the same
                for(CFitField* referencedField : mesg->getFields())
                {
                    if(referencedField->getFieldDefNr() == subfieldProfile->getReferencedFieldDefNr() &&
                       referencedField->getUIntValue() == subfieldProfile->getReferencedFieldValue())
                    {
                        // the value of the referenced field matches with the field profile reference-value
                        field->setProfile(subfieldProfile);
                    }
                }
            }
        }
    }

    static void expandComponents(CFitMessage* mesg, const CFitField* field)
    {
        // todo accumulated fields
        const CFitFieldProfile* fieldProfile = field->profile();
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
                    mesg->addField(f);

                }
                else {
                    uint32_t value = (field->getUIntValue() >> offset) & compProfile->getBitmask();
                    CFitIntField<uint32_t> *f = new CFitIntField<uint32_t>(field, compProfile, value, true);
                    mesg->addField(f);
                }
                offset += compProfile->getBits();
            }
        }
    }

    static void swapFieldData(const CFitFieldDefinition& fieldDef, uint8_t* fieldData)
    {
        if (fieldDef.getEndianAbilityFlag() && (fieldDef.parent()->getArchitectureBit() != ArchEndianLittle)) {
            // Swap the bytes for each element.
            int typeSize = fieldDef.getBaseType()->size();
            int elements = fieldDef.getSize() / typeSize;

            for (int element = 0; element < elements; element++) {
                for (int i = 0; i < (typeSize / 2); i++) {
                    uint8_t tmp = (uint8_t)fieldData[element * typeSize + i];
                    fieldData[element * typeSize + i] = fieldData[element * typeSize + typeSize - i - 1];
                    fieldData[element * typeSize + typeSize - i - 1] = tmp;
                }
            }
        }
    }
};


void CFitFieldDataState::reset()
{
    fieldDataIndex = 0;
    fieldIndex = 0;
}


DecodeState CFitFieldDataState::process(uint8_t &dataByte) {
    CFitMessage* mesg = latestMessage();
    CFitFieldDefinition& fieldDef = defintion(mesg->getLocalMesgNr())->getFieldByIndex(fieldIndex);

    // add the read byte to the data array
    fieldData[fieldDataIndex++] = dataByte;

    if (fieldDataIndex >= fieldDef.getSize()) {
        // all bytes are read for current field

        // new field with data
        //const CFitField& field = CFitFieldBuilder::buildField(fieldDef, fieldData, mesg);
        // Ignore field of unknown base type.
        CFitField* f = CFitFieldBuilder::buildField(fieldDef, fieldData, mesg);
        mesg->addField(f);

        // The special case time record.
        // timestamp has always the same value for all enums. it does not matter againts which we're comparing.
        if (fieldDef.getDefNr() == RecordTimestamp) {
            setTimestamp(f->getUIntValue());
        }

        // new field follows, reset
        fieldDataIndex = 0;
        fieldIndex++;
    }

    if (fieldIndex >= defintion(mesg->getLocalMesgNr())->getNrOfFields()) {
        // Now that the entire message is decoded we may evaluate subfields and expand components
        CFitFieldBuilder::evaluateSubfieldsAndExpandComponents(mesg);

        reset();
        // TODO macro
        // qDebug() << latestMessage()->messageInfo();
        // after all fields read, go to next record header
        return StateRecord;
    }

    // there are more fields to read for the current message
    return StateFieldData;
};
