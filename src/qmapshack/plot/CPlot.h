/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CPLOT_H
#define CPLOT_H

#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CPropertyTrk.h"
#include "plot/IPlot.h"
#include <functional>

class CLimit;

class CPlot : public IPlot
{
    Q_OBJECT
public:
    CPlot(CGisItemTrk *trk, CLimit& limit, CPlotData::axistype_e type, const QString &xLabel, const QString &yLabel, qreal factor, fTrkPtGetVal getX, fTrkPtGetVal getY, QWidget *parent);
    CPlot(CGisItemTrk *trk, CLimit& limit, mode_e mode,  QWidget *parent);
    virtual ~CPlot() = default;

    void setup(const CPropertyTrk::property_t &p);

    void updateData() override;

    void setMouseFocus(const CTrackData::trkpt_t * ptMouseMove) override;

public slots:
    void setLimits();

private:
    CLimit& limit;

    qreal factor = 1.0;
    fTrkPtGetVal getX = nullptr;
    fTrkPtGetVal getY = nullptr;
};

#endif //CPLOT_H

