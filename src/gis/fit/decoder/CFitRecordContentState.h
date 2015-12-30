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

#ifndef CFITRECORDCONTENTSTATE_H
#define CFITRECORDCONTENTSTATE_H

#include "gis/fit/decoder/IFitDecoderState.h"

class CFitRecordContentState : public IFitDecoderState
{
public:
    CFitRecordContentState(shared_state_data_t &data) : IFitDecoderState(data, eDecoderStateRecordContent) { reset(); };
    virtual ~CFitRecordContentState() {};

    virtual void reset();
    virtual decode_state_e process(uint8_t &dataByte);

private:
    uint8_t offset;

    uint8_t architecture;
    uint32_t globalMesgNr;
    uint8_t nrOfFields;
};

#endif //CFITRECORDCONTENTSTATE_H
