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
#include "gis/fit/decoder/CFitDefinitionMessage.h"

unsigned int CFitByteDataTransformer::getUIntValue(const CFitBaseType& baseType, uint8_t* rawData)
{
    switch(baseType.nr()) {
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
        default:
            return 0;
    }
}

int CFitByteDataTransformer::getSIntValue(const CFitBaseType& baseType, uint8_t *rawData)
{
    switch(baseType.nr()) {
        case TypeSint8:
            return getSint8(rawData);
        case TypeSint16:
            return getSint16(rawData);
        case TypeSint32:
            return getSint32(rawData);
        default:
            return 0;
    }
}

double CFitByteDataTransformer::getFloatValue(const CFitBaseType& baseType, uint8_t* rawData)
{
    switch(baseType.nr()) {
        case TypeFloat32:
            return getFloat32(rawData);
        case TypeFloat64:
            return getFloat64(rawData);
        default:
            return 0;
    }
}

uint8_t CFitByteDataTransformer::getUint8(uint8_t* rawData)
{
    return (uint8_t) rawData[0];
}

uint16_t CFitByteDataTransformer::getUint16(uint8_t* rawData)
{
    return ((uint16_t)rawData[1] << 8) | (uint16_t)rawData[0];
}

uint32_t CFitByteDataTransformer::getUint32(uint8_t* rawData)
{
    return (((uint32_t)rawData[3] << 24) | ((uint32_t)rawData[2] << 16) | ((uint32_t)rawData[1] << 8) | (uint32_t)rawData[0]);
}

int8_t CFitByteDataTransformer::getSint8(uint8_t* rawData)
{
    return (int8_t) rawData[0];
}

int16_t CFitByteDataTransformer::getSint16(uint8_t* rawData)
{
    return ((int16_t)rawData[1] << 8) | (int16_t)rawData[0];
}

int32_t CFitByteDataTransformer::getSint32(uint8_t* rawData)
{
    return (((int32_t)rawData[3] << 24) | ((int32_t)rawData[2] << 16) | ((int32_t)rawData[1] << 8) | (int32_t)rawData[0]);
}

float CFitByteDataTransformer::getFloat32(uint8_t* rawData)
{
    int32_t fValue = (int32_t) (((int32_t)rawData[3] << 24) | ((int32_t)rawData[2] << 16) | ((int32_t)rawData[1] << 8) | rawData[0]);
    float value;
    memcpy(&value, &fValue, sizeof(float));
    return value;
}

double CFitByteDataTransformer::getFloat64(uint8_t* rawData)
{
    unsigned long long dValue = ((unsigned long long) rawData[7] << 56) | ((unsigned long long) rawData[6] << 48)
                                | ((unsigned long long) rawData[5] << 40) | ((unsigned long long) rawData[4] << 32)
                                | ((unsigned long long) rawData[3] << 24) | ((unsigned long long) rawData[2] << 16)
                                | ((unsigned long long) rawData[1] << 8) | rawData[0];
    double value;
    memcpy(&value, &dValue, sizeof(double));
    return value;
}

QString CFitByteDataTransformer::getString(uint8_t* rawData, uint8_t length)
{
    // find the 0 termination
    uint8_t i = 0;
    while(rawData[i] != 0 )
    {
        i++;
        // no 0 termination found, but length exceded
        if(i > length)
            break;
    }

    return QString::fromUtf8((const char*)rawData, i);
}

QByteArray CFitByteDataTransformer::getBytes(uint8_t* rawData, uint8_t length)
{
    return QByteArray((const char*)rawData, length);
}

void CFitByteDataTransformer::swapFieldData(const CFitFieldDefinition& fieldDef, uint8_t* fieldData)
{
    if (fieldDef.getEndianAbilityFlag() && (fieldDef.parent().getArchitectureBit() != ArchEndianLittle)) {
        // Swap the bytes for each element.
        int typeSize = fieldDef.getBaseType().size();
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