/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg <>

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
#include "gis/fit/defs/CFitFieldProfile.h"
#include "gis/fit/defs/CFitProfileLookup.h"
#include "gis/fit/defs/fit_const.h"

static const quint8 fitEndianFlagMask = 0x80;

CFitFieldDefinition::CFitFieldDefinition(CFitDefinitionMessage* parent, CFitFieldProfile* fieldProfile, quint8 defNr,
                                         quint8 devDataIdx, quint8 size, quint8 type)
    : defNr(defNr),
      devDataIdx(devDataIdx),
      size(size),
      type(type),
      baseType(CFitBaseTypeMap::get(type)),
      parentDefintion(parent),
      fieldProfile(fieldProfile) {}

CFitFieldDefinition::CFitFieldDefinition(CFitDefinitionMessage* parent, quint8 defNr, quint8 size, quint8 type)
    : CFitFieldDefinition(parent, nullptr, defNr, fitDevDataIndexInvalid, size, type) {
  fieldProfile = CFitProfileLookup::getFieldForProfile(
      parentDefintion ? parentDefintion->getGlobalMesgNr() : fitGlobalMesgNrInvalid, defNr);
}

CFitFieldDefinition::CFitFieldDefinition()
    : CFitFieldDefinition(nullptr, fitLocalMesgNrInvalid, 0, eBaseTypeNrInvalid) {}

QString CFitFieldDefinition::fieldInfo() const {
  QString fstr = QString("%1 %2 %3 (%4): %5 %6, type %7, size %8, endian %9")
                     .arg(profile().hasSubfields()    ? "dynamic"
                          : profile().hasComponents() ? "component"
                                                      : "field",
                          profile().getName(), profile().getFieldType() == eFieldTypeDevelopment ? " DEV" : "")
                     .arg(getDevDataIdx())
                     .arg(getDefNr())
                     .arg(getBaseType().name())
                     .arg(getType(), getSize(), getEndianAbilityFlag());
  return fstr;
}

quint8 CFitFieldDefinition::getDefNr() const { return defNr; }

quint8 CFitFieldDefinition::getDevDataIdx() const { return devDataIdx; }

QPair<quint8, quint8> CFitFieldDefinition::getDevProfileId() const { return QPair<quint8, quint8>(devDataIdx, defNr); }

quint8 CFitFieldDefinition::getSize() const { return size; }

quint8 CFitFieldDefinition::getType() const { return type; }

bool CFitFieldDefinition::getEndianAbilityFlag() const { return (type & fitEndianFlagMask) != 0; }

const CFitBaseType& CFitFieldDefinition::getBaseType() const { return *baseType; }
