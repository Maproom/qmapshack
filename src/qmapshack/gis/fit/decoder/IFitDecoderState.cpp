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

#include "gis/fit/decoder/IFitDecoderState.h"

decode_state_e IFitDecoderState::processByte(quint8 &dataByte)
{
    incFileBytesRead();
    buildCrc(dataByte);
    decode_state_e state = process(dataByte);
    if (bytesLeftToRead() == 2)
    {
        if (state != eDecoderStateRecord)
        {
            // we come from a wrong state...
            throw tr("FIT decoding error: Decoder not in correct state %1 after last data byte in file.").arg(state);
        }
        // end of file, 2 bytes left, this is the crc
        return eDecoderStateFileCrc;
    }
    return state;
}


void IFitDecoderState::buildCrc(quint8 byte)
{
    static const quint16 crc_table[16] =
    {
        0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
        0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
    };
    quint16 tmp;
    quint16 crc = data.crc;
    // compute checksum of lower four bits of byte
    tmp = crc_table[crc & 0xF];
    crc = (crc >> 4) & 0x0FFF;
    crc = crc ^ tmp ^ crc_table[byte & 0xF];
    // now compute checksum of upper four bits of byte
    tmp = crc_table[crc & 0xF];
    crc = (crc >> 4) & 0x0FFF;
    crc = crc ^ tmp ^ crc_table[(byte >> 4) & 0xF];
    data.crc = crc;
}

static const quint8 fitRecordHeaderTimeOffsetMask = 0x1F; // bit 0-4: 0001 1111

void IFitDecoderState::setTimestamp(quint32 fullTimestamp)
{
    data.timestamp = fullTimestamp;
    data.lastTimeOffset = (quint8) (data.timestamp & fitRecordHeaderTimeOffsetMask);
}

void IFitDecoderState::setTimestampOffset(quint32 offsetTimestamp)
{
    quint8 timeOffset = offsetTimestamp & fitRecordHeaderTimeOffsetMask;
    data.timestamp += (timeOffset - data.lastTimeOffset) & fitRecordHeaderTimeOffsetMask;
    data.lastTimeOffset = timeOffset;
}

void IFitDecoderState::addMessage(const CFitDefinitionMessage& definition)
{
    data.messages.append(CFitMessage(definition));
    data.lastMessage = &data.messages.last();
}

void IFitDecoderState::addDefinition(const CFitDefinitionMessage &definition)
{
    data.definitions[definition.getLocalMesgNr()] = definition;
    data.lastDefinition = &data.definitions[definition.getLocalMesgNr()];
}

void IFitDecoderState::endDefinition()
{
    data.definitionHistory.append(*data.lastDefinition);
}

CFitDefinitionMessage*IFitDecoderState::definition(quint32 localMessageType)
{
    return &(data.definitions[localMessageType]);
}

void IFitDecoderState::setFileLength(quint32 fileLength)
{
    data.fileLength = fileLength;
}

quint32 IFitDecoderState::bytesLeftToRead()
{
    return data.fileLength - data.fileBytesRead;
}


void IFitDecoderState::resetFileBytesRead()
{
    data.fileBytesRead = 0;
}

void IFitDecoderState::incFileBytesRead()
{
    data.fileBytesRead++;
}

void IFitDecoderState::addDevFieldProfile(const CFitFieldProfile &fieldProfile)
{
    // for documentation: a development field definition is linked to an developer data ID. Only the tuple developer data index
    // and field definition number must be unique. So far no fit file with more than one developer data ID has been created.
    if(devFieldProfile(fieldProfile.getFieldDefNum())->getFieldDefNum() ==  fieldProfile.getFieldDefNum())
    {
        throw tr("FIT decoding error: a development field with the field_definition_number %1 already exists.")
              .arg(fieldProfile.getFieldDefNum());
    }
    data.devFieldProfiles.append(fieldProfile);
}

CFitFieldProfile* IFitDecoderState::devFieldProfile(quint32 fieldNr)
{
    for (int i = 0; i < data.devFieldProfiles.size(); i++)
    {
        if (fieldNr == data.devFieldProfiles[i].getFieldDefNum())
        {
            return &data.devFieldProfiles[i];
        }
    }
    // dummy field for unknown field nr.
    static CFitFieldProfile dummyFieldProfile;
    return &dummyFieldProfile;

    //return data.devFieldProfiles[fieldNr];
}

void IFitDecoderState::clearDevFieldProfiles()
{
    data.devFieldProfiles.clear();
}
