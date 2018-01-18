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

#include "realtime/CRtDraw.h"
#include "realtime/CRtWorkspace.h"

#include <QtGui>

CRtWorkspace * CRtWorkspace::pSelf = nullptr;

CRtWorkspace::CRtWorkspace(QWidget *parent)
    : QWidget(parent)
{
    pSelf = this;
    setupUi(this);
}

void CRtWorkspace::draw(QPainter& p, const QPolygonF &viewport, CRtDraw *gis)
{
}

void CRtWorkspace::fastDraw(QPainter& p, const QRectF& viewport, CRtDraw *gis)
{
}
