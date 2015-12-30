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


#ifndef CFITFIELDBUILDER_H
#define CFITFIELDBUILDER_H


#include "gis/fit/decoder/IFitField.h"
#include "gis/fit/decoder/CFitMessage.h"

class CFitFieldBuilder
{
public:
    static void evaluateSubfieldsAndExpandComponents(CFitMessage& mesg);
    static IFitField * buildField(const CFitFieldDefinition& def, uint8_t* fieldData, const CFitMessage& message);

private:
    static IFitField * buildSIntField(const CFitFieldDefinition& def, const CFitFieldProfile* fieldProfile, uint8_t* fieldData);
    static IFitField * buildUIntField(const CFitFieldDefinition& def, const CFitFieldProfile* fieldProfile, uint8_t* fieldData);
    static IFitField * buildFloatField(const CFitFieldDefinition& def, const CFitFieldProfile* fieldProfile, uint8_t* fieldData);
    static IFitField * buildStringField(const CFitFieldDefinition& def, const CFitFieldProfile* fieldProfile, uint8_t* fieldData);
    static IFitField * buildByteField(const CFitFieldDefinition& def, const CFitFieldProfile* fieldProfile, uint8_t* fieldData);

    static void evaluateFieldProfile(CFitMessage& mesg, IFitField & field);
    static void expandComponents(CFitMessage& mesg, const IFitField & field);
};

#endif //CFITFIELDBUILDER_H
