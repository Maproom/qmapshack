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
    CFitMessage(CFitDefinitionMessage* def);
    CFitMessage();
    //CFitMessage(const CFitMessage &mesg);

    CFitDefinitionMessage* getDefinition() const;
    uint16_t getGlobalMesgNr() const;
    uint8_t getLocalMesgNr() const;

    bool hasField(const uint8_t fieldDefNum);
    CFitField* getField(const uint8_t fieldDefNum);
    bool isFieldValueValid(const uint8_t fieldDefNum);
    void addField(CFitField* field);

    QString getFieldString(const uint8_t fieldDefNum);
    uint8_t* getFieldBytes(const uint8_t fieldDefNum);
    int getFieldIntValue(const uint8_t fieldDefNum);
    unsigned int getFieldUIntValue(const uint8_t fieldDefNum);
    double getFieldDoubleValue(const uint8_t fieldDefNum);

    CFitProfile* profile() { return messageProfile; }
    QString messageInfo();


private:
    QMap<uint8_t, CFitField*> fields;
    uint16_t globalMesgNr;
    uint8_t localMesgNr;
    CFitProfile* messageProfile;
};


#endif //CFITMESSAGE_H
