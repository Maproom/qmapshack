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

#include "CGeoSearchWeb.h"

CGeoSearchWeb * CGeoSearchWeb::pSelf = nullptr;

CGeoSearchWeb::CGeoSearchWeb(QObject * parent)
    : QObject(parent)
{
    pSelf = this;

    services << service_t(tr("PeakFinder"), ">https://www.peakfinder.org/?lat=%3&amp;lng=%2&amp;off=20&amp;azi=0&amp;zoom=4");
    services << service_t(tr("Waymarked Trails Hiking"), "https://hiking.waymarkedtrails.org/#routelist?map=13!%3!%2");
}


