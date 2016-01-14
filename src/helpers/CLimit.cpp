/**********************************************************************************************
    Copyright (C) 2014-2016 Oliver Eichler oliver.eichler@gmx.de

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
#include "helpers/CSettings.h"

QSet<CLimit*> CLimit::allLimits;

CLimit::CLimit(const QString &cfgPath, fGetLimit getMin, fGetLimit getMax, fGetLimit getMinAuto, fGetLimit getMaxAuto)
    : cfgPath(cfgPath)
    , fGetMin(getMin)
    , fGetMax(getMax)
    , fGetMinAuto(getMinAuto)
    , fGetMaxAuto(getMaxAuto)
{

    allLimits << this;
}

CLimit::~CLimit()
{
    allLimits.remove(this);
}


void CLimit::setSource(const QString& source)
{
    this->source = source;
    minUser = fGetMin(source);
    maxUser = fGetMax(source);
}

qreal CLimit::getMin() const
{
    SETTINGS;
    qreal val;

    switch(mode)
    {
    case eModeUser:
        val = minUser;
        break;

    case eModeAutomatic:
        val = fGetMinAuto(source);
        break;

    case eModeDefault:
        cfg.beginGroup(cfgPath);
        val = cfg.value(source + "/min", fGetMin(source)).toReal();
        cfg.endGroup();
        break;
    }

    return val;
}

qreal CLimit::getMax() const
{
    SETTINGS;
    qreal val;

    switch(mode)
    {
    case eModeUser:
        val = maxUser;
        break;

    case eModeAutomatic:
        val = fGetMaxAuto(source);
        break;

    case eModeDefault:
        cfg.beginGroup(cfgPath);
        val = cfg.value(source + "/max", fGetMax(source)).toReal();
        cfg.endGroup();
        break;
    }

    return val;
}

void CLimit::setMin(const qreal &val)
{
    SETTINGS;

    switch(mode)
    {
    case eModeUser:
        minUser = val;
        break;

    case eModeDefault:
        cfg.beginGroup(cfgPath);
        cfg.setValue(source + "/min", val);
        cfg.endGroup();
        break;
    }

    emit sigChanged();
}

void CLimit::setMax(const qreal &val)
{
    SETTINGS;

    switch(mode)
    {
    case eModeUser:
        maxUser = val;
        break;

    case eModeDefault:
        cfg.beginGroup(cfgPath);
        cfg.setValue(source + "/max", val);
        cfg.endGroup();
        break;
    }

    emit sigChanged();
}
