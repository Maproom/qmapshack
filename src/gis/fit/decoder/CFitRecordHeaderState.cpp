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

#include "gis/fit/decoder/CFitRecordHeaderState.h"
#include "gis/fit/defs/fit_fields.h"

/*
 * normal header
 * bit: value description
 * 7: 0 normal Header
 * 6: 0/1 data message / definition message
 * 5: - reserved
 * 4: - reserved
 * 3: 0/1 local message type (0-15 -> 0000 - 1111)
 * 2:     local message type
 * 1:     local message type
 * 0:     local message type
 */
static const uint8_t RecordHeaderDefBit =  ((uint8_t) 0x40); // bit 6: 0100 0000
static const uint8_t RecordHeaderMesgMask = ((uint8_t) 0x0F); // bit 0-3: 0000 1111

/*
 * compressed timestamp header
 * bit: value description
 * 7: 1 compressed timestamp header
 * 6: 0/1 local message type
 * 5:     local message type
 * 4: 0/1 time offset (seconds) (0-31 -> 0 0000-1 1111)
 * 3:     time offset
 * 2:     time offset
 * 1:     time offset
 * 0:     time offset
 */
static const uint8_t RecordHeaderTypeBit =  ((uint8_t) 0x80); // bit 7: 1000 0000
static const uint8_t RecordHeaderTimeMesgMask =  ((uint8_t) 0x60); // bit 5-6: 0110 0000
static const uint8_t RecordHeaderTimeMesgShift = 5;


decode_state_e CFitRecordHeaderState::process(uint8_t &dataByte)
{
    if ((dataByte & RecordHeaderTypeBit) != 0)
    {
        // this is a compressed timestamp header
        uint8_t localMessageType = (dataByte & RecordHeaderTimeMesgMask) >> RecordHeaderTimeMesgShift;
        CFitDefinitionMessage* def = defintion(localMessageType);

        if (!def->hasField(eRecordTimestamp))
        {
            // create dummy definition field for timestamp
            // later on passed timestamp is a uint32, therefore a 4 byte type is created.
            // remark on enum for timestamp (RecordTimestamp:
            // the timestamp field has for all message types the same number (253) therefore it does not matter which
            // enum is taken here.
            def->addField(CFitFieldDefinition(def, eRecordTimestamp, sizeof(uint32_t), TypeUint8));
        }
        setTimestampOffset(dataByte);

        addMessage(*def);
        const CFitFieldDefinition& fieldDef = def->getField(eRecordTimestamp);
        latestMessage()->addField(new CFitIntField<uint32_t>(fieldDef, &fieldDef.profile(), getTimestamp(), true));

        return eDecoderStateFieldData;
    }
    else
    {
        uint8_t localMessageType = dataByte & RecordHeaderMesgMask;
        if ((dataByte & RecordHeaderDefBit) != 0)
        {
            // this is a definition message
            addDefinition(CFitDefinitionMessage(localMessageType));
            return eDecoderStateRecordContent;
        }
        else
        {
            // this is a data message
            addMessage(*defintion(localMessageType));
            // go to eDecoderStateFieldData
            return eDecoderStateFieldData;
        }
    }
}