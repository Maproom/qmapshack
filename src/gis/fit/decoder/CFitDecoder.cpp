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

#include "gis/fit/decoder/CFitDecoder.h"

#include "gis/fit/decoder/CFitHeaderState.h"
#include "gis/fit/decoder/CFitRecordHeaderState.h"
#include "gis/fit/decoder/CFitRecordContentState.h"
#include "gis/fit/decoder/CFitFieldDefinitionState.h"
#include "gis/fit/decoder/CFitCrcState.h"
#include "gis/fit/decoder/CFitFieldDataState.h"


CFitDecoder::CFitDecoder() {

    stateMap[StateFileHeader] = new CFitHeaderState(data);
    stateMap[StateRecord] = new CFitRecordHeaderState(data);
    stateMap[StateRecordContent] = new CFitRecordContentState(data);
    stateMap[StateFieldDef] = new CFitFieldDefinitionState(data);
    stateMap[StateFieldData] = new CFitFieldDataState(data);
    stateMap[StateFileCrc] = new CFitCrcState(data);
}

CFitDecoder::~CFitDecoder()
{
    // TODO destructor
    for(CFitDecoderState* state: stateMap)
    {
        delete state;
    }
}

void CFitDecoder::resetSharedData()
{
    data.defintions = QMap<uint8_t, CFitDefinitionMessage*>();
    data.messages = QList<CFitMessage*>();
    data.lastDefintion = nullptr;
    data.lastMessage = nullptr;
    data.timestamp = 0;
    data.lastTimeOffset = 0;
    data.fileBytesRead = 0;
    data.fileLength = 0;
    data.crc = 0;
}

bool CFitDecoder::decode(QFile &file) {
    resetSharedData();

    file.seek(0);

    uint8_t dataByte;
    DecodeState state = StateFileHeader;
    while (!file.atEnd()) {
        file.getChar((char *) &dataByte);
        try
        {
            state = stateMap[state]->processByte(dataByte);
            if (state == StateEnd) {
                return true;
            }
        } catch(QString exstr)
        {
            qWarning() << exstr;
            return false;
        }
    }
    // unexpected end of file
    return false;
}

QList<CFitMessage*> CFitDecoder::getMessages() {
    return data.messages;
}
