/**********************************************************************************************
    Copyright (C) 2008 Oliver Eichler <oliver.eichler@gmx.de>

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
#ifndef CGARMINSTRTBL6_H
#define CGARMINSTRTBL6_H

#include "IGarminStrTbl.h"

class CGarminStrTbl6 : public IGarminStrTbl
{
public:
    CGarminStrTbl6(const quint16 codepage, const quint8 mask, QObject* parent);
    virtual ~CGarminStrTbl6();

    void get(CFileExt& file, quint32 offset, type_e t, QStringList& info) override;

private:
    static const char str6tbl1[];
    static const char str6tbl2[];
    static const char str6tbl3[];

    void fill();
    /// temp shift reg buffer
    quint32 reg = 0;
    /// bits in buffer
    quint32 bits = 0;
    /// pointer to current data;
    const quint8* p = nullptr;
};
#endif                           //CGARMINSTRTBL6_H
