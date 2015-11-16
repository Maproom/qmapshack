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
#include "gis/trk/CKnownExtension.h"
#include "units/IUnit.h"

#include <QtWidgets>
#include <QString>

CGraphTrk::CGraphTrk(CGisItemTrk *trk)
    : trk(trk)
{

    property_t propProgress
    {
         QObject::tr("Progress")
        ,QIcon("://icons/32x32/Progress.png")
        ,CPlotData::eAxisTime
        ,QObject::tr("time")
        ,QObject::tr("distance [%1]").arg(IUnit::self().baseunit)
        ,IUnit::self().basefactor
        ,[](const CGisItemTrk::trkpt_t &p) {return p.time.toTime_t(); }
        ,[](const CGisItemTrk::trkpt_t &p) {return p.distance; }
    };
    properties << propProgress;

    foreach(const QString &key, trk->getExistingDataSources())
    {
        // skip elevation as it is covered by the profile plot
        if(key == "ele")
        {
            continue;
        }

        const CKnownExtension &ext = CKnownExtension::get(key);

        QString name = (ext.known ? ext.name : key);
        qDebug() << "register" << name << QString("%1 [%2]").arg(name).arg(ext.unit) << ext.factor;

        property_t property
        {
            name
            ,QIcon(ext.icon)
            ,CPlotData::eAxisLinear
            ,QObject::tr("distance [%1]")
            ,QString("%1 [%2]").arg(name).arg(ext.unit)
            ,ext.factor
            ,[](const CGisItemTrk::trkpt_t &p) {return p.distance;}
            ,ext.valueFunc
        };

        properties << property;

    }
}

void CGraphTrk::fillComboBox(QComboBox * box) const
{
    foreach(const property_t& p, properties)
    {
        box->addItem(p.icon, p.name);
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
