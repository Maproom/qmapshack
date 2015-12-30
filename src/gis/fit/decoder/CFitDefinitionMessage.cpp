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

#include "gis/fit/decoder/CFitDefinitionMessage.h"

#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/defs/CFitProfileLockup.h"

static const uint8_t ArchitecureEndianMask = 0x01;


CFitDefinitionMessage::CFitDefinitionMessage()
    : CFitDefinitionMessage(LocalMesgNrInvalid)
{
}

CFitDefinitionMessage::CFitDefinitionMessage(const CFitDefinitionMessage& copy)
    : globalMesgNr(copy.globalMesgNr), architecture(copy.architecture), nrOfFields(copy.nrOfFields), localMesgNr(copy.localMesgNr),
    fields(copy.fields)
{
    messageProfile = CFitProfileLockup::getProfile(globalMesgNr);
    for(CFitFieldDefinition& field : fields)
    {
        field.setParent(this);
    }
}

CFitDefinitionMessage::CFitDefinitionMessage(uint8_t localMesgNr)
    : architecture(0), nrOfFields(0), localMesgNr(localMesgNr), fields()
{
    setGlobalMesgNr(GlobalMesgNrInvalid);
}


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

const QList<CFitFieldDefinition>&CFitDefinitionMessage::getFields() const
{
    return fields;
}

bool CFitDefinitionMessage::hasField(const uint8_t fieldNum) const
{
    for (int i=0; i< fields.size(); i++)
    {
        if (fieldNum == fields[i].getDefNr())
        {
            return true;
        }
    }
    return false;
}

// dummy field for unknown field nr.
static const CFitFieldDefinition* dummyDefinitionField = nullptr;
const CFitFieldDefinition& invalidDefinitionField()
{
    if(!dummyDefinitionField)
    {
        dummyDefinitionField = new CFitFieldDefinition();
    }
    return *dummyDefinitionField;
}

const CFitFieldDefinition& CFitDefinitionMessage::getField(const uint8_t fieldNum) const
{
    for (int i=0; i< fields.size(); i++)
    {
        if (fieldNum == fields[i].getDefNr())
        {
            return fields[i];
        }
    }
    return invalidDefinitionField();
}


const CFitFieldDefinition& CFitDefinitionMessage::getFieldByIndex(const uint16_t index) const
{
    if (index < fields.size())
    {
        return fields[index];
    }
    return invalidDefinitionField();
}


QStringList CFitDefinitionMessage::messageInfo() const
{
    QStringList list;
    list << QString("Definition %1 (%2) local nr %3, arch %4, # fields %5")
        .arg(profile()->getName())
        .arg(getGlobalMesgNr())
        .arg(getLocalMesgNr())
        .arg(getArchitectureBit())
        .arg(getNrOfFields());

    for(const CFitFieldDefinition& field: fields)
    {
        list << field.fieldInfo();
    }
    return list;
}



