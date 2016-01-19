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

#include "gis/fit/decoder/CFitHeaderState.h"


/**
 * byte
 * 0: the header size wihtout crc (12 for the current version)
 * 1: protocol version
 * 2: profil version LSB
 * 3: profil version MSB
 * 4: data size LSB
 * 5: data size
 * 6: data size
 * 7: data size
 * 8: "."
 * 9: "F"
 * 10: "I"
 * 11: "T"
 * 12: CRC LSB
 * 13: CRC MSB
 */

static const quint8 fitProtocolVersionMajor =1;

static const quint8 fitProtocolMajerVersionShift = 4;
static const quint8 fitProtocolMajorVersionMask = 0x0F << fitProtocolMajerVersionShift;

void CFitHeaderState::reset()
{
    setFileLength(3); // Header byte + CRC.
    offset = 0;

    setTimestamp(0);
    resetFileBytesRead();
}

decode_state_e CFitHeaderState::process(quint8 &dataByte)
{
    bool invalid = false;
    switch (offset++)
    {
    case 0:
        // header length
        headerLength = dataByte;
        setFileLength(headerLength + 2);
        break;

    case 1:
        // protocol version
        if ((dataByte & fitProtocolMajorVersionMask) >
            (fitProtocolVersionMajor << fitProtocolMajerVersionShift))
        {
            throw QObject::tr("FIT decoding error: protocol %1 version not supported.").arg(dataByte & fitProtocolMajorVersionMask);
        }
        break;

    case 4:
        // data size
        dataSize = (quint32) (dataByte & 0xFF);
        break;

    case 5:
        // data size
        dataSize |= (quint32) (dataByte & 0xFF) << 8;
        break;

    case 6:
        // data size
        dataSize |= (quint32) (dataByte & 0xFF) << 16;
        break;

    case 7:
        // data size
        dataSize |= (quint32) (dataByte & 0xFF) << 24;
        setFileLength(dataSize + headerLength + 2);     // include crc
        break;

    case 8:
        // "."
        invalid = (dataByte != '.');
        break;

    case 9:
        // "F"
        invalid = (dataByte != 'F');
        break;

    case 10:
        // "I"
        invalid = (dataByte != 'I');
        break;

    case 11:
        // "T"
        invalid = (dataByte != 'T');
        break;

    default:
        break;
    }

    if (invalid)
    {
        throw QObject::tr("FIT decoding error: file header signature mismatch. File is not FIT.");
    }

    if (offset == headerLength)
    {
        // end of header
        return eDecoderStateRecord;
    }
    return eDecoderStateFileHeader;
}