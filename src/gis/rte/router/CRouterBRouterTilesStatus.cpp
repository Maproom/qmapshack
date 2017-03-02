/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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

#include "CRouterBRouterTilesStatus.h"

CRouterBRouterTilesStatus::CRouterBRouterTilesStatus(QObject *parent)
    : QObject(parent),
      max(0),
      val(0),
      size(0),
      file(nullptr)
{
}

CRouterBRouterTilesStatus::~CRouterBRouterTilesStatus()
{
}

void CRouterBRouterTilesStatus::updateProgress(qint64 received, qint64 total)
{
    max = total;
    val = received;
}
