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

#include "gis/fit/decoder/CFitFieldDefinition.h"

#include "gis/fit/decoder/CFitDefinitionMessage.h"
#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/defs/CFitProfileLockup.h"



static const quint8 fitEndianFlagMask = 0x80;


CFitFieldDefinition::CFitFieldDefinition(CFitDefinitionMessage* parent, quint8 defNr, quint8 size, quint8 type)
    : defNr(defNr), size(size), type(type), baseType(CFitBaseTypeMap::get(type)), parentDefintion(parent)
{
    fieldProfile = CFitProfileLockup::getFieldForProfile(parentDefintion ? parentDefintion->getGlobalMesgNr() : fitGlobalMesgNrInvalid, defNr);
}

CFitFieldDefinition::CFitFieldDefinition()
    : CFitFieldDefinition(nullptr, fitLocalMesgNrInvalid, 0, TypeInvalid)
{
}

QString CFitFieldDefinition::fieldInfo() const
{
    QString fstr = QString("%1 %2 (%3): %4, type %5, size %6, endian %7")
                   .arg(profile().hasSubfields() ? "dynamic" : profile().hasComponents() ? "component" : "field")
                   .arg(profile().getName())
                   .arg(getDefNr())
                   .arg(getBaseType().name())
                   .arg(getType())
                   .arg(getSize())
                   .arg(getEndianAbilityFlag());
    return fstr;
}

quint8 CFitFieldDefinition::getDefNr() const
{
    return defNr;
}

quint8 CFitFieldDefinition::getSize() const
{
    return size;
}

quint8 CFitFieldDefinition::getType() const
{
    return type;
}

bool CFitFieldDefinition::getEndianAbilityFlag() const
{
    return (type & fitEndianFlagMask) != 0;
}

const CFitBaseType& CFitFieldDefinition::getBaseType() const
{
    return *baseType;
}