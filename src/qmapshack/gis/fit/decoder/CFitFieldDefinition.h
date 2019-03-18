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

#include <QtCore>

class CFitDefinitionMessage;
class CFitBaseType;
class CFitFieldProfile;


class CFitFieldDefinition final
{
public:
    CFitFieldDefinition(CFitDefinitionMessage* parent, CFitFieldProfile* fieldProfile, quint8 defNr, quint8 size, quint8 type);
    CFitFieldDefinition(CFitDefinitionMessage* parent, quint8 defNr, quint8 size, quint8 type);
    CFitFieldDefinition();

    QString fieldInfo() const;

    quint8 getDefNr() const;
    quint8 getSize() const;
    quint8 getType() const;
    const CFitBaseType& getBaseType() const;
    bool getEndianAbilityFlag() const;

    const CFitDefinitionMessage& parent() const { return *parentDefintion; }
    const CFitFieldProfile& profile() const { return *fieldProfile; }

    void setParent(CFitDefinitionMessage* parent) { parentDefintion = parent; }

private:
    quint8 defNr;
    quint8 size;
    quint8 type;
    CFitBaseType* baseType;
    const CFitDefinitionMessage* parentDefintion;
    const CFitFieldProfile* fieldProfile;
};

#endif // CFITFIELDDEFINITION_H
