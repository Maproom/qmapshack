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

#include "types.h"

QDataStream& operator>>(QDataStream& s, uintX& v)
{
    quint8 tmp;
    int shift   = 0;
    v.val       = 0;

    s >> tmp;
    while(tmp & 0x80)
    {
        v.val |= (tmp & 0x7F)<<shift;
        shift += 7;
        s >> tmp;
    }

    v.val |= tmp << shift;

    return s;
}

QDataStream& operator>>(QDataStream& s, intX& v)
{
    quint8 tmp;
    int shift   = 0;
    v.val       = 0;

    s >> tmp;
    while(tmp & 0x80)
    {
        v.val |= (tmp & 0x7F)<<shift;
        shift += 7;
        s >> tmp;
    }

    if(tmp & 0x40)
    {
        v.val = -(v.val | ((tmp & 0x3f) << shift));
    }
    else
    {
        v.val |= tmp << shift;
    }

    return s;
}

QDataStream& operator>>(QDataStream& s, utf8& v)
{
    uintX l;
    s >> l;

    v.val = QString::fromUtf8(s.device()->read(l));

    return s;
}
