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

#include "gis/fit/decoder/CFitMessage.h"

#include "gis/fit/defs/CFitProfileLockup.h"
#include "gis/fit/defs/fit_const.h"


CFitMessage::CFitMessage(const CFitDefinitionMessage& def) : fields()
{
    localMesgNr = def.getLocalMesgNr();
    globalMesgNr = def.getGlobalMesgNr();
    messageProfile = CFitProfileLockup::getProfile(globalMesgNr);
}

CFitMessage::CFitMessage() : fields()  {
    localMesgNr = LocalMesgNrInvalid;
    globalMesgNr = GlobalMesgNrInvalid;
    messageProfile = CFitProfileLockup::getProfile(globalMesgNr);
}

CFitMessage::~CFitMessage()
{
    for(CFitField* field : fields)
    {
        delete field;
    }
}

bool CFitMessage::isValid() const
{
    return getGlobalMesgNr() != GlobalMesgNrInvalid;
}

QStringList CFitMessage::messageInfo() const
{
    QStringList list;
    list << QString("Message %1 (%3) %4 [loc]")
    .arg(messageProfile->getName())
    .arg(getGlobalMesgNr())
    .arg(getLocalMesgNr());

    for(const CFitField* field: fields) {
        list << field->fieldInfo();
    }
    return list;
}
uint16_t CFitMessage::getGlobalMesgNr() const
{
    return globalMesgNr;
}

uint8_t CFitMessage::getLocalMesgNr() const
{
    return localMesgNr;
}

bool CFitMessage::hasField(const uint8_t fieldDefNum) const
{
    return fields.contains(fieldDefNum);
}

// dummy field for unknown field defintion nr.
// lazy initialized because profiles are not created at that point
CFitField* dummyField = nullptr;
CFitField* invalidField()
{
    if(!dummyField)
    {
        dummyField = new CFitField();
    }
    return dummyField;
}

const CFitField* CFitMessage::getField(const uint8_t fieldDefNum) const
{
    if(hasField(fieldDefNum))
        return fields[fieldDefNum];
    return invalidField();
}

void CFitMessage::addField(CFitField* field)
{
    fields.insert(field->getFieldDefNr(), field);
}

bool CFitMessage::isFieldValueValid(const uint8_t fieldDefNum) const
{
    return getField(fieldDefNum)->isValidValue();
}

int CFitMessage::getFieldIntValue(const uint8_t fieldDefNum) const
{
    return getField(fieldDefNum)->getSIntValue();
}
unsigned int CFitMessage::getFieldUIntValue(const uint8_t fieldDefNum) const
{
        return getField(fieldDefNum)->getUIntValue();
}
double CFitMessage::getFieldDoubleValue(const uint8_t fieldDefNum) const
{
    return getField(fieldDefNum)->getDoubleValue();
}

QString CFitMessage::getFieldString(const uint8_t fieldDefNum) const
{
    return getField(fieldDefNum)->getString();
}
QByteArray CFitMessage::getFieldBytes(const uint8_t fieldDefNum) const
{
    return getField(fieldDefNum)->getBytes();
}
