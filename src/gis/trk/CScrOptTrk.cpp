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

#include "gis/trk/CScrOptTrk.h"
#include "gis/trk/CGisItemTrk.h"
#include "mouse/IMouse.h"
#include "canvas/CCanvas.h"
#include "CMainWindow.h"

CScrOptTrk::CScrOptTrk(CGisItemTrk * trk, IMouse *parent)
    : IScrOpt(parent)
    , QWidget(parent->getCanvas())
    , trk(trk)
{
    setupUi(this);

    QPointF pt = trk->getPointCloseBy(parent->getPoint().toPoint());
    move(pt.toPoint());
    adjustSize();
    show();
}

CScrOptTrk::~CScrOptTrk()
{

}

void CScrOptTrk::draw(QPainter& p)
{
    trk->drawHighlight(p);
}
