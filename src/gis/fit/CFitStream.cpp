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

#include "gis/fit/CFitStream.h"

#include <QtCore>

bool CFitStream::decodeFile()
{
    return decode.decode(file);
}


void CFitStream::reset()
{
    readPos = 0;
}


const CFitMessage& CFitStream::nextMesg()
{
    return decode.getMessages().at(readPos++);
}


const CFitMessage& CFitStream::lastMesg()
{
    int pos = readPos-1;
    if(pos < 0)
    {
        pos = 0;
    }
    return decode.getMessages().at(pos);
}


bool CFitStream::hasMoreMesg()
{
    return readPos < decode.getMessages().size();
}

const CFitMessage& CFitStream::nextMesgOf(quint16 mesgNum)
{
    while(hasMoreMesg())
    {
        const CFitMessage& mesg = nextMesg();
        if (mesg.getGlobalMesgNr() == mesgNum)
        {
            return mesg;
        }
    }

    static CFitMessage dummyMessage {};
    return dummyMessage;
}


const CFitMessage& CFitStream::firstMesgOf(quint16 mesgNum)
{
    reset();
    const CFitMessage& mesg = nextMesgOf(mesgNum);
    reset();
    return mesg;
}

int CFitStream::countMesgOf(quint16 mesgNr)
{
    reset();
    int c = 0;
    while(nextMesgOf(mesgNr).getGlobalMesgNr() != fitGlobalMesgNrInvalid)
    {
        c++;
    }
    reset();
    return c;
}


