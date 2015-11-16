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

#include "plot/CPlotAxis.h"
#include "plot/CPlotAxisTime.h"
#include "plot/CPlotData.h"
#include "units/IUnit.h"

CPlotData::CPlotData(axistype_e type, QObject * parent)
    : QObject(parent)
    , axisType(type)  
{
    setXAxisType(type);
    yaxis = new CPlotAxis(this);
}


CPlotData::~CPlotData()
{
}

void CPlotData::setXAxisType(axistype_e type)
{
    delete xaxis;

    if(type == eAxisLinear)
    {
        xaxis = new CPlotAxis(this);
    }
    else
    {
        xaxis = new CPlotAxisTime(this);
    }
    xaxis->setAutoscale(false);

    axisType = type;
}

void CPlotData::setLimits()
{
    if(lines.size() == 0 || badData)
    {
        return;
    }

    QList<line_t>::const_iterator line  = lines.begin();
    if(line == lines.end())
    {
        return;
    }
    QPolygonF::const_iterator p         = line->points.begin();

    xmin = p->x();
    xmax = p->x();
    ymin = p->y();
    ymax = p->y();

    while(line != lines.end())
    {
        QPolygonF::const_iterator p = line->points.begin();
        while(p != line->points.end())
        {
            if(p->y() != NOFLOAT)
            {
                if(p->x() > xmax)
                {
                    xmax = p->x();
                }
                if(p->x() < xmin)
                {
                    xmin = p->x();
                }
                if(p->y() > ymax)
                {
                    ymax = p->y();
                }
                if(p->y() < ymin)
                {
                    ymin = p->y();
                }
            }
            ++p;
        }

        ++line;
    }

    xaxis->setLimits(xmin,xmax);
    yaxis->setLimits(ymin,ymax);
}
