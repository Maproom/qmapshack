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

#include "helpers/CLimit.h"
#include "plot/CPlot.h"
#include "plot/CPlotAxis.h"

CPlot::CPlot(CGisItemTrk* trk, CLimit& limit, CPlotData::axistype_e type, const QString& xLabel, const QString& yLabel, qreal factor, fTrkPtGetVal getX, fTrkPtGetVal getY, QWidget* parent)
    : IPlot(trk, type, eModeNormal, parent)
    , limit(limit)
    , factor(factor)
    , getX(getX)
    , getY(getY)
{
    connect(&limit, &CLimit::sigChanged, this, &CPlot::setLimits);

    setXLabel(xLabel);
    setYLabel(yLabel);

    CPlot::updateData();
}

CPlot::CPlot(CGisItemTrk* trk, CLimit& limit, IPlot::mode_e mode, QWidget* parent)
    : IPlot(trk, CPlotData::eAxisLinear, mode, parent)
    , limit(limit)
{
    connect(&limit, &CLimit::sigChanged, this, &CPlot::setLimits);
}

void CPlot::setup(const CPropertyTrk::property_t& p)
{
    if(p.axisType == CPropertyTrk::property_t::eAxisDistance)
    {
        data->setXAxisType(CPlotData::eAxisLinear);

        qreal scale;
        QString unit;
        IUnit::self().meter2unit(trk->getTotalDistance(), scale, unit);
        setXTicScale(scale);
        setXLabel(tr("Distance [%1]").arg(unit));
    }
    else if(p.axisType == CPropertyTrk::property_t::eAxisTime)
    {
        data->setXAxisType(CPlotData::eAxisTime);
        setXLabel(tr("Time"));
    }

    setYLabel(p.yLabel);
    factor = p.factor;
    getX = p.getX;
    getY = p.getY;
    limit.setSource(p.key);
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
    const CTrackData& t = trk->getTrackData();
    for(const CTrackData::trkpt_t& trkpt : t)
    {
        if(!trkpt.isHidden()
           && getY(trkpt) != NOFLOAT)
        {
            line << QPointF(getX(trkpt), getY(trkpt) * factor);
        }
    }

    newLine(line, "GPS");
    setLimits();
}

void CPlot::setMouseFocus(const CTrackData::trkpt_t* ptMouseMove)
{
    if(nullptr == ptMouseMove || getX == nullptr || getY == nullptr)
    {
        if(posMouse1 != NOPOINT)
        {
            posMouse1 = NOPOINT;
            needsRedraw = true;
        }
    }
    else
    {
        if(posMouse1 == NOPOINT)
        {
            needsRedraw = true;
        }

        posMouse1.rx() = left + data->x().val2pt(getX(*ptMouseMove));
        posMouse1.ry() = top + data->y().val2pt(getY(*ptMouseMove));
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
