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

#include "gis/fit/decoder/CFitDevFieldDefinitionState.h"
#include "gis/fit/defs/fit_const.h"
#include "gis/fit/defs/CFitBaseType.h"

/**
 * byte
 * 0: field number
 * 1: size in bytes of field data
 * 2: developer data index (maps to developer data id message)
 */

void CFitDevFieldDefinitionState::reset()
{
    offset = 0;
}

decode_state_e CFitDevFieldDefinitionState::process(quint8 &dataByte)
{
    switch (offset++)
    {
    case 0:
        // field number
        fieldNr = dataByte;
        break;

    case 1:
        // field data size
        size = dataByte;
        break;

    case 2:
        // field developer data index
        devDataIndex = dataByte;
        // get the previously (in RecordHeaderState) added definition message
        CFitDefinitionMessage* def = latestDefinition();
        CFitFieldProfile* profile = devFieldProfile(fieldNr);

        // add the new field definition
        def->addDevField(CFitFieldDefinition(def, profile, fieldNr, size, profile->getBaseType().baseTypeField()));
        reset();
        if (def->getDevFields().size() >= def->getNrOfDevFields())
        {
            FITDEBUG(2, qDebug() << latestDefinition()->messageInfo())
            endDefinition();
            return eDecoderStateRecord;
        }
    }

    return eDecoderStateDevFieldDef;
}
