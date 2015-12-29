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


#ifndef CFITPROFILELOCKUP_H
#define CFITPROFILELOCKUP_H

#include "gis/fit/defs/CFitProfile.h"

class CFitProfileLockup
{
public:
    static const CFitProfile* getProfile(uint16_t globalMesgNr);
    static const CFitFieldProfile* getFieldForProfile(uint16_t globalMesgNr, uint8_t fieldDefNr);
};

#endif // CFITPROFILELOCKUP_H
