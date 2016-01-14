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

static const quint8 fitArchitecureEndianMask = 0x01;


CFitDefinitionMessage::CFitDefinitionMessage()
    : CFitDefinitionMessage(fitLocalMesgNrInvalid)
{
}

CFitDefinitionMessage::CFitDefinitionMessage(const CFitDefinitionMessage& copy)
    : globalMesgNr(copy.globalMesgNr), architecture(copy.architecture), nrOfFields(copy.nrOfFields),
      localMesgNr(copy.localMesgNr), fields(copy.fields), messageProfile(CFitProfileLockup::getProfile(globalMesgNr))
{
    for(CFitFieldDefinition& field : fields)
    {
        field.setParent(this);
    }
}

CFitDefinitionMessage::CFitDefinitionMessage(quint8 localMesgNr)
    : globalMesgNr(fitGlobalMesgNrInvalid), architecture(0), nrOfFields(0), localMesgNr(localMesgNr), fields(),
      messageProfile(CFitProfileLockup::getProfile(fitGlobalMesgNrInvalid))
{
}


void CFitDefinitionMessage::setArchiteture(quint8 arch)
{
    architecture = arch;
}

void CFitDefinitionMessage::setGlobalMesgNr(quint16 globalMesgNr)
{
    this->globalMesgNr = globalMesgNr;
    messageProfile = CFitProfileLockup::getProfile(globalMesgNr);
}

void CFitDefinitionMessage::setNrOfFields(quint8 nrOfFields)
{
    this->nrOfFields = nrOfFields;
}

quint16 CFitDefinitionMessage::getGlobalMesgNr() const
{
    return globalMesgNr;
}

quint8 CFitDefinitionMessage::getLocalMesgNr() const
{
    return localMesgNr;
}

quint8 CFitDefinitionMessage::getArchitectureBit() const
{
    return architecture & fitArchitecureEndianMask;
}
quint8 CFitDefinitionMessage::getNrOfFields() const
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

bool CFitDefinitionMessage::hasField(const quint8 fieldNum) const
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

const CFitFieldDefinition& CFitDefinitionMessage::getField(const quint8 fieldNum) const
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


const CFitFieldDefinition& CFitDefinitionMessage::getFieldByIndex(const quint16 index) const
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
        .arg(profile().getName())
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



