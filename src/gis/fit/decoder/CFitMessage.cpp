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
#include "gis/fit/decoder/CFitMessage.h"
#include "gis/fit/defs/CFitProfile.h"
#include "gis/fit/defs/CFitProfileLookup.h"
#include "gis/fit/defs/fit_const.h"

CFitMessage::CFitMessage(const CFitDefinitionMessage& def)
    : fields(), globalMesgNr(def.getGlobalMesgNr()), localMesgNr(def.getLocalMesgNr()),
    messageProfile(CFitProfileLookup::getProfile(globalMesgNr))
{
}

CFitMessage::CFitMessage()
    : fields(), globalMesgNr(fitGlobalMesgNrInvalid), localMesgNr(fitLocalMesgNrInvalid),
    messageProfile(CFitProfileLookup::getProfile(fitGlobalMesgNrInvalid))
{
}

bool CFitMessage::isValid() const
{
    return getGlobalMesgNr() != fitGlobalMesgNrInvalid;
}

void CFitMessage::updateFieldProfile(quint8 fieldDefNr, const CFitFieldProfile* fieldProfile)
{
    fields[fieldDefNr].setProfile(fieldProfile);
}

QStringList CFitMessage::messageInfo() const
{
    QStringList list;
    list << QString("Message %1 (%3) %4 [loc]")
        .arg(profile().getName())
        .arg(getGlobalMesgNr())
        .arg(getLocalMesgNr());

    for(const CFitField &  field: fields)
    {
        list << field.fieldInfo();
    }
    return list;
}

quint16 CFitMessage::getGlobalMesgNr() const
{
    return globalMesgNr;
}

quint8 CFitMessage::getLocalMesgNr() const
{
    return localMesgNr;
}

bool CFitMessage::hasField(const quint8 fieldDefNum) const
{
    return fields.contains(fieldDefNum);
}

void CFitMessage::addField(CFitField &  field)
{
    if(fields.contains(field.getFieldDefNr()))
    {
        qCritical("fit field %d already added to map.", (int) field.getFieldDefNr());
    }
    fields.insert(field.getFieldDefNr(), field);
}

bool CFitMessage::isFieldValueValid(const quint8 fieldDefNum) const
{
    return fields[fieldDefNum].isValidValue();
}

const QVariant& CFitMessage::getFieldValue(const quint8 fieldDefNum) const
{
    return fields[fieldDefNum].getValue();
}
