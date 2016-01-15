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
{
}


CFitBaseType::CFitBaseType(BaseTypeNr baseTypeNr, quint8* invalidBytes, quint8 size, QString name)
    : typeSize(size), baseTypeNr(baseTypeNr), namestr(name)
{
    memcpy(this->invalidBytes, invalidBytes, size);
}

CFitBaseType::~CFitBaseType()
{
}

quint8 CFitBaseType::size() const
{
    return typeSize;
}
bool CFitBaseType::isSizeUndefined() const
{
    return typeSize == 0;
}

BaseTypeNr CFitBaseType::nr() const
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
CFitBaseType build(BaseTypeNr baseTypeNr, QString name, quint8 size, T invalid)
{
    return CFitBaseType (baseTypeNr, (quint8*)&invalid, size, name);
}

CFitBaseType EnumType = build<quint8>(TypeEnum, "Enum", sizeof(quint8), 0xFF);
CFitBaseType Sint8Type = build<qint8>(TypeSint8, "Sint8", sizeof(qint8), 0x7F);
CFitBaseType Uint8Type = build<quint8>(TypeUint8, "Uint8", sizeof(quint8),  0xFF);
CFitBaseType Sint16Type = build<qint16>(TypeSint16, "Sint16", sizeof(qint16), 0x7FFF);
CFitBaseType Uint16Type = build<quint16>(TypeUint16, "Uint16",sizeof(quint16), 0xFFFF);
CFitBaseType Sint32Type = build<qint32>(TypeSint32, "Sint32", sizeof(qint32),0x7FFFFFFF);
CFitBaseType Uint32Type = build<quint32>(TypeUint32, "Uint32", sizeof(quint32),0xFFFFFFFF);
CFitBaseType StringType = build<char>(TypeString, "String", 0,0x00);
CFitBaseType Float32Type = build<float>(TypeFloat32, "Flaot32", sizeof(float), (float)0xFFFFFFFF);
CFitBaseType Float64Type = build<double>(TypeFloat64, "Float64", sizeof(double), (double)0xFFFFFFFFFFFFFFFFull);

CFitBaseType Uint8zType = build<quint8>(TypeUint8z, "Uint8z", sizeof(quint8), 0x00);
CFitBaseType Uint16zType = build<quint16>(TypeUint16z, "Uint16z", sizeof(quint16), 0x0000);
CFitBaseType Uint32zType = build<quint32>(TypeUint32z, "Uint32z", sizeof(quint32), 0x00000000);
CFitBaseType ByteType = build<quint8>(TypeByte, "Byte", 0, 0xFF); // Field is invalid if all bytes are invalid.

CFitBaseType InvalidType = build<quint8>(TypeInvalid, "Invalid",0, 0);

static QMap<quint8, CFitBaseType> buildBaseTypeMap()
{
    QMap<quint8, CFitBaseType> baseTypes;
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
}

QMap<quint8, CFitBaseType> CFitBaseTypeMap::baseTypes = buildBaseTypeMap();


CFitBaseType* CFitBaseTypeMap::get(quint8 nr)
{
    if (baseTypes.contains(nr & fitBaseTypeNumMask))
    {
        return &baseTypes[nr & fitBaseTypeNumMask];
    }

    return &InvalidType;
}
