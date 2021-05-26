/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "realtime/gpstether/CRtGpsTether.h"
#include "realtime/IRtSource.h"
#include "realtime/opensky/CRtOpenSky.h"

#include <QtWidgets>

QMutex IRtSource::mutex(QMutex::Recursive);

IRtSource::IRtSource(type_e type, bool singleInstanceOnly, QTreeWidget* parent)
    : QObject(parent)
    , QTreeWidgetItem(parent)
    , type(type)
    , singleInstanceOnly(singleInstanceOnly)
{
}

void IRtSource::loadSettings(QSettings& cfg)
{
    setCheckState(eColumnCheckBox, Qt::CheckState(cfg.value("checkState", Qt::Checked).toInt()));
}

void IRtSource::saveSettings(QSettings& cfg) const
{
    cfg.setValue("checkState", checkState(eColumnCheckBox));
}


IRtSource* IRtSource::create(int type, QTreeWidget* parent)
{
    switch(type)
    {
    case eTypeOpenSky:
        return new CRtOpenSky(parent);

    case eTypeGpsTether:
        return new CRtGpsTether(parent);
    }

    return nullptr;
}
