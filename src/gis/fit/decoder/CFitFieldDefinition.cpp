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



static const uint8_t EndianFlagMask = 0x80;


CFitFieldDefinition::CFitFieldDefinition(CFitDefinitionMessage* parent, uint8_t defNr, uint8_t size, uint8_t type)
        : defNr(defNr), size(size), type(type), parentDefintion(parent)
{
    baseType = CFitBaseTypeMap::get(type);
    fieldProfile = CFitProfileLockup::getFieldForProfile(parentDefintion ? parentDefintion->getGlobalMesgNr() : GlobalMesgNrInvalid, defNr);
}

CFitFieldDefinition::CFitFieldDefinition()
        : CFitFieldDefinition(nullptr, LocalMesgNrInvalid, 0, TypeInvalid) { }

QString CFitFieldDefinition::fieldInfo() const
{
    QString fstr = QString("%1 %2 (%3): %4, type %5, size %6, endian %7")
            .arg(profile()->hasSubfields() ? "dynamic" : profile()->hasComponents() ? "component" : "field")
            .arg(profile()->getName())
            .arg(getDefNr())
            .arg(getBaseType()->str())
            .arg(getType())
            .arg(getSize())
            .arg(getEndianAbilityFlag());
    return fstr;
}

uint8_t CFitFieldDefinition::getDefNr() const {
    return defNr;
}

uint8_t CFitFieldDefinition::getSize() const {
    return size;
}

uint8_t CFitFieldDefinition::getType() const {
    return type;
}

bool CFitFieldDefinition::getEndianAbilityFlag() const {
    return (type & EndianFlagMask) != 0;
}

CFitBaseType* CFitFieldDefinition::getBaseType() const
{
    return baseType;
}