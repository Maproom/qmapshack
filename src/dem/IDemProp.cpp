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

#include "dem/CDemDraw.h"
#include "dem/IDem.h"
#include "dem/IDemProp.h"

IDemProp::IDemProp(IDem *demfile, CDemDraw *dem)
    : demfile(demfile)
    , dem(dem)
{
    connect(demfile, &IDem::sigPropertiesChanged, this, &IDemProp::slotPropertiesChanged);
}

IDemProp::~IDemProp()
{
}

