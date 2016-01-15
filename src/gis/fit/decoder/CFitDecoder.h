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

#ifndef CFITDECODER_H
#define CFITDECODER_H

#include "gis/fit/decoder/CFitDecoder.h"
#include "gis/fit/decoder/CFitMessage.h"
#include "gis/fit/decoder/IFitDecoderState.h"

#include <QtCore>


class CFitDecoder final
{
public:
    CFitDecoder();
    ~CFitDecoder();

    bool decode(QFile& file);
    const QList<CFitMessage>& getMessages() const;

private:
    void resetSharedData();
    void printDebugInfo();

    // map containing all states for the decoder. Needs to be pointer because decoder state is abstract class
    QMap<decode_state_e, IFitDecoderState *> stateMap;

    // shared data passed along the decoder state instances.
    IFitDecoderState::shared_state_data_t data;
};



#endif // CFITDECODER_H
