/**********************************************************************************************
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "gis/search/CGeoSearchConfig.h"
#include "helpers/CSettings.h"
#include "helpers/CWptIconManager.h"
#include <QIcon>

CGeoSearchConfig * CGeoSearchConfig::pSelf = nullptr;

CGeoSearchConfig::CGeoSearchConfig(QObject *parent) : QObject(parent)
{
    pSelf = this;
}


void CGeoSearchConfig::load()
{
    SETTINGS;
    cfg.beginGroup("Search");
    accumulativeResults = cfg.value("accumulativeResults", accumulativeResults).toBool();
    symbolName = cfg.value("symbol", "Default").toString();
    currentService = service_e(cfg.value("current", eServiceNominatim).toInt());
    cfg.beginGroup("google");
    googleApiKey = cfg.value("key", "").toString();
    cfg.endGroup();
    cfg.beginGroup("geonames");
    geonamesUsername = cfg.value("username", "").toString();
    cfg.endGroup();
    cfg.beginGroup("nominatim");
    nominatimEmail = cfg.value("email", "").toString();
    nominatimLimit = cfg.value("limit", nominatimLimit).toInt();
    cfg.endGroup();
    cfg.endGroup();
    emit sigConfigChanged();
}

void CGeoSearchConfig::save() const
{
    SETTINGS;
    cfg.beginGroup("Search");
    cfg.setValue("accumulativeResults", accumulativeResults);
    cfg.setValue("symbol", symbolName);
    cfg.setValue("current", currentService);
    cfg.beginGroup("google");
    cfg.setValue("key", googleApiKey);
    cfg.endGroup();
    cfg.beginGroup("geonames");
    cfg.setValue("username", geonamesUsername);
    cfg.endGroup();
    cfg.beginGroup("nominatim");
    cfg.setValue("email", nominatimEmail);
    cfg.setValue("limit", nominatimLimit);
    cfg.endGroup();
    cfg.endGroup();
}

const QIcon CGeoSearchConfig::getCurrentIcon() const
{
    switch(currentService)
    {
    case CGeoSearchConfig::eServiceNominatim:
    {
        return QIcon("://icons/32x32/SearchNominatim.png");
        break;
    }

    case CGeoSearchConfig::eServiceGeonamesSearch:
    case CGeoSearchConfig::eServiceGeonamesAddress:
    {
        return QIcon("://icons/32x32/SearchGeonames.png");
        break;
    }

    case CGeoSearchConfig::eServiceGoogle:
    {
        return QIcon("://icons/32x32/SearchGoogle.png");
        break;
    }

    default:
        return QIcon("://icons/32x32/Zoom.png");
    }
}

void CGeoSearchConfig::emitChanged()
{
    emit sigConfigChanged();
}
