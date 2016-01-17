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
    eBaseTypeNrInvalid = 0xff
} fit_base_type_nr_e;

class CFitBaseType final
{
public:
    CFitBaseType();
    CFitBaseType(fit_base_type_nr_e baseTypeNr, QString name, quint8 size, std::initializer_list<quint8> invalid);

    quint8 size() const;
    fit_base_type_nr_e nr() const;
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
    quint8 invalidBytes[8];
    quint8 typeSize;
    fit_base_type_nr_e baseTypeNr;
    QString namestr;
};

static const CFitBaseType EnumType= CFitBaseType(eBaseTypeNrEnum, "Enum", sizeof(quint8), {0xFF});
static const CFitBaseType Sint8Type = CFitBaseType(eBaseTypeNrSint8, "Sint8", sizeof(qint8), {0x7F});
static const CFitBaseType Uint8Type = CFitBaseType(eBaseTypeNrUint8, "Uint8", sizeof(quint8), {0xFF});
static const CFitBaseType Sint16Type = CFitBaseType(eBaseTypeNrSint16, "Sint16", sizeof(qint16), {0x7F, 0xFF});
static const CFitBaseType Uint16Type = CFitBaseType(eBaseTypeNrUint16, "Uint16", sizeof(quint16), {0xFF, 0xFF});
static const CFitBaseType Sint32Type = CFitBaseType(eBaseTypeNrSint32, "Sint32", sizeof(qint32), {0x7F, 0xFF, 0xFF, 0xFF});
static const CFitBaseType Uint32Type = CFitBaseType(eBaseTypeNrUint32, "Uint32", sizeof(quint32), {0xFF, 0xFF, 0xFF, 0xFF});
static const CFitBaseType StringType = CFitBaseType(eBaseTypeNrString, "String", 0, {0x00}); // Field is invalid if all bytes are invalid.
static const CFitBaseType Float32Type = CFitBaseType(eBaseTypeNrFloat32, "Flaot32", sizeof(float), {0xFF, 0xFF, 0xFF, 0xFF});
static const CFitBaseType Float64Type = CFitBaseType(eBaseTypeNrFloat64, "Float64", sizeof(double), {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});

static const CFitBaseType Uint8zType = CFitBaseType(eBaseTypeNrUint8z, "Uint8z", sizeof(quint8), {0x00});
static const CFitBaseType Uint16zType = CFitBaseType(eBaseTypeNrUint16z, "Uint16z", sizeof(quint16), {0x00, 0x00});
static const CFitBaseType Uint32zType = CFitBaseType(eBaseTypeNrUint32z, "Uint32z", sizeof(quint32), {0x00, 0x00, 0x00, 0x00});
static const CFitBaseType ByteType = CFitBaseType(eBaseTypeNrByte, "Byte", 0, {0xFF}); // Field is invalid if all bytes are invalid.

static const CFitBaseType InvalidType = CFitBaseType(eBaseTypeNrInvalid, "Invalid", 0, {0});


class CFitBaseTypeMap
{
public:
    static const quint8 fitBaseTypeNumMask =0x1F; // 0000 0000 0001 1111

    /**
     * param nr: either the "real" base type number (0 -13) or the masked base type byte.
     */
    static CFitBaseType* get(quint8 nr);
private:
    static void initialize(QMap<quint8, CFitBaseType>& baseTypesMap);
};


#endif //CFITBASETYPE_H
