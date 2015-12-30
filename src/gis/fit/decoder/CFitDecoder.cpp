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

#include "gis/fit/decoder/CFitDecoder.h"

#include "gis/fit/decoder/CFitCrcState.h"
#include "gis/fit/decoder/CFitFieldDataState.h"
#include "gis/fit/decoder/CFitFieldDefinitionState.h"
#include "gis/fit/decoder/CFitHeaderState.h"
#include "gis/fit/decoder/CFitRecordContentState.h"
#include "gis/fit/decoder/CFitRecordHeaderState.h"


CFitDecoder::CFitDecoder()
{
    stateMap[eDecoderStateFileHeader] = new CFitHeaderState(data);
    stateMap[eDecoderStateRecord] = new CFitRecordHeaderState(data);
    stateMap[eDecoderStateRecordContent] = new CFitRecordContentState(data);
    stateMap[eDecoderStateFieldDef] = new CFitFieldDefinitionState(data);
    stateMap[eDecoderStateFieldData] = new CFitFieldDataState(data);
    stateMap[eDecoderStateFileCrc] = new CFitCrcState(data);
}

CFitDecoder::~CFitDecoder()
{
    for(CFitDecoderState* state: stateMap)
    {
        delete state;
    }
}

void CFitDecoder::resetSharedData()
{
    data.defintions = QMap<uint8_t, CFitDefinitionMessage>();
    data.defintionHistory = QList<CFitDefinitionMessage>();
    data.messages = QList<CFitMessage>();
    data.lastDefintion = nullptr;
    data.lastMessage = nullptr;
    data.timestamp = 0;
    data.lastTimeOffset = 0;
    data.fileBytesRead = 0;
    data.fileLength = 0;
    data.crc = 0;
}

void printDefintions(const QList<CFitDefinitionMessage>& defs)
{
    for(int i = 0; i < defs.size(); i++)
    {
        for(QString& s: defs[i].messageInfo())
        {
            qDebug() << s;
        }
    }
}

void printMessages(const QList<CFitMessage>& messages)
{
    for(int i = 0; i < messages.size(); i++)
    {
        for(QString& s: messages[i].messageInfo())
        {
            qDebug() << s;
        }
    }
}

void CFitDecoder::printDebugInfo()
{
    FITDEBUG(1, printDefintions(data.defintionHistory))
    FITDEBUG(1, printMessages(data.messages))
}
bool CFitDecoder::decode(QFile &file)
{
    resetSharedData();

    file.seek(0);

    uint8_t dataByte;
    decode_state_e state = eDecoderStateFileHeader;
    while (!file.atEnd())
    {
        file.getChar((char *) &dataByte);
        try
        {
            state = stateMap[state]->processByte(dataByte);
            if (state == eDecoderStateEnd)
            {
                printDebugInfo();
                return true;
            }
        }
        catch(QString exstr)
        {
            printDebugInfo();
            qWarning() << exstr;
            return false;
        }
    }
    printDebugInfo();
    // unexpected end of file
    return false;
}

const QList<CFitMessage>& CFitDecoder::getMessages() const
{
    return data.messages;
}
