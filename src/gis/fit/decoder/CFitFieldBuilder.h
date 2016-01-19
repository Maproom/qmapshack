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


#include "gis/fit/decoder/CFitField.h"
#include "gis/fit/decoder/CFitMessage.h"

class CFitFieldBuilder
{
public:
    CFitFieldBuilder() = delete;
    static void evaluateSubfieldsAndExpandComponents(CFitMessage& mesg);
    static CFitField buildField(const CFitFieldDefinition& def, quint8* fieldData, const CFitMessage& message);

private:
    static bool isValueValid(const CFitFieldDefinition &def, quint8 *fieldData);
    static void evaluateFieldProfile(CFitMessage& mesg, const CFitField & field);
    static void expandComponents(CFitMessage& mesg, const CFitField & field);
};

#endif //CFITFIELDBUILDER_H
