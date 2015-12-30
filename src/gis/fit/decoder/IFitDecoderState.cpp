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

#include "gis/fit/decoder/IFitDecoderState.h"


decode_state_e IFitDecoderState::processByte(uint8_t &dataByte)
{
    incFileBytesRead();
    buildCrc(dataByte);
    decode_state_e state = process(dataByte);
    if (bytesLeftToRead() == 2)
    {
        if (state != eDecoderStateRecord)
        {
            // we come from a wrong state...
            throw QString("Decoder not in correct state after last data byte in file.");
        }
        // end of file, 2 bytes left, this is the crc
        return eDecoderStateFileCrc;
    }
    return state;
}


void IFitDecoderState::buildCrc(uint8_t byte)
{
    static const uint16_t crc_table[16] =
    {
        0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
        0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
    };
    uint16_t tmp;
    uint16_t crc = data.crc;
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


uint16_t IFitDecoderState::getCrc()
{
    return data.crc;
}

CFitMessage*IFitDecoderState::latestMessage()
{
    return data.lastMessage;
}

CFitDefinitionMessage*IFitDecoderState::latestDefinition()
{
    return data.lastDefintion;
}


static const uint8_t RecordHeaderTimeOffsetMask = 0x1F; // bit 0-4: 0001 1111

void IFitDecoderState::setTimestamp(uint32_t fullTimestamp)
{
    data.timestamp = fullTimestamp;
    data.lastTimeOffset = (uint8_t) (data.timestamp & RecordHeaderTimeOffsetMask);
}

void IFitDecoderState::setTimestampOffset(uint32_t offsetTimestamp)
{
    uint8_t timeOffset = offsetTimestamp & RecordHeaderTimeOffsetMask;
    data.timestamp += (timeOffset - data.lastTimeOffset) & RecordHeaderTimeOffsetMask;
    data.lastTimeOffset = timeOffset;
}

uint32_t IFitDecoderState::getTimestamp()
{
    return data.timestamp;
}

void IFitDecoderState::addMessage(const CFitDefinitionMessage& definition)
{
    data.messages.append(CFitMessage(definition));
    data.lastMessage = &data.messages.last();
}

void IFitDecoderState::addDefinition(CFitDefinitionMessage definition)
{
    data.defintions[definition.getLocalMesgNr()] = definition;
    data.lastDefintion = &data.defintions[definition.getLocalMesgNr()];
}

void IFitDecoderState::endDefintion()
{
    data.defintionHistory.append(*data.lastDefintion);
}

CFitDefinitionMessage*IFitDecoderState::defintion(uint32_t localMessageType)
{
    return &(data.defintions[localMessageType]);
}

void IFitDecoderState::setFileLength(uint32_t fileLength)
{
    data.fileLength = fileLength;
}

uint32_t IFitDecoderState::bytesLeftToRead()
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
