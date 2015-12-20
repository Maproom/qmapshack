/**********************************************************************************************
 Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/fit/decoder/CFitMessage.h"
#include "gis/fit/decoder/CFitDefinitionMessage.h"

#include <QtCore>


typedef enum {
    StateFileHeader, // 0
    StateRecord, // 1
    StateRecordContent, // 2
    StateFieldDef, // 3
    StateFieldData, // 4
    StateFileCrc, // 5
    //StateError, // 6
    StateEnd // 7
} DecodeState;

class CFitDecoderState {
public:
    struct shared_state_data
    {
        uint16_t crc;
        uint32_t fileLength;
        uint32_t fileBytesRead;
        uint8_t lastTimeOffset;
        uint32_t timestamp;
        CFitDefinitionMessage* lastDefintion;
        CFitMessage* lastMessage;
        QMap<uint8_t, CFitDefinitionMessage*> defintions;
        QList<CFitMessage*> messages;
    };

    CFitDecoderState(shared_state_data &data, DecodeState stateClass) : data(data) { };
    virtual ~CFitDecoderState() {};

    virtual void reset() = 0;
    DecodeState processByte(uint8_t &dataByte);

protected:
    virtual DecodeState process(uint8_t &dataByte) = 0;

    CFitMessage* latestMessage();
    void addMessage(CFitDefinitionMessage* definition);

    void setFileLength(uint32_t fileLength);
    void resetFileBytesRead();
    void incFileBytesRead();
    uint32_t bytesLeftToRead();

    CFitDefinitionMessage* latestDefinition();
    CFitDefinitionMessage* defintion(uint32_t localMessageType);
    void addDefinition(CFitDefinitionMessage* definition);

    void setTimestamp(uint32_t fullTimestamp);
    void setTimestampOffset(uint32_t offsetTimestamp);
    uint32_t getTimestamp();
    uint16_t getCrc();

private:
    void buildCrc(uint8_t byte);

    shared_state_data &data;
};




#endif // CFITSTATE_H
