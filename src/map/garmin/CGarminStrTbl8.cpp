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

#include "CGarminStrTbl8.h"

#include <QtCore>

CGarminStrTbl8::CGarminStrTbl8(const quint16 codepage, const quint8 mask, QObject * parent)
    : IGarminStrTbl(codepage, mask, parent)
{
}


CGarminStrTbl8::~CGarminStrTbl8()
{
}


void CGarminStrTbl8::get(CFileExt& file, quint32 offset, type_e t, QStringList& info)
{
    info.clear();
    offset = calcOffset(file, offset, t);

    if(offset == 0xFFFFFFFF)
    {
        return;
    }

    if(offset > (quint32)sizeLBL1)
    {
        //qWarning() << "Index into string table to large" << hex << offset << dataLBL.size() << hdrLbl->addr_shift << hdrNet->net1_addr_shift;
        return;
    }

    QByteArray data;
    quint32 size = (sizeLBL1 - offset) < 200 ? (sizeLBL1 - offset) : 200;
    readFile(file, offsetLBL1 + offset, size, data);
    char * lbl = data.data();

    char * pBuffer = buffer;
    *pBuffer = 0;
    while(*lbl != 0)
    {
        if((unsigned)*lbl >= 0x1B && (unsigned)*lbl <= 0x1F)
        {
            *pBuffer = 0;
            if(strlen(buffer))
            {
                if (codepage != 0)
                {
                    info << codec->toUnicode(buffer);
                }
                else
                {
                    info << buffer;
                }
                pBuffer = buffer;
                *pBuffer = 0;
            }
            ++lbl;
            continue;
        }
        else if((unsigned)*lbl < 0x07)
        {
            ++lbl;
            continue;
        }
        else
        {
            *pBuffer++ = *lbl++;
        }
    }

    *pBuffer = 0;
    if(strlen(buffer))
    {
        if (codepage != 0)
        {
            info << codec->toUnicode(buffer);
        }
        else
        {
            info << buffer;
        }
    }
}
