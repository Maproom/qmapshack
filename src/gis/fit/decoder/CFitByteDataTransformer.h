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

#ifndef CFITBYTEDATATRANSFORMER_H
#define CFITBYTEDATATRANSFORMER_H

#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/decoder/CFitFieldDefinition.h"

#include <QtCore>

class CFitByteDataTransformer {
public:
    static unsigned int getUIntValue(CFitBaseType* baseType, uint8_t* rawData);
    static int getSIntValue(CFitBaseType *baseType, uint8_t *rawData);
    static double getFloatValue(CFitBaseType* baseType, uint8_t* rawData);
    /*
     * param rawData: the fit utf-8 string, 0 terminated.
     */
    static QString getString(uint8_t* rawData, uint8_t length=0);
    static QByteArray getBytes(uint8_t* rawData, uint8_t length);
    static void swapFieldData(const CFitFieldDefinition& fieldDef, uint8_t* fieldData);

private:
    static uint8_t getUint8(uint8_t* rawData);
    static uint16_t getUint16(uint8_t* rawData);
    static uint32_t getUint32(uint8_t* rawData);
    static int8_t getSint8(uint8_t* rawData);
    static int16_t getSint16(uint8_t* rawData);
    static int32_t getSint32(uint8_t* rawData);
    static float getFloat32(uint8_t* rawData);
    static double getFloat64(uint8_t* rawData);
};

#endif //CFITBYTEDATATRANSFORMER_H
