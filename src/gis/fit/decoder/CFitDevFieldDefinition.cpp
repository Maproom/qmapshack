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

#include "gis/fit/decoder/CFitDevFieldDefinition.h"

#include "gis/fit/decoder/CFitDefinitionMessage.h"
#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/defs/CFitFieldProfile.h"
#include "gis/fit/defs/CFitProfileLookup.h"
#include "gis/fit/defs/fit_const.h"


CFitDevFieldDefinition::CFitDevFieldDefinition(CFitDefinitionMessage* parent, quint8 fieldNr, quint8 size, quint8 devDataIndex)
    : fieldNr(fieldNr), size(size), devDataIndex(devDataIndex)
{
}

CFitDevFieldDefinition::CFitDevFieldDefinition()
    : CFitDevFieldDefinition(nullptr, 0, 0, 0)
{
}

QString CFitDevFieldDefinition::fieldInfo() const
{
    QString fstr = QString("DEV FIELD %1: type %3, size %3")
                   .arg(getFieldNr())
                   .arg(getSize())
                   .arg(getDevDataIndex());
    return fstr;
}

quint8 CFitDevFieldDefinition::getFieldNr() const
{
    return fieldNr;
}

quint8 CFitDevFieldDefinition::getSize() const
{
    return size;
}

quint8 CFitDevFieldDefinition::getDevDataIndex() const {
    return devDataIndex;
}