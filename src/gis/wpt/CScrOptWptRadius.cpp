/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
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


#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CGisWorkspace.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/wpt/CProjWpt.h"
#include "gis/wpt/CScrOptWptRadius.h"
#include "helpers/CDraw.h"
#include "mouse/IMouse.h"

#include <QtWidgets>

CScrOptWptRadius::CScrOptWptRadius(CGisItemWpt *wpt, const QPoint& point, IMouse *parent)
    : IScrOpt(parent)
    , key(wpt->getKey())
{
    setupUi(this);
    setOrigin(point);
    label->setFont(CMainWindow::self().getMapFont());

    qreal proximity = wpt->getProximity();
    if(proximity == NOFLOAT)
    {
        proximity = 0.;
    }

    QString val, unit;
    IUnit::self().meter2distance(proximity, val, unit);
    label->setText(QString("%1%2").arg(val).arg(unit));

    adjustSize();

    toolAvoid->setChecked(wpt->isAvoid());

    anchor = wpt->getPointCloseBy(point);
    move(anchor.toPoint() + QPoint(-width()/2,SCR_OPT_OFFSET));
    show();

    connect(toolDelete, &QToolButton::clicked, this, &CScrOptWptRadius::slotDelete);
    connect(toolEdit,   &QToolButton::clicked, this, &CScrOptWptRadius::slotEdit);
    connect(toolAvoid,  &QToolButton::clicked, this, &CScrOptWptRadius::slotAvoid);

    adjustSize();
}

CScrOptWptRadius::~CScrOptWptRadius()
{
}

void CScrOptWptRadius::slotDelete()
{
    IGisItem * item = CGisWorkspace::self().getItemByKey(key);
    if(nullptr == item)
    {
        deleteLater();
        return;
    }
    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt *>(item);
    wpt->setProximity(NOFLOAT);
    deleteLater();
}

void CScrOptWptRadius::slotAvoid()
{
    CGisWorkspace::self().toggleWptAvoid(key);
    deleteLater();
}

void CScrOptWptRadius::slotEdit()
{
    CGisWorkspace::self().editWptRadius(key);
    deleteLater();
}

void CScrOptWptRadius::draw(QPainter& p)
{
    IGisItem * item = CGisWorkspace::self().getItemByKey(key);
    if(nullptr == item)
    {
        deleteLater();
        return;
    }
    item->drawHighlight(p);

    CDraw::bubble(p, geometry(), anchor.toPoint());
}
