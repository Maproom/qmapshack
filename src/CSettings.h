/**********************************************************************************************
    Copyright (C) 2012 Oliver Eichler oliver.eichler@gmx.de

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
#ifndef CSETTINGS_H
#define CSETTINGS_H

#include "CAppOpts.h"
#include <QtCore>

class CSettings : public QObject
{

    public:
        CSettings()
        {
            if(!qlOpts->configfile.isEmpty())
            {
                cfg = new QSettings(qlOpts->configfile, QSettings::IniFormat, this);
            }
            else
            {
                cfg = new QSettings(this);
            }
        }
        ~CSettings(){}

        QSettings& get(){return *cfg;}

    private:
        QSettings  * cfg;
};

#define SETTINGS \
CSettings ccfg;\
QSettings& cfg = ccfg.get()
#endif                           //CSETTINGS_H
