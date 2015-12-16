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

#ifndef CFITDECODER_H
#define CFITDECODER_H

#include "gis/fit/decoder/CFitDecoder.h"
#include "gis/fit/decoder/CFitDecoderState.h"
#include "gis/fit/decoder/CFitMessage.h"

#include <QtCore>


class CFitDecoder
{
public:
    CFitDecoder();
    ~CFitDecoder();

    bool decode(QFile& file);
    QList<CFitMessage*> getMessages();

private:
    // map containing all states for the decoder. Needs to be pointer because decoder state is abstract class
    QMap<DecodeState, CFitDecoderState*> stateMap;

    // shared data passed along the decoder state instances.
    CFitDecoderState::shared_state_data data;

    void resetSharedData();
};



#endif // CFITDECODER_H
