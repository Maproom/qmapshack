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

#include "gis/fit/defs/CFitBaseType.h"

CFitBaseType::CFitBaseType() : CFitBaseType(eBaseTypeNrInvalid, "invalid", 0, {0})
{
}

CFitBaseType::CFitBaseType(fit_base_type_nr_e baseTypeNr, QString name, quint8 size, std::initializer_list<quint8> invalid)
        : typeSize(size), baseTypeNr(baseTypeNr), namestr(name)
{
    quint8 i = 0;
    for(quint8 bit : invalid)
    {
        invalidBytes[i++] = bit;
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
    return baseTypeNr == eBaseTypeNrSint8 || baseTypeNr == eBaseTypeNrSint16 || baseTypeNr == eBaseTypeNrSint32;
}

bool CFitBaseType::isUnsignedInt() const
{
    return baseTypeNr == eBaseTypeNrUint8 || baseTypeNr == eBaseTypeNrUint8z
           || baseTypeNr == eBaseTypeNrUint16 || baseTypeNr == eBaseTypeNrUint16z
           || baseTypeNr == eBaseTypeNrUint32 || baseTypeNr == eBaseTypeNrUint32z
           || baseTypeNr == eBaseTypeNrEnum;
}

bool CFitBaseType::isFloat() const
{
    return baseTypeNr == eBaseTypeNrFloat32 || baseTypeNr == eBaseTypeNrFloat64;
}

bool CFitBaseType::isNumber() const
{
    return isInteger() ||isFloat();
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


/*
const EnumType = CFitBaseType(TypeEnum, "Enum", sizeof(quint8), {0xFF});
Sint8Type = CFitBaseType(TypeSint8, "Sint8", sizeof(qint8), {0x7F});
Uint8Type = CFitBaseType(TypeUint8, "Uint8", sizeof(quint8),  {0xFF});
Sint16Type = CFitBaseType(TypeSint16, "Sint16", sizeof(qint16), {0x7F, 0xFF});
Uint16Type = CFitBaseType(TypeUint16, "Uint16",sizeof(quint16), {0xFF,0xFF});
Sint32Type = CFitBaseType(TypeSint32, "Sint32", sizeof(qint32),{0x7F,0xFF,0xFF,0xFF});
Uint32Type = CFitBaseType(TypeUint32, "Uint32", sizeof(quint32),{0xFF,0xFF,0xFF,0xFF});
StringType = CFitBaseType(TypeString, "String", 0,{0x00});
Float32Type = CFitBaseType(TypeFloat32, "Flaot32", sizeof(float), {0xFF,0xFF,0xFF,0xFF});
Float64Type = CFitBaseType(TypeFloat64, "Float64", sizeof(double), {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF});

Uint8zType = CFitBaseType(TypeUint8z, "Uint8z", sizeof(quint8), {0x00});
Uint16zType = CFitBaseType(TypeUint16z, "Uint16z", sizeof(quint16), {0x00,0x00});
Uint32zType = CFitBaseType(TypeUint32z, "Uint32z", sizeof(quint32), {0x00,0x00,0x00,0x00});
ByteType = CFitBaseType(TypeByte, "Byte", 0, {0xFF}); // Field is invalid if all bytes are invalid.

InvalidType = CFitBaseType(TypeInvalid, "Invalid",0, {0});
*/

void CFitBaseTypeMap::initialize(QMap<quint8, CFitBaseType>& baseTypesMap)
{
    baseTypesMap.insert(EnumType.nr(), EnumType);
    baseTypesMap.insert(Sint8Type.nr(), Sint8Type);
    baseTypesMap.insert(Uint8Type.nr(), Uint8Type);
    baseTypesMap.insert(Sint16Type.nr(), Sint16Type);
    baseTypesMap.insert(Uint16Type.nr(), Uint16Type);
    baseTypesMap.insert(Sint32Type.nr(), Sint32Type);
    baseTypesMap.insert(Uint32Type.nr(), Uint32Type);
    baseTypesMap.insert(StringType.nr(), StringType);
    baseTypesMap.insert(Float32Type.nr(), Float32Type);
    baseTypesMap.insert(Float64Type.nr(), Float64Type);
    baseTypesMap.insert(Uint8zType.nr(), Uint8zType);
    baseTypesMap.insert(Uint16zType.nr(), Uint16zType);
    baseTypesMap.insert(Uint32zType.nr(), Uint32zType);
    baseTypesMap.insert(ByteType.nr(), ByteType);
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
