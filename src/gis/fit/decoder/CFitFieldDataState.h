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

#ifndef CFITFIELDDATASTATE_H
#define CFITFIELDDATASTATE_H

#include "gis/fit/decoder/CFitDecoderState.h"

static const int FitMaxFieldSize=255;

class CFitFieldDataState : public CFitDecoderState
{
public:
    CFitFieldDataState(shared_state_data_t &data) : CFitDecoderState(data, eDecoderStateFieldData) { reset(); };
    virtual ~CFitFieldDataState() {};
    virtual void reset();
    virtual decode_state_e process(uint8_t &dataByte);

private:
    uint8_t fieldIndex;
    uint8_t fieldDataIndex;
    uint8_t fieldData[FitMaxFieldSize];
};


#endif // CFITFIELDDATASTATE_H
