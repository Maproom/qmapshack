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
    baseTypesMap.insert(fitEnumType.nr(), fitEnumType);
    baseTypesMap.insert(fitSint8Type.nr(), fitSint8Type);
    baseTypesMap.insert(fitUint8Type.nr(), fitUint8Type);
    baseTypesMap.insert(fitSint16Type.nr(), fitSint16Type);
    baseTypesMap.insert(fitUint16Type.nr(), fitUint16Type);
    baseTypesMap.insert(fitSint32Type.nr(), fitSint32Type);
    baseTypesMap.insert(fitUint32Type.nr(), fitUint32Type);
    baseTypesMap.insert(fitSint64Type.nr(), fitSint64Type);
    baseTypesMap.insert(fitUint64Type.nr(), fitUint64Type);
    baseTypesMap.insert(fitStringType.nr(), fitStringType);
    baseTypesMap.insert(fitFloat32Type.nr(), fitFloat32Type);
    baseTypesMap.insert(fitFloat64Type.nr(), fitFloat64Type);
    baseTypesMap.insert(fitUint8zType.nr(), fitUint8zType);
    baseTypesMap.insert(fitUint16zType.nr(), fitUint16zType);
    baseTypesMap.insert(fitUint32zType.nr(), fitUint32zType);
    baseTypesMap.insert(fitUint64zType.nr(), fitUint64zType);
    baseTypesMap.insert(fitByteType.nr(), fitByteType);
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
