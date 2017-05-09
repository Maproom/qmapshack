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

#ifndef CFITMESSAGE_H
#define CFITMESSAGE_H

#include "gis/fit/decoder/CFitField.h"

#include <QtCore>

class CFitDefinitionMessage;
class CFitProfile;
class CFitFieldProfile;

class CFitMessage final
{
public:
    CFitMessage(const CFitDefinitionMessage& def);
    CFitMessage();

    bool isValid() const;
    quint16 getGlobalMesgNr() const { return globalMesgNr; }
    quint8 getLocalMesgNr()   const { return localMesgNr; }

    bool hasField(const quint8 fieldDefNum) const;

    bool isFieldValueValid(const quint8 fieldDefNum) const;
    const QVariant getFieldValue(const quint8 fieldDefNum) const;
    void addField(CFitField & field);

    const CFitProfile& profile() const { return *messageProfile; }
    QStringList messageInfo() const;
    const QList<CFitField> getFields() const { return fields.values(); }
    void updateFieldProfile(quint8 fieldDefNr, const CFitFieldProfile* fieldProfile);

private:
    QMap<quint8, CFitField> fields;
    QMap<quint8, CFitField> devFields;
    quint16 globalMesgNr;
    quint8 localMesgNr;
    const CFitProfile* messageProfile;
};

#endif //CFITMESSAGE_H
