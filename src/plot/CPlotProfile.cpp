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

#include "CMainWindow.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "plot/CPlotAxis.h"
#include "plot/CPlotProfile.h"
#include "units/IUnit.h"

#include <proj_api.h>

CPlotProfile::CPlotProfile(QWidget * parent)
    : IPlot(0, CPlotData::eAxisLinear, eModeNormal, parent)
{
}

CPlotProfile::CPlotProfile(CGisItemTrk *trk, mode_e mode, QWidget *parent)
    : IPlot(trk, CPlotData::eAxisLinear, mode, parent)
{
    setWindowTitle(trk->getNameEx());
    updateData();
}

CPlotProfile::~CPlotProfile()
{
}

void CPlotProfile::setTrack(CGisItemTrk * track)
{
    trk = track;
    trk->registerPlot(this);

    updateData();
}

void CPlotProfile::updateData()
{
    CPlotData::axistype_e type = data->axisType;

    if(mode == eModeIcon)
    {
        setXLabel(trk->getName());
        setYLabel("");
    }
    else
    {
        if(type == CPlotData::eAxisLinear)
        {
            setXLabel(tr("distance [%1]").arg(IUnit::self().baseunit));
        }
        else
        {
            setXLabel(tr("time [h]"));
        }
        setYLabel(tr("alt. [%1]").arg(IUnit::self().baseunit));
    }


    clear();

    QPolygonF lineEle;
    QPolygonF lineDem;
    QPolygonF coords;

    IGisProject * project = dynamic_cast<IGisProject*>(trk->parent());

    qreal basefactor = IUnit::self().basefactor;
    const CGisItemTrk::trk_t& t = trk->getTrackData();
    foreach (const CGisItemTrk::trkseg_t& seg, t.segs)
    {
        foreach(const CGisItemTrk::trkpt_t& trkpt, seg.pts)
        {
            if(trkpt.flags & CGisItemTrk::trkpt_t::eHidden)
            {
                continue;
            }

            if(trkpt.ele == NOINT)
            {
                continue;
            }

            lineEle << QPointF(type == CPlotData::eAxisLinear ? trkpt.distance : (qreal)trkpt.time.toTime_t(), trkpt.ele * basefactor);
            coords << QPointF(trkpt.lon * DEG_TO_RAD, trkpt.lat * DEG_TO_RAD);
            lineDem << QPointF(type == CPlotData::eAxisLinear ? trkpt.distance : (qreal)trkpt.time.toTime_t(), NOFLOAT);

            if(project == 0 || trkpt.keyWpt.item.isEmpty() || (mode == eModeIcon))
            {
                continue;
            }

            CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(project->getItemByKey(trkpt.keyWpt));
            if(wpt)
            {
                CPlotData::point_t tag;
                tag.point = lineEle.last();
                tag.icon  = wpt->getIcon();
                tag.label = wpt->getName();
                data->tags << tag;
            }
        }
    }

    CMainWindow::self().getEelevationAt(coords, lineDem);

    newLine(lineEle, "GPS");
    if(!lineDem.isEmpty())
    {
        addLine(lineDem, "DEM");
    }
    setLimits();
    resetZoom();
}

void CPlotProfile::setMouseFocus(const CGisItemTrk::trkpt_t * ptMouseMove)
{
    if(ptMouseMove == 0)
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

        posMouse.rx() = left  + data->x().val2pt(ptMouseMove->distance);
        posMouse.ry() = top  +  data->y().val2pt(ptMouseMove->ele);
    }
    update();

}

void CPlotProfile::setMouseFocus(const CGisItemTrk::trkpt_t * ptRange1, const CGisItemTrk::trkpt_t *ptRange2)
{

    if(ptRange1 == 0 || ptRange2 == 0)
    {
        idxSel1 = idxSel2 = NOIDX;
    }
    else
    {
        if(ptRange1->idxTotal < ptRange2->idxTotal)
        {
            while(ptRange1->flags & CGisItemTrk::trkpt_t::eHidden)
            {
                ptRange1++;
            }
            while(ptRange2->flags & CGisItemTrk::trkpt_t::eHidden)
            {
                ptRange2--;
            }
            idxSel1 = ptRange1->idxVisible;
            idxSel2 = ptRange2->idxVisible;
        }
        else
        {
            while(ptRange1->flags & CGisItemTrk::trkpt_t::eHidden)
            {
                ptRange1--;
            }
            while(ptRange2->flags & CGisItemTrk::trkpt_t::eHidden)
            {
                ptRange2++;
            }
            idxSel1 = ptRange2->idxVisible;
            idxSel2 = ptRange1->idxVisible;
        }
    }
    update();
}
