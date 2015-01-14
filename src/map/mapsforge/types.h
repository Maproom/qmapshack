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

#ifndef TYPES_H
#define TYPES_H

#include <QtCore>

struct uintX
{
    uintX() : val(0)
    {
    }
    operator quint64()
    {
        return val;
    }
    quint64 val;
};

struct intX
{
    intX() : val(0)
    {
    }
    operator qint64()
    {
        return val;
    }
    qint64 val;
};

struct utf8
{
    operator QString()
    {
        return val;
    }
    QString val;
};

extern QDataStream& operator>>(QDataStream& s, uintX& v);
extern QDataStream& operator>>(QDataStream& s, intX& v);
extern QDataStream& operator>>(QDataStream& s, utf8& v);

#endif //TYPES_H

