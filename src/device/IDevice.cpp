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

#include "device/IDevice.h"
#include "gis/CGisListWks.h"

IDevice::IDevice(const QString &path, const QString &key, QTreeWidget *parent)
    : QTreeWidgetItem(parent)
    , path(path)
    , key(key)
{
    setIcon(CGisListWks::eColumnName, QIcon("://icons/32x32/Device.png"));
}

IDevice::~IDevice()
{

}

