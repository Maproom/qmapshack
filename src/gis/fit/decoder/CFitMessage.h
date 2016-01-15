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

#include "gis/fit/decoder/CFitDefinitionMessage.h"
#include "gis/fit/decoder/IFitField.h"

#include <QtCore>

class MesgDefinition;   // Forward declaration.

class CFitMessage final
{
public:
    CFitMessage(const CFitDefinitionMessage& def);
    CFitMessage();
    virtual ~CFitMessage();

    bool isValid() const;
    quint16 getGlobalMesgNr() const;
    quint8 getLocalMesgNr() const;

    bool hasField(const quint8 fieldDefNum) const;
    const IFitField & getField(const quint8 fieldDefNum) const;
    bool isFieldValueValid(const quint8 fieldDefNum) const;
    void addField(IFitField * field);

    QString getFieldString(const quint8 fieldDefNum) const;
    QByteArray getFieldBytes(const quint8 fieldDefNum) const;
    int getFieldIntValue(const quint8 fieldDefNum) const;
    unsigned int getFieldUIntValue(const quint8 fieldDefNum) const;
    qreal getFieldDoubleValue(const quint8 fieldDefNum) const;

    const CFitProfile& profile() const { return *messageProfile; }
    QStringList messageInfo() const;
    const QList<IFitField *> getFields() const { return fields.values(); }


private:
    QMap<quint8, IFitField *> fields;
    quint16 globalMesgNr;
    quint8 localMesgNr;
    const CFitProfile* messageProfile;
};


#endif //CFITMESSAGE_H
