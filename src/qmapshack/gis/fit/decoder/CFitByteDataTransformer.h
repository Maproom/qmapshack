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

#ifndef CFITBYTEDATATRANSFORMER_H
#define CFITBYTEDATATRANSFORMER_H

#include <QtCore>

class CFitBaseType;
class CFitFieldDefinition;

class CFitByteDataTransformer
{
public:
    CFitByteDataTransformer() = delete;
    static quint64 getUIntValue(const CFitBaseType& baseType, quint8* rawData);
    static qint64 getSIntValue(const CFitBaseType &baseType, quint8 *rawData);
    static qreal getFloatValue(const CFitBaseType& baseType, quint8* rawData);
    /*
     * param rawData: the fit utf-8 string, 0 terminated.
     */
    static QString getString(quint8* rawData, quint8 length);
    static QByteArray getBytes(quint8* rawData, quint8 length);
    static void swapFieldData(const CFitFieldDefinition& fieldDef, quint8* fieldData);

private:
    static quint8 getUint8(quint8* rawData);
    static quint16 getUint16(quint8* rawData);
    static quint32 getUint32(quint8* rawData);
    static quint64 getUint64(quint8* rawData);
    static qint8 getSint8(quint8* rawData);
    static qint16 getSint16(quint8* rawData);
    static qint32 getSint32(quint8* rawData);
    static qint64 getSint64(quint8* rawData);
    static qreal getFloat32(quint8* rawData);
    static qreal getFloat64(quint8* rawData);
};

#endif //CFITBYTEDATATRANSFORMER_H
