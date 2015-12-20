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

#include "gis/fit/decoder/CFitFieldDefinition.h"

#include "gis/fit/decoder/CFitDefinitionMessage.h"
#include "gis/fit/defs/CFitProfileLockup.h"



static const uint8_t EndianFlagMask = 0x80;


CFitFieldDefinition::CFitFieldDefinition(CFitDefinitionMessage* parent, uint8_t defNr, uint8_t size, uint8_t type)
        : defNr(defNr), size(size), type(type), parentDefintion(parent) {
    baseType = CFitBaseTypeMap::get(type);
    if(parentDefintion)
        fieldProfile = CFitProfileLockup::getFieldForProfile(parentDefintion->getGlobalMesgNr(), defNr);
    else
        fieldProfile = nullptr;
}

CFitFieldDefinition::CFitFieldDefinition() : CFitFieldDefinition(nullptr, LocalMesgNrInvalid, 0, TypeInvalid) { }

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