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

#ifndef CFITPROFILE_H
#define CFITPROFILE_H

#include "gis/fit/defs/CFitFieldProfile.h"

#include <QtCore>


class CFitProfile
{
public:
    CFitProfile();
    CFitProfile(const CFitProfile& copy);
    CFitProfile(QString name, uint16_t globalMesgNr);
    void addField(CFitFieldProfile* field);
    CFitFieldProfile * getField(uint8_t fieldDefNr);

    QString getName() const { return name; }
    uint16_t getGlobalMesgNr() const { return globalMesgNr; }

private:
    QString name;
    uint16_t globalMesgNr;
    QMap<uint8_t, CFitFieldProfile*> fields;
};

#endif // CFITPROFILE_H
