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
#ifndef CGARMINSTRTBLUTF8_H
#define CGARMINSTRTBLUTF8_H

#include "IGarminStrTbl.h"

class CGarminStrTblUtf8 : public IGarminStrTbl
{
public:
    CGarminStrTblUtf8(const quint16 codepage, const quint8 mask, QObject* parent);
    virtual ~CGarminStrTblUtf8();

    void get(CFileExt& file, quint32 offset, type_e t, QStringList& info) override;
};
#endif //CGARMINSTRTBLUTF8_H
