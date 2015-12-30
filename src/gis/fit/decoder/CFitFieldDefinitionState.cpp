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

#include "gis/fit/decoder/CFitFieldDefinitionState.h"


/**
 * byte
 * 0: field definition number
 * 1: size in bytes of field data
 * 2: base type
 *
 */

void CFitFieldDefinitionState::reset()
{
    offset = 0;
};

decode_state_e CFitFieldDefinitionState::process(uint8_t &dataByte)
{
    switch (offset++)
    {
    case 0:
        // field definition number
        defNr = dataByte;
        break;

    case 1:
        // field data size
        size = dataByte;
        break;

    case 2:
        // field base type
        type = dataByte;
        // get the previously (in RecordHeaderState) added definition message
        CFitDefinitionMessage* def = latestDefinition();
        // add the new field definition
        def->addField(CFitFieldDefinition(def, defNr, size, type));
        reset();
        if (def->getFields().size() >= def->getNrOfFields())
        {
            FITDEBUG(2, qDebug() << latestDefinition()->messageInfo())
            endDefintion();
            return eDecoderStateRecord;
        }
        else
        {
            return eDecoderStateFieldDef;
        }
    }

    return eDecoderStateFieldDef;
};