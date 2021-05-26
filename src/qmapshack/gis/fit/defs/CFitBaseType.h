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

#ifndef CFITBASETYPE_H
#define CFITBASETYPE_H

#include <QtCore>

typedef enum
{
    eBaseTypeNrEnum = 0,
    eBaseTypeNrSint8 = 1,
    eBaseTypeNrUint8 = 2,
    eBaseTypeNrSint16 = 3,
    eBaseTypeNrUint16 = 4,
    eBaseTypeNrSint32 = 5,
    eBaseTypeNrUint32 = 6,
    eBaseTypeNrString = 7,
    eBaseTypeNrFloat32 = 8,
    eBaseTypeNrFloat64 = 9,
    eBaseTypeNrUint8z = 10,
    eBaseTypeNrUint16z = 11,
    eBaseTypeNrUint32z = 12,
    eBaseTypeNrByte = 13,
    eBaseTypeNrSint64 = 14,
    eBaseTypeNrUint64 = 15,
    eBaseTypeNrUint64z = 16,
    eBaseTypeNrInvalid = 0xff
} fit_base_type_nr_e;

class CFitBaseType final
{
public:
    CFitBaseType();
    CFitBaseType(fit_base_type_nr_e baseTypeNr, QString name, quint8 size, std::initializer_list<quint8> invalid);

    quint8 size() const;
    fit_base_type_nr_e nr() const;
    quint8 baseTypeField() const;
    const quint8* invalidValueBytes() const;
    bool isSizeUndefined() const;
    bool isInteger() const;
    bool isSignedInt() const;
    bool isUnsignedInt() const;
    bool isFloat() const;
    bool isNumber() const;
    bool isString() const;
    bool isByte() const;
    QString name() const;

private:
    // fixed size to 8, which is enough for float64
    quint8 invalidBytes[8] {0, 0, 0, 0, 0, 0, 0, 0};
    quint8 typeSize;
    fit_base_type_nr_e baseTypeNr;
    QString namestr;
};

class fit
{
public:
    static const CFitBaseType EnumType;
    static const CFitBaseType Sint8Type;
    static const CFitBaseType Uint8Type;
    static const CFitBaseType Sint16Type;
    static const CFitBaseType Uint16Type;
    static const CFitBaseType Sint32Type;
    static const CFitBaseType Uint32Type;
    static const CFitBaseType Sint64Type;
    static const CFitBaseType Uint64Type;
    static const CFitBaseType StringType;
    static const CFitBaseType Float32Type;
    static const CFitBaseType Float64Type;
    static const CFitBaseType Uint8zType;
    static const CFitBaseType Uint16zType;
    static const CFitBaseType Uint32zType;
    static const CFitBaseType Uint64zType;
    static const CFitBaseType ByteType;
    static const CFitBaseType InvalidType;
};

class CFitBaseTypeMap
{
public:
    static const quint8 fitBaseTypeNumMask = 0x1F; // 0000 0000 0001 1111

    /**
     * param nr: either the "real" base type number (0 -13) or the masked base type byte.
     */
    static CFitBaseType* get(quint8 nr);
private:
    static void initialize(QMap<quint8, CFitBaseType>& baseTypesMap);
};


#endif //CFITBASETYPE_H
