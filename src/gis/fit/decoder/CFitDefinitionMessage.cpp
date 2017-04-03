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
#include "gis/fit/defs/CFitProfile.h"
#include "gis/fit/defs/CFitProfileLookup.h"
#include "gis/fit/defs/fit_const.h"

static const quint8 fitArchitecureEndianMask = 0x01;


CFitDefinitionMessage::CFitDefinitionMessage()
    : CFitDefinitionMessage(fitLocalMesgNrInvalid, false)
{
}

CFitDefinitionMessage::CFitDefinitionMessage(const CFitDefinitionMessage& copy)
    : globalMesgNr(copy.globalMesgNr), architecture(copy.architecture), nrOfFields(copy.nrOfFields),
    nrOfDevFields(copy.nrOfDevFields), localMesgNr(copy.localMesgNr), devFlag(copy.devFlag), fields(copy.fields),
    devFields(copy.devFields), messageProfile(CFitProfileLookup::getProfile(globalMesgNr))
{
    for(CFitFieldDefinition& field : fields)
    {
        field.setParent(this);
    }
}

CFitDefinitionMessage::CFitDefinitionMessage(quint8 localMesgNr, bool devFlag)
    : globalMesgNr(fitGlobalMesgNrInvalid), architecture(0), nrOfFields(0), nrOfDevFields(0), localMesgNr(localMesgNr),
      devFlag(devFlag), fields(), devFields(), messageProfile(CFitProfileLookup::getProfile(fitGlobalMesgNrInvalid))
{
}


void CFitDefinitionMessage::setArchiteture(quint8 arch)
{
    architecture = arch;
}

void CFitDefinitionMessage::setGlobalMesgNr(quint16 globalMesgNr)
{
    this->globalMesgNr = globalMesgNr;
    messageProfile = CFitProfileLookup::getProfile(globalMesgNr);
}

void CFitDefinitionMessage::setNrOfFields(quint8 nrOfFields)
{
    this->nrOfFields = nrOfFields;
}

void CFitDefinitionMessage::setNrOfDevFields(quint8 nrOfDevFields)
{
    this->nrOfDevFields = nrOfDevFields;
}

quint8 CFitDefinitionMessage::getArchitectureBit() const
{
    return architecture & fitArchitecureEndianMask;
}

void CFitDefinitionMessage::addField(CFitFieldDefinition fieldDef)
{
    fields.append(fieldDef);
}

void CFitDefinitionMessage::addDevField(CFitFieldDefinition fieldDef)
{
    devFields.append(fieldDef);
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

const CFitFieldDefinition& CFitDefinitionMessage::getField(const quint8 fieldNum) const
{
    for (int i=0; i< fields.size(); i++)
    {
        if (fieldNum == fields[i].getDefNr())
        {
            return fields[i];
        }
    }
    // dummy field for unknown field nr.
    static const CFitFieldDefinition dummyDefinitionField;

    return dummyDefinitionField;
}


const CFitFieldDefinition& CFitDefinitionMessage::getFieldByIndex(const quint16 index) const
{
    if (index < fields.size())
    {
        return fields[index];
    }
    // dummy field for unknown field nr.
    static const CFitFieldDefinition dummyDefinitionField;

    return dummyDefinitionField;
}

const CFitFieldDefinition& CFitDefinitionMessage::getDevFieldByIndex(const quint16 index) const
{
    if (index < devFields.size())
    {
        return devFields[index];
    }
    // dummy field for unknown field nr.
    static const CFitFieldDefinition dummyDefinitionDevField;

    return dummyDefinitionDevField;
}


QStringList CFitDefinitionMessage::messageInfo() const
{
    QStringList list;
    list << QString("Definition %1 (%2) local nr %3, arch %4, # fields %5, # dev fields %6")
        .arg(profile().getName())
        .arg(getGlobalMesgNr())
        .arg(getLocalMesgNr())
        .arg(getArchitectureBit())
        .arg(getNrOfFields())
        .arg(getNrOfDevFields());

    for(const CFitFieldDefinition& field: fields)
    {
        list << field.fieldInfo();
    }

    for(const CFitFieldDefinition& devField: devFields)
    {
        list << devField.fieldInfo();
    }

    return list;
}



