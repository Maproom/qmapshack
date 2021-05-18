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

#include "gis/fit/decoder/CFitCrcState.h"
#include "gis/fit/decoder/CFitDecoder.h"
#include "gis/fit/decoder/CFitDevFieldDefinitionState.h"
#include "gis/fit/decoder/CFitFieldDataState.h"
#include "gis/fit/decoder/CFitFieldDefinitionState.h"
#include "gis/fit/decoder/CFitHeaderState.h"
#include "gis/fit/decoder/CFitRecordContentState.h"
#include "gis/fit/decoder/CFitRecordHeaderState.h"
#include "gis/fit/defs/fit_const.h"

CFitDecoder::CFitDecoder()
{
    stateMap[eDecoderStateFileHeader] = new CFitHeaderState(data);
    stateMap[eDecoderStateRecord] = new CFitRecordHeaderState(data);
    stateMap[eDecoderStateRecordContent] = new CFitRecordContentState(data);
    stateMap[eDecoderStateFieldDef] = new CFitFieldDefinitionState(data);
    stateMap[eDecoderStateDevFieldDef] = new CFitDevFieldDefinitionState(data);
    stateMap[eDecoderStateFieldData] = new CFitFieldDataState(data);
    stateMap[eDecoderStateFileCrc] = new CFitCrcState(data);
}

CFitDecoder::~CFitDecoder()
{
    qDeleteAll(stateMap);
    stateMap.clear();

    data.messages.clear();
}

void CFitDecoder::resetSharedData()
{
    data.definitions = QMap<quint8, CFitDefinitionMessage>();
    data.definitionHistory = QList<CFitDefinitionMessage>();
    data.messages = QList<CFitMessage>();
    data.devFieldProfiles = QList<CFitFieldProfile>();
    data.lastDefinition = nullptr;
    data.lastMessage = nullptr;
    data.timestamp = 0;
    data.lastTimeOffset = 0;
    data.fileBytesRead = 0;
    data.fileLength = 0;
    data.crc = 0;
}

void printDefinitions(const QList<CFitDefinitionMessage>& defs)
{
    for(int i = 0; i < defs.size(); i++)
    {
        for(QString& s : defs[i].messageInfo())
        {
            qDebug() << s;
        }
    }
}

void printMessages(const QList<CFitMessage>& messages)
{
    for(int i = 0; i < messages.size(); i++)
    {
        for(QString& s : messages[i].messageInfo())
        {
            qDebug() << s;
        }
    }
}

void CFitDecoder::printDebugInfo()
{
    FITDEBUG(1, printDefinitions(data.definitionHistory))
    FITDEBUG(1, printMessages(data.messages))
}

QList<QString> decoderStateNames = {"File Header", "Record", "Record Content", "Field Definition",
                                    "Development Field Definition", "Field Data", "CRC", "End"};

void printByte(QFile& file, decode_state_e state, quint8 dataByte)
{
    FITDEBUG(3, qDebug() << QString("decoding byte %1 - %2 - %3")
             .arg(file.pos(), 6, 10, QLatin1Char(' '))
             .arg(dataByte, 8, 2, QLatin1Char('0'))
             .arg(decoderStateNames.at(state)));
}

void CFitDecoder::decode(QFile& file)
{
    resetSharedData();

    file.seek(0);

    quint8 dataByte;
    decode_state_e state = eDecoderStateFileHeader;
    while (!file.atEnd())
    {
        file.getChar((char*) &dataByte);
        try
        {
            printByte(file, state, dataByte);
            state = stateMap[state]->processByte(dataByte);
            if (state == eDecoderStateEnd)
            {
                // end of file, everything ok
                printDebugInfo();
                return;
            }
        }
        catch(QString& errormsg)
        {
            printDebugInfo();
            throw errormsg;
        }
    }
    // unexpected end of file
    printDebugInfo();
    throw tr("FIT decoding error: unexpected end of file %1.").arg(file.fileName());
}

const QList<CFitMessage>& CFitDecoder::getMessages() const
{
    return data.messages;
}
