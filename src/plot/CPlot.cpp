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

#include "plot/CPlot.h"
#include "plot/CPlotAxis.h"
#include "helpers/CLimit.h"

CPlot::CPlot(CGisItemTrk * trk,  CLimit& limit, CPlotData::axistype_e type, const QString& xLabel, const QString& yLabel, qreal factor, fTrkPtGetVal getX, fTrkPtGetVal getY, QWidget * parent)
    : IPlot(trk, type, eModeNormal, parent)
    , limit(limit)
    , factor(factor)
    , getX(getX)
    , getY(getY)
{

    connect(&limit, &CLimit::sigChanged, this, &CPlot::setLimits);

    setXLabel(xLabel);
    setYLabel(yLabel);

    updateData();
}

CPlot::CPlot(CGisItemTrk *trk,  CLimit& limit, QWidget *parent)
    : IPlot(trk, CPlotData::eAxisLinear, eModeNormal, parent)
    , limit(limit)
{
    connect(&limit, &CLimit::sigChanged, this, &CPlot::setLimits);
}

void CPlot::setup(const QString& source, const QString& unit, CPlotData::axistype_e type, const QString &xLabel, const QString &yLabel, qreal f, fTrkPtGetVal funcGetX, fTrkPtGetVal funcGetY)
{    
    data->setXAxisType(type);
    setXLabel(xLabel);
    setYLabel(yLabel);
    factor = f;
    getX = funcGetX;
    getY = funcGetY;
    limit.setSource(source, unit);
    updateData();
}


void CPlot::updateData()
{
    clear();

    if(isHidden() || (getX == nullptr) || (getY == nullptr))
    {
        resetZoom();
        update();
        return;
    }

    QPolygonF line;
    const CGisItemTrk::trk_t& t = trk->getTrackData();
    foreach (const CGisItemTrk::trkseg_t& seg, t.segs)
    {
        foreach(const CGisItemTrk::trkpt_t& trkpt, seg.pts)
        {
            if(!(trkpt.flags & CGisItemTrk::trkpt_t::eHidden)
               && getY(trkpt) != NOFLOAT)
            {
                line << QPointF(getX(trkpt), getY(trkpt) * factor);
            }
        }
    }

    newLine(line, "GPS");
    setLimits();
}

void CPlot::setMouseFocus(const CGisItemTrk::trkpt_t * ptMouseMove)
{
    if(ptMouseMove == 0 ||  getX == nullptr || getY == nullptr)
    {
        if(posMouse != NOPOINT)
        {
            posMouse = NOPOINT;
            needsRedraw = true;
        }
    }
    else
    {
        if(posMouse == NOPOINT)
        {
            needsRedraw = true;
        }

        posMouse.rx() = left  + data->x().val2pt(getX(*ptMouseMove));
        posMouse.ry() = top  +  data->y().val2pt(getY(*ptMouseMove));
    }
    update();
}

void CPlot::setLimits()
{
    IPlot::setLimits();
    data->ymin = limit.getMin() == NOFLOAT ? data->ymin : limit.getMin();
    data->ymax = limit.getMax() == NOFLOAT ? data->ymax : limit.getMax();

    data->y().setLimits(data->ymin, data->ymax);
    resetZoom();
    update();
}
