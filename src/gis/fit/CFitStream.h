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

#ifndef CFITSTREAM_H
#define CFITSTREAM_H

#include "gis/fit/decoder/CFitDecoder.h"
#include "gis/fit/decoder/CFitDefinitionMessage.h"
#include "gis/fit/decoder/CFitMessage.h"
#include "gis/fit/defs/fit_const.h"


#include <QtCore>


/*
   Encapsulates the access to the FIT messages. Looping over the read FIT messages can be done using the
   methods nextMesg() and hasMoreMesg() (Iterator pattern).
 */
class CFitStream final
{
public:
    CFitStream(QFile& dev) : file(dev) { }

    /**
       setup FIT file decoder.
       return: true if the FIT file can be decoded.
       throws: fit::RuntimeException
     */
    bool decodeFile();

    /**
       sets the stream at the beginning (first position).
     */
    void reset();
    /**
       Get the next FIT message
       throws: fit::RuntimeException
     */
    const CFitMessage& nextMesg();

    /**
       return: the last read message again
     */
    const CFitMessage& lastMesg();

    /**
       return: true if there a further FIT message is available
     */
    bool hasMoreMesg();

    /**
       return: the next message of the given message type (xx_MESG_NUM) beginning reading at the current position
     */
    const CFitMessage& nextMesgOf(quint16 mesgNum);

    /**
       return: gets the first message of the given type (xx_MESG_NUM) beginning reading at start of stream.
       Sets the strema to the beginning again.
     */
    const CFitMessage& firstMesgOf(quint16 mesgNum);

    int countMesgOf(quint16 mesgNr);


    QString getFileName() { return file.fileName(); }

private:
    QFile& file;
    CFitDecoder decode;
    int readPos = 0;
};

#endif //CFITSTREAM_H

