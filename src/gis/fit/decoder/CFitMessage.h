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

#ifndef CFITMESSAGE_H
#define CFITMESSAGE_H

#include "gis/fit/decoder/CFitField.h"
#include "gis/fit/decoder/CFitDefinitionMessage.h"

#include <QtCore>

class MesgDefinition;   // Forward declaration.

class CFitMessage {
public:
    CFitMessage(const CFitDefinitionMessage& def);
    CFitMessage();
    virtual ~CFitMessage();

    uint16_t getGlobalMesgNr() const;
    uint8_t getLocalMesgNr() const;

    bool hasField(const uint8_t fieldDefNum) const;
    const CFitField* getField(const uint8_t fieldDefNum) const;
    bool isFieldValueValid(const uint8_t fieldDefNum) const;
    void addField(CFitField* field);

    QString getFieldString(const uint8_t fieldDefNum) const;
    QByteArray getFieldBytes(const uint8_t fieldDefNum) const;
    int getFieldIntValue(const uint8_t fieldDefNum) const;
    unsigned int getFieldUIntValue(const uint8_t fieldDefNum) const;
    double getFieldDoubleValue(const uint8_t fieldDefNum) const;

    CFitProfile* profile() const { return messageProfile; }
    QStringList messageInfo() const;
    const QList<CFitField*> getFields() const { return fields.values(); }


private:
    QMap<uint8_t, CFitField*> fields;
    uint16_t globalMesgNr;
    uint8_t localMesgNr;
    CFitProfile* messageProfile;
};


#endif //CFITMESSAGE_H
