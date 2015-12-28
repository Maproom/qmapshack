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


#ifndef CFITMESSAGEDEFINTION_H
#define CFITMESSAGEDEFINTION_H

#include "gis/fit/decoder/CFitFieldDefinition.h"
#include "gis/fit/defs/CFitProfile.h"

#include <QtCore>


class CFitDefinitionMessage {
public:
    CFitDefinitionMessage(uint8_t localMesgNr);

    CFitDefinitionMessage();
    CFitDefinitionMessage(const CFitDefinitionMessage& copy);

    void setArchiteture(uint8_t arch);
    void setGlobalMesgNr(uint16_t globalMesgNr);
    void setNrOfFields(uint8_t nrOfFields);

    uint16_t getGlobalMesgNr() const;
    uint8_t getArchitectureBit() const;
    uint8_t getNrOfFields() const;
    uint8_t getLocalMesgNr() const;

    const CFitProfile* profile() const { return messageProfile; }

    const QList<CFitFieldDefinition>& getFields() const;
    void addField(CFitFieldDefinition field);
    bool hasField(const uint8_t fieldNum) const;
    CFitFieldDefinition& getField(const uint8_t fieldNum);
    CFitFieldDefinition& getFieldByIndex(const uint16_t index);

    QStringList messageInfo() const;


private:
    uint16_t globalMesgNr;
    uint8_t architecture;
    uint8_t nrOfFields;
    uint8_t localMesgNr;
    QList<CFitFieldDefinition> fields;
    CFitProfile* messageProfile;
};


#endif // CFITMESSAGEDEFINTION_H
