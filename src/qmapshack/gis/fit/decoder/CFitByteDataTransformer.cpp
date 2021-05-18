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

#include "gis/fit/decoder/CFitByteDataTransformer.h"
#include "gis/fit/decoder/CFitDefinitionMessage.h"
#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/defs/fit_const.h"

quint64 CFitByteDataTransformer::getUIntValue(const CFitBaseType& baseType, quint8* rawData)
{
    switch(baseType.nr())
    {
    case eBaseTypeNrUint8:
    case eBaseTypeNrUint8z:
    case eBaseTypeNrEnum:
        return getUint8(rawData);

    case eBaseTypeNrUint16:
    case eBaseTypeNrUint16z:
        return getUint16(rawData);

    case eBaseTypeNrUint32:
    case eBaseTypeNrUint32z:
        return getUint32(rawData);

    case eBaseTypeNrUint64:
    case eBaseTypeNrUint64z:
        return getUint64(rawData);

    default:
        return 0;
    }
}

qint64 CFitByteDataTransformer::getSIntValue(const CFitBaseType& baseType, quint8* rawData)
{
    switch(baseType.nr())
    {
    case eBaseTypeNrSint8:
        return getSint8(rawData);

    case eBaseTypeNrSint16:
        return getSint16(rawData);

    case eBaseTypeNrSint32:
        return getSint32(rawData);

    case eBaseTypeNrSint64:
        return getSint64(rawData);

    default:
        return 0;
    }
}

qreal CFitByteDataTransformer::getFloatValue(const CFitBaseType& baseType, quint8* rawData)
{
    switch(baseType.nr())
    {
    case eBaseTypeNrFloat32:
        return getFloat32(rawData);

    case eBaseTypeNrFloat64:
        return getFloat64(rawData);

    default:
        return 0;
    }
}

quint8 CFitByteDataTransformer::getUint8(quint8* rawData)
{
    return (quint8) rawData[0];
}

quint16 CFitByteDataTransformer::getUint16(quint8* rawData)
{
    return ((quint16)rawData[1] << 8) | (quint16)rawData[0];
}

quint32 CFitByteDataTransformer::getUint32(quint8* rawData)
{
    return ((quint32)rawData[3] << 24) | ((quint32)rawData[2] << 16) | ((quint32)rawData[1] << 8) | (quint32)rawData[0];
}

quint64 CFitByteDataTransformer::getUint64(quint8* rawData)
{
    return ((quint64) rawData[7] << 56) | ((quint64) rawData[6] << 48)
           | ((quint64) rawData[5] << 40) | ((quint64) rawData[4] << 32)
           | ((quint64) rawData[3] << 24) | ((quint64) rawData[2] << 16)
           | ((quint64) rawData[1] << 8) | rawData[0];
}

qint8 CFitByteDataTransformer::getSint8(quint8* rawData)
{
    return (qint8) rawData[0];
}

qint16 CFitByteDataTransformer::getSint16(quint8* rawData)
{
    return ((qint16)rawData[1] << 8) | (qint16)rawData[0];
}

qint32 CFitByteDataTransformer::getSint32(quint8* rawData)
{
    return ((qint32)rawData[3] << 24) | ((qint32)rawData[2] << 16) | ((qint32)rawData[1] << 8) | (qint32)rawData[0];
}


qint64 CFitByteDataTransformer::getSint64(quint8* rawData)
{
    return ((qint64) rawData[7] << 56) | ((qint64) rawData[6] << 48)
           | ((qint64) rawData[5] << 40) | ((qint64) rawData[4] << 32)
           | ((qint64) rawData[3] << 24) | ((qint64) rawData[2] << 16)
           | ((qint64) rawData[1] << 8) | rawData[0];
}

qreal CFitByteDataTransformer::getFloat32(quint8* rawData)
{
    qint32 fValue = (qint32) (((qint32)rawData[3] << 24) | ((qint32)rawData[2] << 16) | ((qint32)rawData[1] << 8) | rawData[0]);
    // comment: qreal is a double type (on almost all systems). Here we need to go through a 32 bit floating type.
    float tmp;
    memcpy(&tmp, &fValue, sizeof(tmp));
    qreal value = tmp;
    return value;
}

qreal CFitByteDataTransformer::getFloat64(quint8* rawData)
{
    unsigned long long dValue = ((unsigned long long) rawData[7] << 56) | ((unsigned long long) rawData[6] << 48)
                                | ((unsigned long long) rawData[5] << 40) | ((unsigned long long) rawData[4] << 32)
                                | ((unsigned long long) rawData[3] << 24) | ((unsigned long long) rawData[2] << 16)
                                | ((unsigned long long) rawData[1] << 8) | rawData[0];
    // comment: qreal is a double type (on almost all systems). Here we need to go through a 64 bit floating type.
    double tmp;
    memcpy(&tmp, &dValue, sizeof(tmp));
    qreal value = tmp;
    return value;
}

QString CFitByteDataTransformer::getString(quint8* rawData, quint8 length)
{
    // find the 0 termination
    quint8 i = 0;
    while(rawData[i] != 0 )
    {
        i++;
        // no 0 termination found, but length exceeded
        if(i > length)
        {
            break;
        }
    }

    return QString::fromUtf8((const char*)rawData, i);
}

QByteArray CFitByteDataTransformer::getBytes(quint8* rawData, quint8 length)
{
    return QByteArray((const char*)rawData, length);
}

void CFitByteDataTransformer::swapFieldData(const CFitFieldDefinition& fieldDef, quint8* fieldData)
{
    if (fieldDef.getEndianAbilityFlag() && (fieldDef.parent().getArchitectureBit() != eFitArchEndianLittle))
    {
        // Swap the bytes for each element.
        int typeSize = fieldDef.getBaseType().size();
        int elements = fieldDef.getSize() / typeSize;

        for (int element = 0; element < elements; element++)
        {
            for (int i = 0; i < (typeSize / 2); i++)
            {
                quint8 tmp = (quint8)fieldData[element * typeSize + i];
                fieldData[element * typeSize + i] = fieldData[element * typeSize + typeSize - i - 1];
                fieldData[element * typeSize + typeSize - i - 1] = tmp;
            }
        }
    }
}
