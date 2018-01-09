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
#include "CGarminStrTbl6.h"

#include <QtCore>

const char CGarminStrTbl6::str6tbl1[] =
{
    ' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
    ,0,0,0,0,0
    ,'0','1','2','3','4','5','6','7','8','9'
    ,0,0,0,0,0,0
};

const char CGarminStrTbl6::str6tbl2[] =
{
    //@   !   "   #   $   %   &    '   (   )   *   +   ,   -   .   /
    '@','!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/'
    ,0,0,0,0,0,0,0,0,0,0
    //:   ;   <   =   >   ?
    ,':',';','<','=','>','?'
    ,0,0,0,0,0,0,0,0,0,0,0
    //[    \   ]   ^   _
    ,'[','\\',']','^','_'
};

const char CGarminStrTbl6::str6tbl3[] =
{
    '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'
};

CGarminStrTbl6::CGarminStrTbl6(const quint16 codepage, const quint8 mask, QObject * parent)
    : IGarminStrTbl(codepage, mask, parent)
{
}


CGarminStrTbl6::~CGarminStrTbl6()
{
}


void CGarminStrTbl6::fill()
{
    quint32 tmp;
    if(bits < 6)
    {
        tmp = *p++;
        reg |= tmp << (24 - bits);
        bits += 8;
    }
}


void CGarminStrTbl6::get(CFileExt& file, quint32 offset, type_e t, QStringList& labels)
{
    labels.clear();

    offset = calcOffset(file, offset,t);

    if(offset == 0xFFFFFFFF)
    {
        return;
    }

    if(offset > (quint32)sizeLBL1)
    {
        //         qWarning() << "Index into string table to large" << hex << offset << dataLBL.size() << hdrLbl->addr_shift << hdrNet->net1_addr_shift;
        return;
    }

    quint8 c1  = 0;
    quint8 c2  = 0;
    quint32 idx = 0;
    reg         = 0;
    bits        = 0;

    QByteArray data;
    quint32 size = (sizeLBL1 - offset) < 200 ? (sizeLBL1 - offset) : 200;

    readFile(file, offsetLBL1 + offset, size, data);

    p = (quint8*)data.data();

    fill();
    while(idx < (sizeof(buffer) - 1))
    {
        c1 = reg >> 26;
        reg <<= 6;
        bits -= 6;
        fill();
        //terminator
        if(c1 > 0x2F)
        {
            break;
        }

        c2 = str6tbl1[c1];
        if(c2 == 0)
        {
            if(c1 == 0x1C)
            {
                c1 = reg >> 26;
                reg <<= 6;
                bits -= 6;
                fill();
                buffer[idx++] = str6tbl2[c1];
            }
            else if(c1 == 0x1B)
            {
                c1 = reg >> 26;
                reg <<= 6;
                bits -= 6;
                fill();
                buffer[idx++] = str6tbl3[c1];
            }
            else if(c1 > 0x1C && c1 < 0x20)
            {
                buffer[idx] = 0;
                if(strlen(buffer))
                {
                    if (codepage != 0)
                    {
                        labels << codec->toUnicode(buffer);
                    }
                    else
                    {
                        labels << buffer;
                    }
                }
                idx = 0;
                buffer[0] = 0;
            }
        }
        else
        {
            buffer[idx++] = str6tbl1[c1];
        }
    }

    buffer[idx] = 0;
    if(strlen(buffer))
    {
        if (codepage != 0)
        {
            labels << codec->toUnicode(buffer);
        }
        else
        {
            labels << buffer;
        }
    }
}
