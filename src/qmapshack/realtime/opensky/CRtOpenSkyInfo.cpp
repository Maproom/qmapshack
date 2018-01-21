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

#include "realtime/opensky/CRtOpenSkyInfo.h"
#include "realtime/opensky/CRtOpenSky.h"

CRtOpenSkyInfo::CRtOpenSkyInfo(CRtOpenSky &source, QWidget *parent)
    : QWidget(parent)
    , source(source)
{
    setupUi(this);            
    connect(&source, &CRtOpenSky::sigChanged, this, &CRtOpenSkyInfo::slotUpdate);
    connect(checkShowNames, &QCheckBox::toggled, &source, &CRtOpenSky::slotSetShowNames);
}


void CRtOpenSkyInfo::slotUpdate()
{
    checkShowNames->setChecked(source.getShowNames());
    labelTimestamp->setText(source.getTimestamp().toString());
    labelNumberOfAircrafts->setText(QString::number(source.getNumberOfAircrafts()));
}
