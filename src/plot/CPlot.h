/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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
#include "plot/IPlot.h"
#include <functional>

class CPlot : public IPlot
{
    Q_OBJECT
public:
    using funcGet = std::function<qreal(const CGisItemTrk::trkpt_t&)>;

    CPlot(CGisItemTrk *trk, CPlotData::axistype_e type, const QString &xLabel, const QString &yLabel, qreal factor, funcGet getX, funcGet getY, QWidget *parent);
    CPlot(CGisItemTrk *trk, QWidget *parent);
    virtual ~CPlot() = default;

    void setup(CPlotData::axistype_e type, const QString &xLabel, const QString &yLabel, qreal f, funcGet funcGetX, funcGet funcGetY);

    void setLimits(qreal min, qreal max);

    void updateData();

    void setMouseFocus(const CGisItemTrk::trkpt_t * ptMouseMove);

private:
    void setLimitsOnData(qreal min, qreal max);

    qreal maxLimit = NOFLOAT;
    qreal minLimit = NOFLOAT;
    qreal factor = 1.0;
    funcGet getX = nullptr;
    funcGet getY = nullptr;
};

#endif //CPLOT_H

