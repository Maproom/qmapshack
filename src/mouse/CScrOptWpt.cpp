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


#include "mouse/CScrOptWpt.h"
#include "mouse/IMouse.h"
#include "gis/wpt/CGisItemWpt.h"
#include "canvas/CCanvas.h"
#include "CMainWindow.h"

#include <QtWidgets>

CScrOptWpt::CScrOptWpt(CGisItemWpt *wpt, IMouse *parent)
    : IScrOpt(parent)
    , QWidget(parent->getCanvas())
    , wpt(wpt)
{
    setupUi(this);
    label->setFont(CMainWindow::self().getMapFont());

    label->setText(wpt->getInfo());

    QPointF pt = wpt->getPointCloseBy(mousePos);
    move(pt.toPoint());
    adjustSize();
    show();
}

CScrOptWpt::~CScrOptWpt()
{

}

