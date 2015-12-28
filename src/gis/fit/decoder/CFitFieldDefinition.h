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

#ifndef CFITFIELDDEFINITION_H
#define CFITFIELDDEFINITION_H

#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/defs/CFitProfile.h"

#include <QtCore>

// forwar declaration to parent
class CFitDefinitionMessage;

class CFitFieldDefinition {
public:
    CFitFieldDefinition(CFitDefinitionMessage* parent, uint8_t defNr, uint8_t size, uint8_t type);
    CFitFieldDefinition();

    QString fieldInfo() const;

    uint8_t getDefNr() const;
    uint8_t getSize() const;
    uint8_t getType() const;
    CFitBaseType* getBaseType() const;
    bool getEndianAbilityFlag() const;

    CFitDefinitionMessage* parent() const { return parentDefintion; }
    CFitFieldProfile* profile() const { return fieldProfile; }

    void setParent(CFitDefinitionMessage* parent) { parentDefintion = parent; }

private:
    uint8_t defNr;
    uint8_t size;
    uint8_t type;
    CFitBaseType* baseType;
    CFitDefinitionMessage* parentDefintion;
    CFitFieldProfile* fieldProfile;
};

#endif // CFITFIELDDEFINITION_H
