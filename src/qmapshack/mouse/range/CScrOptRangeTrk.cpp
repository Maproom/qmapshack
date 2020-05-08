/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CMainWindow.h"
#include "gis/trk/CGisItemTrk.h"
#include "helpers/CDraw.h"
#include "mouse/range/CScrOptRangeTrk.h"

#include <QtWidgets>

CScrOptRangeTrk::CScrOptRangeTrk(const QPointF &point, CGisItemTrk * trk, IMouse *mouse, QWidget *parent)
    : IScrOpt(mouse)
{
    if(parent != nullptr)
    {
        setParent(parent);
    }
    setupUi(this);

    bool noRange = !trk->isRangeSelected();

    label->setFont(CMainWindow::self().getMapFont());
    label->setText(noRange ?  tr("No range selected") : trk->getInfoRange());
    adjustSize();

    setOrigin(point.toPoint());
    moveTo(point.toPoint());
    show();

    toolCopy->setDisabled(noRange);
    toolActivity->setDisabled(noRange);
}

CScrOptRangeTrk::~CScrOptRangeTrk()
{
}

void CScrOptRangeTrk::draw(QPainter& p)
{
    if(isVisible())
    {
        CDraw::bubble(p, geometry(), origin, backgroundColor);
    }
}

