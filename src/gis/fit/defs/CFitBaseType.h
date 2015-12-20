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

#ifndef CFITBASETYPE_H
#define CFITBASETYPE_H

#include <QtCore>

typedef enum
{
    TypeEnum = 0,
    TypeSint8 = 1,
    TypeUint8 = 2,
    TypeSint16 = 3,
    TypeUint16 = 4,
    TypeSint32 = 5,
    TypeUint32 = 6,
    TypeString = 7,
    TypeFloat32 = 8,
    TypeFloat64 = 9,
    TypeUint8z = 10,
    TypeUint16z = 11,
    TypeUint32z = 12,
    TypeByte = 13,
    TypeInvalid = 0xff
} BaseTypeNr;

class CFitBaseType
{
public:
    CFitBaseType(BaseTypeNr baseTypeNr, uint8_t* invalidBytes, uint8_t size, QString str);
    CFitBaseType();
    ~CFitBaseType();

    uint8_t size();
    BaseTypeNr nr();
    uint8_t* invalidValueBytes();
    bool isInteger();
    bool isSignedInt();
    bool isUnsignedInt();
    bool isFloat();
    bool isNumber();
    QString str();

private:
    uint8_t* invalidBytes;
    uint8_t typeSize;
    BaseTypeNr baseTypeNr;
    QString name;
};


extern CFitBaseType EnumType;
extern CFitBaseType Sint8Type;
extern CFitBaseType Uint8Type;
extern CFitBaseType Sint16Type;
extern CFitBaseType Uint16Type;
extern CFitBaseType Sint32Type;
extern CFitBaseType Uint32Type;
extern CFitBaseType StringType;
extern CFitBaseType Float32Type;
extern CFitBaseType Float64Type;

extern CFitBaseType Uint8zType;
extern CFitBaseType Uint16zType;
extern CFitBaseType Uint32zType;
extern CFitBaseType ByteType; // Field is invalid if all bytes are invalid.

extern CFitBaseType InvalidType;

class CFitBaseTypeMap
{
public:
    static const uint8_t BaseTypeNumMask =0x1F; // 0000 0000 0001 1111

    /**
     * param nr: either the "real" base type number (0 -13) or the masked base type byte.
     */
    static CFitBaseType* get(uint8_t nr);
private:
    static QMap<uint8_t, CFitBaseType> baseTypes;
};


#endif //CFITBASETYPE_H
