/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef IRTGPSDEVICE_H
#define IRTGPSDEVICE_H

#include <QString>
class QSettings;

class IRtGpsDevice
{
public:
    IRtGpsDevice() = default;
    virtual ~IRtGpsDevice() = default;

    virtual void loadSettings(QSettings& cfg) = 0;
    virtual void saveSettings(QSettings& cfg) const = 0;

};

#endif //IRTGPSDEVICE_H

