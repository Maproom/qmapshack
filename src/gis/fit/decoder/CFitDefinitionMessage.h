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

#include <QtCore>

class CFitBaseType;
class CFitProfile;

class CFitDefinitionMessage final
{
public:
    CFitDefinitionMessage(quint8 localMesgNr);

    CFitDefinitionMessage();
    CFitDefinitionMessage(const CFitDefinitionMessage& copy);

    void setArchiteture(quint8 arch);
    void setGlobalMesgNr(quint16 globalMesgNr);
    void setNrOfFields(quint8 nrOfFields);

    quint8 getArchitectureBit() const;
    quint16 getGlobalMesgNr()   const { return globalMesgNr; }
    quint8 getNrOfFields()      const { return nrOfFields;   }
    quint8 getLocalMesgNr()     const { return localMesgNr;  }

    const CFitProfile& profile() const { return *messageProfile; }

    const QList<CFitFieldDefinition>& getFields() const { return fields; }

    void addField(CFitFieldDefinition field);
    bool hasField(const quint8 fieldNum) const;
    const CFitFieldDefinition& getField(const quint8 fieldNum) const;
    const CFitFieldDefinition& getFieldByIndex(const quint16 index) const;

    QStringList messageInfo() const;


private:
    quint16 globalMesgNr;
    quint8 architecture;
    quint8 nrOfFields;
    quint8 localMesgNr;
    QList<CFitFieldDefinition> fields;
    const CFitProfile* messageProfile;
};


#endif // CFITMESSAGEDEFINTION_H
