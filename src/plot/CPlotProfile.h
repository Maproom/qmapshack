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

#ifndef CPLOTPROFILE_H
#define CPLOTPROFILE_H


#include "plot/IPlot.h"

class CLimit;

class CPlotProfile : public IPlot
{
    Q_OBJECT
public:
    CPlotProfile(QWidget * parent);
    CPlotProfile(CGisItemTrk * trk, CLimit& lim, mode_e mode, QWidget * parent);
    virtual ~CPlotProfile();

    void setTrack(CGisItemTrk * track, CLimit& lim);
    virtual void updateData() override;
    virtual void setMouseFocus(const CGisItemTrk::trkpt_t * ptMouseMove) override;

public slots:
    void setLimits();

private:
    CLimit * limit = nullptr;
};

#endif //CPLOTPROFILE_H

