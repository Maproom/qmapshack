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

CLimit::CLimit(const QString &cfgPath, fGetLimit getMin, fGetLimit getMax, fGetLimit getMinAuto, fGetLimit getMaxAuto, fGetUnit getUnit, fMarkChanged markChanged)
    : cfgPath(cfgPath)
    , funcGetMin(getMin)
    , funcGetMax(getMax)
    , funcGetMinAuto(getMinAuto)
    , funcGetMaxAuto(getMaxAuto)
    , funcGetUnit(getUnit)
    , funcMarkChanged(markChanged)
{
    allLimits << this;
}

CLimit::~CLimit()
{
    allLimits.remove(this);
}

void CLimit::setMode(mode_e m)
{
    bool markAsChanged = mode != m;

    mode = m;

    if(markAsChanged)
    {
        funcMarkChanged();
    }

    emit sigChanged();
}

void CLimit::setSource(const QString& src)
{
    bool markAsChanged = source != src;

    source  = src;
    minUser = funcGetMin(source);
    maxUser = funcGetMax(source);

    if(markAsChanged)
    {
        funcMarkChanged();
    }
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
        val = funcGetMinAuto(source);
        break;

    case eModeDefault:
        cfg.beginGroup(cfgPath);
        val = cfg.value(source + "/min", funcGetMin(source)).toReal();
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
        val = funcGetMaxAuto(source);
        break;

    case eModeDefault:
        cfg.beginGroup(cfgPath);
        val = cfg.value(source + "/max", funcGetMax(source)).toReal();
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
    {
        bool markAsChanged = minUser != val;
        minUser = val;
        if(markAsChanged)
        {
            funcMarkChanged();
        }
        break;
    }

    case eModeDefault:
    {
        cfg.beginGroup(cfgPath);
        cfg.setValue(source + "/min", val);
        cfg.endGroup();
        break;
    }
    }

    emit sigChanged();
}

void CLimit::setMax(const qreal &val)
{
    SETTINGS;

    switch(mode)
    {
    case eModeUser:
    {
        bool markAsChanged = maxUser != val;
        maxUser = val;
        if(markAsChanged)
        {
            funcMarkChanged();
        }
        break;
    }

    case eModeDefault:
    {
        cfg.beginGroup(cfgPath);
        cfg.setValue(source + "/max", val);
        cfg.endGroup();
        break;
    }
    }

    emit sigChanged();
}

QString CLimit::getUnit() const
{
    return funcGetUnit(source);
}
