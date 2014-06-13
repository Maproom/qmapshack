/**********************************************************************************************
    Copyright (C) 2008 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef IGARMINSTRTBL_H
#define IGARMINSTRTBL_H

#include <QObject>

class CFileExt;
class QByteArray;
class QStringList;

class IGarminStrTbl : public QObject
{
    public:
        IGarminStrTbl(const quint16 codepage, const quint8 mask, QObject * parent);
        virtual ~IGarminStrTbl();

        enum type_e {norm,poi,net};

        void registerLBL1(const quint32 offset, const quint32 size, const quint8 shift){offsetLBL1 = offset; sizeLBL1 = size; addrshift1 = shift;}
        void registerLBL6(const quint32 offset, const quint32 size){offsetLBL6 = offset; sizeLBL6 = size;}
        void registerNET1(const quint32 offset, const quint32 size, const quint8 shift){offsetNET1 = offset; sizeNET1 = size; addrshift2 = shift;}

        virtual void get(CFileExt& file, quint32 offset, type_e t, QStringList& info) = 0;
    protected:
        void readFile(CFileExt &file, quint32 offset, quint32 size, QByteArray& data);
        quint32 calcOffset(CFileExt& file, const quint32 offset, type_e t);

        quint32 offsetLBL1;
        quint32 sizeLBL1;
        quint32 offsetLBL6;
        quint32 sizeLBL6;
        quint32 offsetNET1;
        quint32 sizeNET1;

        quint8 addrshift1;
        quint8 addrshift2;

        // conversion of strings
        quint16 codepage;
        QTextCodec * codec;
        const quint8 mask;
        quint32 mask32;
        quint64 mask64;

        char buffer[1024];
};
#endif                           //IGARMINSTRTBL_H
