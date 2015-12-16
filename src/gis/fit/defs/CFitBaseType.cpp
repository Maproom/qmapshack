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

#include "gis/fit/defs/CFitBaseType.h"

CFitBaseType::CFitBaseType() : CFitBaseType(TypeInvalid, 0, 0, "invalid")
{}

CFitBaseType::CFitBaseType(const CFitBaseType& copy)
: invalidBytes(copy.invalidBytes), baseTypeNr(copy.baseTypeNr),typeSize(copy.typeSize), name(copy.name)
{}

CFitBaseType::CFitBaseType(BaseTypeNr baseTypeNr, char* invalidBytes, uint8_t size, QString name)
: invalidBytes(invalidBytes), baseTypeNr(baseTypeNr), typeSize(size), name(name){ }

uint8_t CFitBaseType::size() { return typeSize; }

BaseTypeNr  CFitBaseType::nr() { return baseTypeNr; }


char* CFitBaseType::invalidValueBytes()
{
    return invalidBytes;
}

bool CFitBaseType::isInteger()
{
    return isSignedInt() || isUnsignedInt();
}

bool CFitBaseType::isSignedInt()
{
    return baseTypeNr == TypeSint8 || baseTypeNr == TypeSint16 || baseTypeNr == TypeSint32;
}

bool CFitBaseType::isUnsignedInt()
{
    return baseTypeNr == TypeUint8 || baseTypeNr == TypeUint8z
           || baseTypeNr == TypeUint16 || baseTypeNr == TypeUint16z
           || baseTypeNr == TypeUint32 || baseTypeNr == TypeUint32z
           || baseTypeNr == TypeEnum ;
}

bool CFitBaseType::isFloat()
{
    return baseTypeNr == TypeFloat32 || baseTypeNr == TypeFloat64;
}

bool CFitBaseType::isNumber()
{
    return isInteger() ||isFloat();
}

QString CFitBaseType::str()
{
     return name;
}

template <class T>
CFitBaseType build(T invalid, BaseTypeNr baseTypeNr, QString name)
{
    char* invalidBytes = (char*)&invalid;
    CFitBaseType t(baseTypeNr, invalidBytes, sizeof(T), name);
    return t;
}

CFitBaseType EnumType = build<uint8_t>(0xFF, TypeEnum, "Enum");
CFitBaseType Sint8Type = build<int8_t>(0x7F, TypeSint8, "Sint8");
CFitBaseType Uint8Type = build<int8_t>(0xFF, TypeUint8, "Uint8");
CFitBaseType Sint16Type = build<int16_t>(0x7FFF, TypeSint16, "Sint16");
CFitBaseType Uint16Type = build<uint16_t>(0xFFFF, TypeUint16, "Uint16");
CFitBaseType Sint32Type = build<int32_t>(0x7FFFFFFF, TypeSint32, "Sint32");
CFitBaseType Uint32Type = build<uint32_t>(0xFFFFFFFF, TypeUint32, "Uint32");
CFitBaseType StringType = build<char>(0x00, TypeString, "String");
CFitBaseType Float32Type = build<float>((float)0xFFFFFFFF, TypeFloat32, "Flaot32");
CFitBaseType Float64Type = build<double>((double)0xFFFFFFFFFFFFFFFFull, TypeFloat64, "Float64");

CFitBaseType Uint8zType = build<uint8_t>(0x00, TypeUint8z, "Uint8z");
CFitBaseType Uint16zType = build<uint16_t>(0x0000, TypeUint16z, "Uint16z");
CFitBaseType Uint32zType = build<uint32_t>(0x00000000, TypeUint32z, "Uint32z");
CFitBaseType ByteType = build<uint8_t>(0xFF, TypeByte, "Byte"); // Field is invalid if all bytes are invalid.

CFitBaseType InvalidType = build<uint8_t>(0, TypeInvalid, "Invalid");

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