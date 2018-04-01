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
#include "gis/WptIcons.h"

CGeoSearchConfig::CGeoSearchConfig(QObject *parent) : QObject(parent)
{
}

CGeoSearchConfig::~CGeoSearchConfig()
{
}

void CGeoSearchConfig::load()
{
    SETTINGS;
    cfg.beginGroup("Search");
    symbolName = cfg.value("symbol","Default").toString();
    cfg.beginGroup("google");
    googleApiEnabled = cfg.value("enabled",true).toBool();
    googleApiKey = cfg.value("key","").toString();
    cfg.endGroup();
    cfg.beginGroup("geonames");
    geonamesSearchEnabled = cfg.value("search",true).toBool();
    geonamesAddressEnabled = cfg.value("address",true).toBool();
    geonamesUsername = cfg.value("username","").toString();
    cfg.endGroup();
    cfg.beginGroup("nominatim");
    nominatimEnabled = cfg.value("enabled",true).toBool();
    nominatimEmail = cfg.value("email","").toString();
    cfg.endGroup();
    cfg.endGroup();
}

void CGeoSearchConfig::save() const
{
    SETTINGS;
    cfg.beginGroup("Search");
    cfg.setValue("symbol",symbolName);
    cfg.beginGroup("google");
    cfg.setValue("enabled",googleApiEnabled);
    cfg.setValue("key",googleApiKey);
    cfg.endGroup();
    cfg.beginGroup("geonames");
    cfg.setValue("search",geonamesSearchEnabled);
    cfg.setValue("address",geonamesAddressEnabled);
    cfg.setValue("username",geonamesUsername);
    cfg.endGroup();
    cfg.beginGroup("nominatim");
    cfg.setValue("enabled",nominatimEnabled);
    cfg.setValue("email",nominatimEmail);
    cfg.endGroup();
    cfg.endGroup();
}
