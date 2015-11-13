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

#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CGraphTrk.h"
#include "units/IUnit.h"

#include <QtWidgets>

CGraphTrk::CGraphTrk(CGisItemTrk *trk)
    : trk(trk)
{

    const CGisItemTrk::trk_t& data = trk->getTrackData();
    if(data.segs.isEmpty())
    {
        return;
    }

    const CGisItemTrk::trkseg_t& seg = data.segs[0];
    if(seg.pts.isEmpty())
    {
        return;
    }

    property_t propProgress
    {
         QObject::tr("Progress")
        ,CPlotData::eAxisTime
        ,QObject::tr("time")
        ,QObject::tr("distance [%1]").arg(IUnit::self().baseunit)
        ,IUnit::self().basefactor
        ,[](const CGisItemTrk::trkpt_t &p) {return p.time.toTime_t(); }
        ,[](const CGisItemTrk::trkpt_t &p) {return p.distance; }
    };


    property_t propSpeed
    {
        QObject::tr("Speed")
        ,CPlotData::eAxisLinear
        ,QObject::tr("distance [%1]")
        ,QObject::tr("speed [%1]").arg(IUnit::self().speedunit)
        ,IUnit::self().speedfactor
        ,[](const CGisItemTrk::trkpt_t &p) {return p.distance; }
        ,[](const CGisItemTrk::trkpt_t &p) {return p.speed; }

    };

    properties << propProgress;
    properties << propSpeed;


    const QHash<QString,QVariant>& extensions = seg.pts.first().extensions;
    QStringList keys = extensions.keys();
    keys.sort();
    foreach(const QString& key, keys)
    {
        bool ok = false;
        extensions[key].toFloat(&ok);
        if(!ok)
        {
            continue;
        }

        QString name = key.split(":").last();

        property_t property
        {
            name
            ,CPlotData::eAxisLinear
            ,QObject::tr("distance [%1]")
            ,name
            ,1.0
            ,[](const CGisItemTrk::trkpt_t &p) {return p.distance;}
            ,[key](const CGisItemTrk::trkpt_t &p) {return p.extensions[key].toFloat();}
        };

        properties << property;

    }

}

void CGraphTrk::fillComboBox(QComboBox * box) const
{
    foreach(const property_t& property, properties)
    {
        box->addItem(property.name);
    }
}

void CGraphTrk::setupPlot(CPlot * plot, int idx) const
{
    if(idx >= properties.size())
    {
        return;
    }
    const property_t& p = properties[idx];

    plot->setup(p.axisType, p.xLabel, p.yLabel, p.factor, p.getX, p.getY);
}
