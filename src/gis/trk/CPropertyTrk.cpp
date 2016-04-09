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
#include "plot/CPlot.h"
#include "units/IUnit.h"

#include <QtWidgets>

CPropertyTrk::CPropertyTrk(CGisItemTrk& trk)
    : trk(trk)
{
    setupData();
}

void CPropertyTrk::setupData()
{
    properties.clear();

    property_t propNull
    {
        QString()
        , QString()
        , QIcon()
        , property_t::eAxisDistance
        , nullptr
        , QString()
        , QString()
        , 1.0
        , nullptr
    };
    properties << propNull;

    QStringList keys = trk.getExistingDataSources();
    for(const QString &key : keys)
    {
        const CKnownExtension &ext = CKnownExtension::get(key);
        QString name = (ext.known ? ext.name : key);

        property_t property
        {
            key
            , name
            , QIcon(ext.icon)
            , property_t::eAxisDistance
            , [](const CGisItemTrk::trkpt_t &p) {return p.distance; }
            , ext.unit
            , ext.known ? QString("%1 [%2]").arg(name).arg(ext.unit) : name
            , ext.factor
            , ext.valueFunc
        };

        // lame hack for properties off the usual scheme
        if(key == CKnownExtension::internalProgress)
        {
            property.min        = 0;
            property.axisType   = property_t::eAxisTime;
            property.getX       = [](const CGisItemTrk::trkpt_t &p) {return p.time.isValid() ? p.time.toTime_t() : NOFLOAT; };
        }

        if(key == CKnownExtension::internalSpeed)
        {
            property.min        = 0;
        }

        properties << property;
    }
}

const CPropertyTrk::property_t& CPropertyTrk::propBySource(const QString& source) const
{
    for(const property_t &prop : properties)
    {
        if(prop.key == source)
        {
            return prop;
        }
    }

    return properties[0];
}

void CPropertyTrk::fillComboBox(QComboBox * box) const
{
    box->clear();

    for(const property_t &p : properties)
    {
        if(p.key == CKnownExtension::internalEle)
        {
            // skip it as there is a dedicated profile plot
            continue;
        }
        box->addItem(p.icon, p.name, p.key);
    }
}

void CPropertyTrk::setupPlot(CPlot * plot, const QString& source) const
{
    const property_t& p = propBySource(source);
    if(p.name.isEmpty())
    {
        plot->clear();
        return;
    }

    plot->setup(p);
}

