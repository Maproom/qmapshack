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

#include <gis/fit/defs/CFitProfileLockup.h>
#include "gis/fit/decoder/CFitFieldDataState.h"
#include "gis/fit/defs/fit_const.h"
#include "gis/fit/defs/fit_fields.h"


class CFitDataTransformer
{
public:
    static CFitFieldProfile* evaluateFieldProfile(CFitFieldDefinition* defintion, CFitMessage* message)
    {
        CFitProfile* profile = CFitProfileLockup::getProfile(message->getGlobalMesgNr());
        CFitFieldProfile* fieldProfile = profile->getField(defintion->getDefNr());

        if(fieldProfile->hasSubfields())
        {
            for(CFitFieldProfile* subfieldProfile : fieldProfile->getSubfields())
            {
                // actually the referenced field is for all subfields the same
                CFitField* referencedField = message->getField(subfieldProfile->getFieldDefNum());
                if(referencedField->getUIntValue() == subfieldProfile->getReferencedFieldValue())
                {
                    return subfieldProfile;
                }
            }
        }
        return fieldProfile;
    }

    static CFitField* buildField(CFitFieldDefinition* defintion, uint8_t* fieldData)
    {
        swapFieldData(defintion, fieldData);
        CFitBaseType* baseType = defintion->getBaseType();
        bool valid = isValidValue(defintion, fieldData);
        if(baseType->isSignedInt())
        {
            int ival = CFitDataTransformer::getIntValue(baseType, fieldData);
            CFitIntField<int32_t>* f = new CFitIntField<int32_t>(defintion, ival, valid);
            return f;
        }
        else if(baseType->isUnsignedInt())
        {
            unsigned  int uval = CFitDataTransformer::getUIntValue(baseType, fieldData);
            CFitIntField<uint32_t>* f = new CFitIntField<uint32_t>(defintion, uval, valid);
            return f;
        }
        else if(baseType->isFloat())
        {
            double dval = CFitDataTransformer::getDoubleValue(baseType, fieldData);
            CFitDoubleField* f = new CFitDoubleField(defintion, dval, valid);
            return f;
        }
        else if(baseType->nr() == TypeString)
        {
            QString str = CFitDataTransformer::getString(fieldData);
            CFitStringField* f = new CFitStringField(defintion, str, valid);
            return f;
        }
        else if(baseType->nr() == TypeByte)
        {
            CFitByteField* f = new CFitByteField(defintion, (uint8_t*)fieldData, valid);
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

    static bool isValidValue(CFitFieldDefinition* fieldDefinition, uint8_t* rawData)
    {
        CFitBaseType* baseType = fieldDefinition->getBaseType();
        char* invalidBytes = baseType->invalidValueBytes();
        if (baseType->nr() == TypeString || baseType->nr() == TypeByte)
        {
            // all byts set to invalid value
            uint8_t invalidCount = 0;
            for(uint8_t i = 0; i < fieldDefinition->getSize(); i++)
            {
                // string and enum specify one byte for invalid value
                if((rawData[i] >> (i * 8)) == invalidBytes[0])
                    invalidCount++;
            }
            return (invalidCount == fieldDefinition->getSize());
        }
        else if (baseType->isNumber())
        {
            if(fieldDefinition->getSize() != baseType->size())
                return false;

            for(uint8_t i = 0; i < fieldDefinition->getSize(); i++)
            {
                if((rawData[i] >> (i * 8)) != invalidBytes[i])
                    return true;
            }
            return false;
        }
        // invalid type
        return false;
    }


    static unsigned int getUIntValue(CFitBaseType* baseType, uint8_t* rawData)
    {
        switch(baseType->nr()) {
            case TypeUint8:
            case TypeUint8z:
            case TypeEnum:
                return getUint8(rawData);
            case TypeUint16:
            case TypeUint16z:
                return getUint16(rawData);
            case TypeUint32:
            case TypeUint32z:
                return getUint32(rawData);
        }
    }

    static int getIntValue(CFitBaseType* baseType, uint8_t* rawData)
    {
        switch(baseType->nr()) {
            case TypeSint8:
                return getSint8(rawData);
            case TypeSint16:
                return getSint16(rawData);
            case TypeSint32:
                return getSint32(rawData);
        }
    }

    static double getDoubleValue(CFitBaseType* baseType, uint8_t* rawData)
    {
        switch(baseType->nr()) {
            case TypeFloat32:
                return getFloat32(rawData);
            case TypeFloat64:
                return getFloat64(rawData);
        }
    }

    static uint8_t getUint8(uint8_t* rawData)
    {
        return (uint8_t) rawData[0];
    }
    static uint16_t getUint16(uint8_t* rawData)
    {
        return ((uint16_t)rawData[1] << 8) | (uint16_t)rawData[0];
    }
    static uint32_t getUint32(uint8_t* rawData)
    {
        return (((uint32_t)rawData[3] << 24) | ((uint32_t)rawData[2] << 16) | ((uint32_t)rawData[1] << 8) | (uint32_t)rawData[0]);
    }
    static int8_t getSint8(uint8_t* rawData)
    {
        return (int8_t) rawData[0];
    }
    static int16_t getSint16(uint8_t* rawData)
    {
        return ((int16_t)rawData[1] << 8) | (int16_t)rawData[0];
    }
    static int32_t getSint32(uint8_t* rawData)
    {
        return (((int32_t)rawData[3] << 24) | ((int32_t)rawData[2] << 16) | ((int32_t)rawData[1] << 8) | (int32_t)rawData[0]);
    }
    static float getFloat32(uint8_t* rawData)
    {
        int32_t fValue = (int32_t) (((int32_t)rawData[3] << 24) | ((int32_t)rawData[2] << 16) | ((int32_t)rawData[1] << 8) | rawData[0]);
        float value;
        memcpy(&value, &fValue, sizeof(float));
        return value;
    }
    static double getFloat64(uint8_t* rawData)
    {
        unsigned long long dValue = ((unsigned long long) rawData[7] << 56) | ((unsigned long long) rawData[6] << 48)
                                    | ((unsigned long long) rawData[5] << 40) | ((unsigned long long) rawData[4] << 32)
                                    | ((unsigned long long) rawData[3] << 24) | ((unsigned long long) rawData[2] << 16)
                                    | ((unsigned long long) rawData[1] << 8) | rawData[0];
        double value;
        memcpy(&value, &dValue, sizeof(double));
        return value;
    }

    static QString getString(uint8_t* rawData)
    {
        QString str;
        uint8_t i = 0;
        while(rawData[i] != 0)
        {
            str = str.append(rawData[i]);
            i++;
        }
        return str;
    }
    uint8_t* getBytes(uint8_t* rawData)
    {
        // TODO
        return nullptr;
    }
};


void CFitFieldDataState::reset() {
    fieldDataIndex = 0;
    fieldIndex = 0;
}

CFitFieldDataState::expand()
{
  if(fieldDef->profile()->hasComponents())
  {
      uint32_t bitPos = 0;
      for(CFitFieldProfile& comp :  fieldDef->profile()->getComponents())
      {
          (fieldData >> bitPos) & comp.getBitmask();
          bitPos += comp.getBits();
      }
  }


}

DecodeState CFitFieldDataState::process(uint8_t &dataByte) {
    CFitFieldDefinition *fieldDef = defintion(latestMessage()->getLocalMesgNr())->getFieldByIndex(fieldIndex);

    // add the read byte to the data array
    fieldData[fieldDataIndex++] = dataByte;

    if (fieldDataIndex >= fieldDef->getSize()) {
        // all bytes are read for current field

        // new field with data
        CFitField* field = CFitDataTransformer::buildField(fieldDef, fieldData);
        // TODO case components

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
        // TODO Now that the entire message is decoded we may evaluate subfields and expand components
        reset();
        // TODO macro
        // qDebug() << latestMessage()->messageInfo();
        // after all fields read, go to next record header
        return StateRecord;
    }

    // there are more fields to read for the current message
    return StateFieldData;
};


