/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg <>

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
#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/defs/fit_fields.h"

/*
 * normal header
 * bit: value description
 * 7: 0 normal Header
 * 6: 0/1 data message / definition message
 * 5: 0/1 developer data flag
 * 4: - reserved
 * 3: 0/1 local message type (0-15 -> 0000 - 1111)
 * 2:     local message type
 * 1:     local message type
 * 0:     local message type
 */
static const quint8 fitRecordHeaderDefBit =  ((quint8) 0x40); // bit 6: 0100 0000
static const quint8 fitRecordHeaderDevBit = ((quint8) 0x20); // bit 5: 0010 0000
static const quint8 fitRecordHeaderMesgMask = ((quint8) 0x0F); // bit 0-3: 0000 1111

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
static const quint8 fitRecordHeaderTypeBit =  ((quint8) 0x80); // bit 7: 1000 0000
static const quint8 fitRecordHeaderTimeMesgMask =  ((quint8) 0x60); // bit 5-6: 0110 0000
static const quint8 fitRecordHeaderTimeMesgShift = 5;


decode_state_e CFitRecordHeaderState::process(quint8 &dataByte)
{
    if ((dataByte & fitRecordHeaderTypeBit) != 0)
    {
        // this is a compressed timestamp header
        quint8 localMessageType = (dataByte & fitRecordHeaderTimeMesgMask) >> fitRecordHeaderTimeMesgShift;
        CFitDefinitionMessage* def = definition(localMessageType);

        if (!def->hasField(eRecordTimestamp))
        {
            // create dummy definition field for timestamp
            // later on passed timestamp is a uint32, therefore a 4 byte type is created.
            // remark on enum for timestamp (RecordTimestamp):
            // the timestamp field has for all message types the same number (253) therefore it does not matter which
            // enum is taken here.
            def->addField(CFitFieldDefinition(def, eRecordTimestamp, sizeof(quint32), eBaseTypeNrUint8));
        }
        setTimestampOffset(dataByte);

        addMessage(*def);
        const CFitFieldDefinition& fieldDef = def->getField(eRecordTimestamp);
        CFitField timeField = CFitField(fieldDef, &fieldDef.profile(), QVariant(getTimestamp()), true);
        latestMessage()->addField(timeField);

        return eDecoderStateFieldData;
    }
    else
    {
        quint8 localMessageType = dataByte & fitRecordHeaderMesgMask;
        if ((dataByte & fitRecordHeaderDefBit) != 0)
        {
            // this is a definition message
            bool developerDataFlag = dataByte & fitRecordHeaderDevBit;
            addDefinition(CFitDefinitionMessage(localMessageType, developerDataFlag));
            return eDecoderStateRecordContent;
        }
        else
        {
            // this is a data message
            addMessage(*definition(localMessageType));
            // go to eDecoderStateFieldData
            return eDecoderStateFieldData;
        }
    }
}
