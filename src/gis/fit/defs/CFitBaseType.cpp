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

CFitBaseType::CFitBaseType() : CFitBaseType(TypeInvalid, 0, 0, "invalid")
{}


CFitBaseType::CFitBaseType(BaseTypeNr baseTypeNr, uint8_t* invalidBytes, uint8_t size, QString name)
: typeSize(size), baseTypeNr(baseTypeNr), namestr(name)
{
    this->invalidBytes = new uint8_t[size];
    memcpy(this->invalidBytes, invalidBytes, size);
}

CFitBaseType::~CFitBaseType()
{
    // delete of the invalidBytes not necessary as it is only destroyed on app exit
}

uint8_t CFitBaseType::size() const
{
    return typeSize;
}

BaseTypeNr  CFitBaseType::nr() const
{
    return baseTypeNr;
}

uint8_t* CFitBaseType::invalidValueBytes() const
{
    return invalidBytes;
}

bool CFitBaseType::isInteger() const
{
    return isSignedInt() || isUnsignedInt();
}

bool CFitBaseType::isSignedInt() const
{
    return baseTypeNr == TypeSint8 || baseTypeNr == TypeSint16 || baseTypeNr == TypeSint32;
}

bool CFitBaseType::isUnsignedInt() const
{
    return baseTypeNr == TypeUint8 || baseTypeNr == TypeUint8z
           || baseTypeNr == TypeUint16 || baseTypeNr == TypeUint16z
           || baseTypeNr == TypeUint32 || baseTypeNr == TypeUint32z
           || baseTypeNr == TypeEnum;
}

bool CFitBaseType::isFloat() const
{
    return baseTypeNr == TypeFloat32 || baseTypeNr == TypeFloat64;
}

bool CFitBaseType::isNumber() const
{
    return isInteger() ||isFloat();
}


bool CFitBaseType::isString() const
{
    return baseTypeNr == TypeString;
}

bool CFitBaseType::isByte() const
{
    return baseTypeNr == TypeByte;
}

QString CFitBaseType::name() const
{
     return namestr;
}

template <class T>
CFitBaseType build(BaseTypeNr baseTypeNr, QString name, T invalid)
{
    //uint8_t* invalidBytes = (uint8_t*)&invalid;
    CFitBaseType t(baseTypeNr, (uint8_t*)&invalid, (uint8_t)sizeof(T), name);
    return t;
}

CFitBaseType EnumType = build<uint8_t>(TypeEnum, "Enum", 0xFF);
CFitBaseType Sint8Type = build<int8_t>(TypeSint8, "Sint8", 0x7F);
CFitBaseType Uint8Type = build<int8_t>(TypeUint8, "Uint8", 0xFF);
CFitBaseType Sint16Type = build<int16_t>(TypeSint16, "Sint16", 0x7FFF);
CFitBaseType Uint16Type = build<uint16_t>(TypeUint16, "Uint16", 0xFFFF);
CFitBaseType Sint32Type = build<int32_t>(TypeSint32, "Sint32", 0x7FFFFFFF);
CFitBaseType Uint32Type = build<uint32_t>(TypeUint32, "Uint32", 0xFFFFFFFF);
CFitBaseType StringType = build<char>(TypeString, "String", 0x00);
CFitBaseType Float32Type = build<float>(TypeFloat32, "Flaot32", (float)0xFFFFFFFF);
CFitBaseType Float64Type = build<double>(TypeFloat64, "Float64", (double)0xFFFFFFFFFFFFFFFFull);

CFitBaseType Uint8zType = build<uint8_t>(TypeUint8z, "Uint8z", 0x00);
CFitBaseType Uint16zType = build<uint16_t>(TypeUint16z, "Uint16z", 0x0000);
CFitBaseType Uint32zType = build<uint32_t>(TypeUint32z, "Uint32z", 0x00000000);
CFitBaseType ByteType = build<uint8_t>(TypeByte, "Byte", 0xFF); // Field is invalid if all bytes are invalid.

CFitBaseType InvalidType = build<uint8_t>(TypeInvalid, "Invalid", 0);

static QMap<uint8_t, CFitBaseType> buildBaseTypeMap()
{
    QMap<uint8_t, CFitBaseType> baseTypes;
    baseTypes.insert(EnumType.nr(), EnumType);
    baseTypes.insert(Sint8Type.nr(), Sint8Type);
    baseTypes.insert(Uint8Type.nr(), Uint8Type);
    baseTypes.insert(Sint16Type.nr(), Sint16Type);
    baseTypes.insert(Uint16Type.nr(), Uint16Type);
    baseTypes.insert(Sint32Type.nr(), Sint32Type);
    baseTypes.insert(Uint32Type.nr(), Uint32Type);
    baseTypes.insert(StringType.nr(), StringType);
    baseTypes.insert(Float32Type.nr(), Float32Type);
    baseTypes.insert(Float64Type.nr(), Float64Type);
    baseTypes.insert(Uint8zType.nr(), Uint8zType);
    baseTypes.insert(Uint16zType.nr(), Uint16zType);
    baseTypes.insert(Uint32zType.nr(), Uint32zType);
    baseTypes.insert(ByteType.nr(), ByteType);
    return baseTypes;
};

QMap<uint8_t, CFitBaseType> CFitBaseTypeMap::baseTypes = buildBaseTypeMap();


CFitBaseType* CFitBaseTypeMap::get(uint8_t nr)
{
    if (baseTypes.contains(nr & BaseTypeNumMask))
        return &baseTypes[nr & BaseTypeNumMask];

    return &InvalidType;
}