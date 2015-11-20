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
#include "gis/trk/CKnownExtension.h"
#include "gis/trk/CPropertyTrk.h"
#include "units/IUnit.h"

#include <QtWidgets>

CPropertyTrk::CPropertyTrk(const CGisItemTrk& trk)
    : trk(trk)
{
    setupData();
}

void CPropertyTrk::setupData()
{
    properties.clear();

    property_t propNull
    {
        ""
        , ""
        , QIcon()
        , CPlotData::eAxisLinear
        , ""
        , ""
        , 1.0
        , nullptr
        , nullptr
    };
    properties << propNull;

    property_t propProgress
    {
        "progress"
        , QObject::tr("Progress")
        , QIcon("://icons/32x32/Progress.png")
        , CPlotData::eAxisTime
        , QObject::tr("time")
        , QObject::tr("distance [%1]").arg(IUnit::self().baseunit)
        , IUnit::self().basefactor
        , [](const CGisItemTrk::trkpt_t &p) {return p.time.toTime_t(); }
        , [](const CGisItemTrk::trkpt_t &p) {return p.distance; }
    };
    properties << propProgress;

    QStringList keys = trk.getExistingDataSources();
    foreach(const QString &key, keys)
    {
        // skip elevation as it is covered by the profile plot
        if(key == "ele")
        {
            continue;
        }

        const CKnownExtension &ext = CKnownExtension::get(key);
        QString name = (ext.known ? ext.name : key);

        property_t property
        {
            key
            , name
            , QIcon(ext.icon)
            , CPlotData::eAxisLinear
            , QObject::tr("distance [%1]")
            , QString("%1 [%2]").arg(name).arg(ext.unit)
            , ext.factor
            , [](const CGisItemTrk::trkpt_t &p) {return p.distance; }
            , ext.valueFunc
        };

        // lame hack
        if(key == "speed")
        {
            property.min = 0;
        }

        properties << property;
    }
}

void CPropertyTrk::fillComboBox(QComboBox * box) const
{
    box->clear();

    foreach(const property_t &p, properties)
    {
        box->addItem(p.icon, p.name, p.key);
    }
}

void CPropertyTrk::setupPlot(CPlot * plot, int idx) const
{
    if(idx >= properties.size())
    {
        return;
    }
    const property_t& p = properties[idx];

    plot->setup(p.axisType, p.xLabel, p.yLabel, p.factor, p.getX, p.getY);
    plot->setLimits(p.min, p.max);
}
