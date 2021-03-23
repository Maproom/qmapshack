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

#include "gis/fit/defs/CFitBaseType.h"

const CFitBaseType fit::EnumType = CFitBaseType(eBaseTypeNrEnum, "Enum", sizeof(quint8), {0xFF});
const CFitBaseType fit::Sint8Type = CFitBaseType(eBaseTypeNrSint8, "Sint8", sizeof(qint8), {0x7F});
const CFitBaseType fit::Uint8Type = CFitBaseType(eBaseTypeNrUint8, "Uint8", sizeof(quint8), {0xFF});
const CFitBaseType fit::Sint16Type = CFitBaseType(eBaseTypeNrSint16, "Sint16", sizeof(qint16), {0x7F, 0xFF});
const CFitBaseType fit::Uint16Type = CFitBaseType(eBaseTypeNrUint16, "Uint16", sizeof(quint16), {0xFF, 0xFF});
const CFitBaseType fit::Sint32Type = CFitBaseType(eBaseTypeNrSint32, "Sint32", sizeof(qint32), {0x7F, 0xFF, 0xFF, 0xFF});
const CFitBaseType fit::Uint32Type = CFitBaseType(eBaseTypeNrUint32, "Uint32", sizeof(quint32), {0xFF, 0xFF, 0xFF, 0xFF});
const CFitBaseType fit::Sint64Type = CFitBaseType(eBaseTypeNrSint64, "Sint64", sizeof(qint64), {0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
const CFitBaseType fit::Uint64Type = CFitBaseType(eBaseTypeNrUint64, "Uint64", sizeof(quint64), {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
const CFitBaseType fit::StringType = CFitBaseType(eBaseTypeNrString, "String", 0, {0x00}); // Field is invalid if all bytes are invalid.
const CFitBaseType fit::Float32Type = CFitBaseType(eBaseTypeNrFloat32, "Float32", sizeof(float), {0xFF, 0xFF, 0xFF, 0xFF});
const CFitBaseType fit::Float64Type = CFitBaseType(eBaseTypeNrFloat64, "Float64", sizeof(double), {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
const CFitBaseType fit::Uint8zType = CFitBaseType(eBaseTypeNrUint8z, "Uint8z", sizeof(quint8), {0x00});
const CFitBaseType fit::Uint16zType = CFitBaseType(eBaseTypeNrUint16z, "Uint16z", sizeof(quint16), {0x00, 0x00});
const CFitBaseType fit::Uint32zType = CFitBaseType(eBaseTypeNrUint32z, "Uint32z", sizeof(quint32), {0x00, 0x00, 0x00, 0x00});
const CFitBaseType fit::Uint64zType = CFitBaseType(eBaseTypeNrUint64z, "Uint64z", sizeof(quint64), {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
const CFitBaseType fit::ByteType = CFitBaseType(eBaseTypeNrByte, "Byte", 0, {0xFF}); // Field is invalid if all bytes are invalid.
const CFitBaseType fit::InvalidType = CFitBaseType(eBaseTypeNrInvalid, "Invalid", 0, {0});



CFitBaseType::CFitBaseType() : CFitBaseType(eBaseTypeNrInvalid, "invalid", 0,
{
    0
})
{
}

CFitBaseType::CFitBaseType(fit_base_type_nr_e baseTypeNr, QString name, quint8 size, std::initializer_list<quint8> invalid)
    : typeSize(size), baseTypeNr(baseTypeNr), namestr(name)
{
    if(size == 0)
    {
        return;
    }

    quint8 i = (quint8) (size - 1);
    for(quint8 bit : invalid)
    {
        invalidBytes[i--] = bit;
    }
}

quint8 CFitBaseType::size() const
{
    return typeSize;
}
bool CFitBaseType::isSizeUndefined() const
{
    return typeSize == 0;
}

fit_base_type_nr_e CFitBaseType::nr() const
{
    return baseTypeNr;
}

quint8 CFitBaseType::baseTypeField() const
{
    if(typeSize > 1)
    {
        return (quint8) (0x80 | baseTypeNr);
    }
    return baseTypeNr;
}

const quint8* CFitBaseType::invalidValueBytes() const
{
    return invalidBytes;
}

bool CFitBaseType::isInteger() const
{
    return isSignedInt() || isUnsignedInt();
}

bool CFitBaseType::isSignedInt() const
{
    return baseTypeNr == eBaseTypeNrSint8 || baseTypeNr == eBaseTypeNrSint16
           || baseTypeNr == eBaseTypeNrSint32 || baseTypeNr == eBaseTypeNrSint64;
}

bool CFitBaseType::isUnsignedInt() const
{
    return baseTypeNr == eBaseTypeNrUint8 || baseTypeNr == eBaseTypeNrUint8z
           || baseTypeNr == eBaseTypeNrUint16 || baseTypeNr == eBaseTypeNrUint16z
           || baseTypeNr == eBaseTypeNrUint32 || baseTypeNr == eBaseTypeNrUint32z
           || baseTypeNr == eBaseTypeNrUint64 || baseTypeNr == eBaseTypeNrUint64z
           || baseTypeNr == eBaseTypeNrEnum;
}

bool CFitBaseType::isFloat() const
{
    return baseTypeNr == eBaseTypeNrFloat32 || baseTypeNr == eBaseTypeNrFloat64;
}

bool CFitBaseType::isNumber() const
{
    return isInteger() || isFloat();
}


bool CFitBaseType::isString() const
{
    return baseTypeNr == eBaseTypeNrString;
}

bool CFitBaseType::isByte() const
{
    return baseTypeNr == eBaseTypeNrByte;
}

QString CFitBaseType::name() const
{
    return namestr;
}


void CFitBaseTypeMap::initialize(QMap<quint8, CFitBaseType>& baseTypesMap)
{
    baseTypesMap.insert(fit::EnumType.nr(), fit::EnumType);
    baseTypesMap.insert(fit::Sint8Type.nr(), fit::Sint8Type);
    baseTypesMap.insert(fit::Uint8Type.nr(), fit::Uint8Type);
    baseTypesMap.insert(fit::Sint16Type.nr(), fit::Sint16Type);
    baseTypesMap.insert(fit::Uint16Type.nr(), fit::Uint16Type);
    baseTypesMap.insert(fit::Sint32Type.nr(), fit::Sint32Type);
    baseTypesMap.insert(fit::Uint32Type.nr(), fit::Uint32Type);
    baseTypesMap.insert(fit::Sint64Type.nr(), fit::Sint64Type);
    baseTypesMap.insert(fit::Uint64Type.nr(), fit::Uint64Type);
    baseTypesMap.insert(fit::StringType.nr(), fit::StringType);
    baseTypesMap.insert(fit::Float32Type.nr(), fit::Float32Type);
    baseTypesMap.insert(fit::Float64Type.nr(), fit::Float64Type);
    baseTypesMap.insert(fit::Uint8zType.nr(), fit::Uint8zType);
    baseTypesMap.insert(fit::Uint16zType.nr(), fit::Uint16zType);
    baseTypesMap.insert(fit::Uint32zType.nr(), fit::Uint32zType);
    baseTypesMap.insert(fit::Uint64zType.nr(), fit::Uint64zType);
    baseTypesMap.insert(fit::ByteType.nr(), fit::ByteType);
}

CFitBaseType* CFitBaseTypeMap::get(quint8 nr)
{
    static bool initialized = false;
    static QMap<quint8, CFitBaseType> baseTypes;
    if(!initialized)
    {
        initialize(baseTypes);
        initialized = true;
    }

    if (baseTypes.contains(nr & fitBaseTypeNumMask))
    {
        return &baseTypes[nr & fitBaseTypeNumMask];
    }
    return &baseTypes[eBaseTypeNrInvalid];
}
