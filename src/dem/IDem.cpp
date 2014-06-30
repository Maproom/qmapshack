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

#include "dem/IDem.h"
#include "dem/CDemDraw.h"


IDem::IDem(CDemDraw *parent)
    : QObject(parent)
    , dem(parent)
    , pjsrc(0)
    , isActivated(false)
{
    pjtar = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");
}

IDem::~IDem()
{
    pj_free(pjtar);
    pj_free(pjsrc);
}

void IDem::saveConfig(QSettings& cfg)
{

}

void IDem::loadConfig(QSettings& cfg)
{

}
