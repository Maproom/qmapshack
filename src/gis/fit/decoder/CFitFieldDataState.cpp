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

#include "gis/fit/decoder/CFitFieldDataState.h"
#include "gis/fit/decoder/CFitFieldBuilder.h"
#include "gis/fit/defs/fit_fields.h"


void CFitFieldDataState::reset()
{
    fieldDataIndex = 0;
    fieldIndex = 0;
}


decode_state_e CFitFieldDataState::process(uint8_t &dataByte) {
    CFitMessage& mesg = *latestMessage();
    const CFitFieldDefinition& fieldDef = defintion(mesg.getLocalMesgNr())->getFieldByIndex(fieldIndex);

    // add the read byte to the data array
    fieldData[fieldDataIndex++] = dataByte;

    if (fieldDataIndex >= fieldDef.getSize()) {
        // all bytes are read for current field

        // new field with data
        CFitField* f = CFitFieldBuilder::buildField(fieldDef, fieldData, mesg);
        mesg.addField(f);

        // The special case time record.
        // timestamp has always the same value for all enums. it does not matter againts which we're comparing.
        if (fieldDef.getDefNr() == eRecordTimestamp) {
            setTimestamp(f->getUIntValue());
        }

        // new field follows, reset
        fieldDataIndex = 0;
        fieldIndex++;
    }

    if (fieldIndex >= defintion(mesg.getLocalMesgNr())->getNrOfFields()) {
        // Now that the entire message is decoded we may evaluate subfields and expand components
        CFitFieldBuilder::evaluateSubfieldsAndExpandComponents(mesg);

        reset();
        FITDEBUG(2, qDebug() << mesg.messageInfo())
        // after all fields read, go to next record header
        return eDecoderStateRecord;
    }

    // there are more fields to read for the current message
    return eDecoderStateFieldData;
};
