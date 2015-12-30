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

#ifndef CFITSTATE_H
#define CFITSTATE_H

#include "gis/fit/decoder/CFitDefinitionMessage.h"
#include "gis/fit/decoder/CFitMessage.h"

#include <QtCore>


typedef enum
{
    eDecoderStateFileHeader,
    eDecoderStateRecord,
    eDecoderStateRecordContent,
    eDecoderStateFieldDef,
    eDecoderStateFieldData,
    eDecoderStateFileCrc,
    eDecoderStateEnd
} decode_state_e;

class IFitDecoderState
{
public:
    struct shared_state_data_t
    {
        uint16_t crc;
        uint32_t fileLength;
        uint32_t fileBytesRead;
        uint8_t lastTimeOffset;
        uint32_t timestamp;
        CFitDefinitionMessage* lastDefintion;
        CFitMessage* lastMessage;
        QMap<uint8_t, CFitDefinitionMessage> defintions;
        QList<CFitDefinitionMessage> defintionHistory;
        QList<CFitMessage> messages;
    };

    IFitDecoderState(shared_state_data_t &data, decode_state_e stateClass) : data(data) { };
    virtual ~IFitDecoderState() {};

    virtual void reset() = 0;
    decode_state_e processByte(uint8_t &dataByte);

protected:
    virtual decode_state_e process(uint8_t &dataByte) = 0;

    CFitMessage* latestMessage();
    void addMessage(const CFitDefinitionMessage& definition);

    void setFileLength(uint32_t fileLength);
    void resetFileBytesRead();
    void incFileBytesRead();
    uint32_t bytesLeftToRead();

    CFitDefinitionMessage* latestDefinition();
    CFitDefinitionMessage* defintion(uint32_t localMessageType);
    void addDefinition(CFitDefinitionMessage definition);
    void endDefintion();

    void setTimestamp(uint32_t fullTimestamp);
    void setTimestampOffset(uint32_t offsetTimestamp);
    uint32_t getTimestamp();
    uint16_t getCrc();

private:
    void buildCrc(uint8_t byte);

    shared_state_data_t &data;
};

#endif // CFITSTATE_H
