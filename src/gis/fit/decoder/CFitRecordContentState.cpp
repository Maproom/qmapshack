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

#include "gis/fit/decoder/CFitRecordContentState.h"
#include "gis/fit/defs/fit_const.h"


/**
 * record content (without field definitions)
 * 0: reserved
 * 1: architecture (0 little, 1 big endian)
 * 2: global message number
 * 3: global message number
 * 4: number of fields in the data message
 */
void CFitRecordContentState::reset()
{
    offset = 0;
};

decode_state_e CFitRecordContentState::process(uint8_t &dataByte)
{
    CFitDefinitionMessage* def = latestDefinition();
    switch (offset++)
    {
    case 0:
        // reserved
        break;

    case 1:
        // architecture
        architecture = dataByte;
        def->setArchiteture(architecture);
        break;

    case 2:
        // global message number
        globalMesgNr = dataByte;
        break;

    case 3:
        // global message number
        globalMesgNr = globalMesgNr | ((uint16_t) dataByte << 8);

        if (architecture == ArchEndianBig)
        {
            globalMesgNr = (globalMesgNr >> 8) | ((globalMesgNr & 0xFF) << 8);
        }
        else if (architecture != ArchEndianLittle)
        {
            throw QString("Architecture %1 not supported.").arg(architecture);
        }
        def->setGlobalMesgNr(globalMesgNr);
        break;

    case 4:
        // number of fields
        nrOfFields = dataByte;
        def->setNrOfFields(nrOfFields);

        reset();
        if (nrOfFields == 0)
        {
            // no fields, records may follow (either for a data message or defninition message)
            return eDecoderStateRecord;
        }
        // the fields definitions follows
        return eDecoderStateFieldDef;
        break;

    default:
        throw QString("invalid offset (%1) for state 'record content'").arg(offset);
    }

    // still more bytes for the record content
    return eDecoderStateRecordContent;
};