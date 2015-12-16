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

#include "gis/fit/decoder/CFitDefinitionMessage.h"

#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/defs/CFitProfileLockup.h"

static const uint8_t ArchitecureEndianMask = 0x01;


// TODO const invalid local and global mesg nr
CFitDefinitionMessage::CFitDefinitionMessage()
        : CFitDefinitionMessage(0xff) {}

CFitDefinitionMessage::CFitDefinitionMessage(const CFitDefinitionMessage& copy)
: localMesgNr(copy.localMesgNr), globalMesgNr(copy.globalMesgNr), architecture(copy.architecture),
  nrOfFields(copy.nrOfFields), fields(copy.fields)
        //, messageProfile(copy.messageProfile)
{
    messageProfile = CFitProfileLockup::getProfile(globalMesgNr);
}

CFitDefinitionMessage::CFitDefinitionMessage(uint8_t localMesgNr)
        : localMesgNr(localMesgNr), globalMesgNr(0xffff), architecture(0), nrOfFields(0), fields(), messageProfile(nullptr) {}


void CFitDefinitionMessage::setArchiteture(uint8_t arch)
{
    architecture = arch;
}

void CFitDefinitionMessage::setGlobalMesgNr(uint16_t globalMesgNr)
{
    this->globalMesgNr = globalMesgNr;
    messageProfile = CFitProfileLockup::getProfile(globalMesgNr);
}

void CFitDefinitionMessage::setNrOfFields(uint8_t nrOfFields)
{
    this->nrOfFields = nrOfFields;
}

uint16_t CFitDefinitionMessage::getGlobalMesgNr() const
{
    return globalMesgNr;
}

uint8_t CFitDefinitionMessage::getLocalMesgNr() const
{
    return localMesgNr;
}

uint8_t CFitDefinitionMessage::getArchitectureBit() const
{
    return architecture & ArchitecureEndianMask;
}
uint8_t CFitDefinitionMessage::getNrOfFields() const
{
    return nrOfFields;
}

void CFitDefinitionMessage::addField(CFitFieldDefinition fieldDef)
{
    fields.append(fieldDef);
}

QList<CFitFieldDefinition>&CFitDefinitionMessage::getFields()
{
    return fields;
}

CFitFieldDefinition* CFitDefinitionMessage::getField(const uint8_t fieldNum)
{
    for (int i=0; i< fields.size(); i++)
    {
        if (fieldNum == fields[i].getDefNr())
            return &(fields[i]);
    }

    return nullptr;
}



CFitFieldDefinition* CFitDefinitionMessage::getFieldByIndex(const uint16_t index)
{
    if (index < fields.size())
        return &(fields[index]);

    return nullptr;
}


QString CFitDefinitionMessage::messageInfo()
{
    QString str = QString("Definition %1 (%2) %3 [loc] %4 [arch] %5 [#field]")
            .arg(profile() ? profile()->getName() : "?")
            .arg(getGlobalMesgNr())
            .arg(getLocalMesgNr())
            .arg(getArchitectureBit())
            .arg(getNrOfFields());

    for(CFitFieldDefinition& field: fields)
    {
            QString fstr = QString("{ field %1 (%2): %3 %4 [type] %5 [size] %6 }")
                .arg(field.profile() ? field.profile()->getName() : "?")
                .arg(field.getDefNr())
                .arg(field.getBaseType()->str())
                .arg(field.getType())
        .arg(field.getSize())
        .arg(field.getEndianAbilityFlag());
        str += fstr;
    }
    return str;
}



