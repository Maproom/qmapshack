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

#ifndef CFITFIELD_H
#define CFITFIELD_H

#include "gis/fit/decoder/CFitFieldDefinition.h"
#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/defs/CFitProfile.h"

#include <QtCore>


class CFitField final
{
public:
    CFitField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, QVariant value, bool valid);
    CFitField(quint16 globalMesgNr, quint8 fieldDefNr, const CFitFieldProfile* profile, QVariant value, bool valid);
    CFitField(const CFitField & copy);
    CFitField();
    virtual ~CFitField() { /* nothing to do here, profile and base type are global and not to delete */ }

    void setProfile(const CFitFieldProfile* profile);
    QString fieldInfo() const;

    const CFitBaseType& getBaseType() const { return *baseType; }
    quint16 getGlobalMesgNr() const { return globalMesgNr; }
    quint8 getFieldDefNr() const { return fieldDefNr; }
    const CFitFieldProfile& profile() const { return *fieldProfile; }

    bool isValidValue() const { return valid; }
    const QVariant& getValue() const {  return value; }


private:
    void applyScaleAndOffset();

    const CFitFieldProfile* fieldProfile;
    quint16 globalMesgNr;
    quint8 fieldDefNr;
    const CFitBaseType* baseType;
    bool valid;
    QVariant value;
    QVariant rawValue;
};


#endif // CFITFIELD_H
