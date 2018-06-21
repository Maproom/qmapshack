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


#include "canvas/CCanvas.h"
#include "CMainWindow.h"
#include "gis/CGisWorkspace.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/wpt/CProjWpt.h"
#include "gis/wpt/CScrOptWpt.h"
#include "helpers/CDraw.h"
#include "mouse/IMouse.h"

#include <QtWidgets>

CScrOptWpt::CScrOptWpt(CGisItemWpt *wpt, const QPoint& point, IMouse *parent)
    : IScrOpt(parent)
    , key(wpt->getKey())
{
    setupUi(this);
    setOrigin(point);
    label->setFont(CMainWindow::self().getMapFont());
    label->setText(wpt->getInfo(IGisItem::eFeatureShowName|IGisItem::eFeatureShowLinks));
    adjustSize();

    toolProj->setDisabled(wpt->isGeocache() || wpt->isOnDevice());
    toolMove->setDisabled(wpt->isGeocache() || wpt->isOnDevice());
    photoAlbum->reload(wpt->getImages());
    toolBubble->setChecked(wpt->hasBubble());
    bool radius = wpt->hasRadius();
    toolNogoArea->setEnabled(radius);
    toolNogoArea->setChecked(radius && wpt->isNogo());
    toolDelRadius->setEnabled(radius);

    anchor = wpt->getPointCloseBy(point);
    move(anchor.toPoint() + QPoint(-width()/2,SCR_OPT_OFFSET));
    show();

//    connect(toolDelete,     &QToolButton::clicked, this, &CScrOptWpt::hide);
//    connect(toolEdit,       &QToolButton::clicked, this, &CScrOptWpt::hide);
//    connect(toolCopy,       &QToolButton::clicked, this, &CScrOptWpt::hide);
//    connect(toolMove,       &QToolButton::clicked, this, &CScrOptWpt::hide);
//    connect(toolProj,       &QToolButton::clicked, this, &CScrOptWpt::hide);
//    connect(toolBubble,     &QToolButton::clicked, this, &CScrOptWpt::hide);
//    connect(toolAddElevation,&QToolButton::clicked,this, &CScrOptWpt::hide);
//    connect(toolDelRadius,  &QToolButton::clicked, this, &CScrOptWpt::hide);
//    connect(toolNogoArea,   &QToolButton::clicked, this, &CScrOptWpt::hide);
//    connect(toolEditRadius, &QToolButton::clicked, this, &CScrOptWpt::hide);

    connect(toolDelete,     &QToolButton::clicked, this, &CScrOptWpt::slotDelete);
    connect(toolEdit,       &QToolButton::clicked, this, &CScrOptWpt::slotEdit);
    connect(toolCopy,       &QToolButton::clicked, this, &CScrOptWpt::slotCopy);
    connect(toolMove,       &QToolButton::clicked, this, &CScrOptWpt::slotMove);
    connect(toolProj,       &QToolButton::clicked, this, &CScrOptWpt::slotProj);
    connect(toolAddElevation,&QToolButton::clicked,this, &CScrOptWpt::slotAddElevation);
    connect(toolBubble,     &QToolButton::clicked, this, &CScrOptWpt::slotBubble);
    connect(toolDelRadius,  &QToolButton::clicked, this, &CScrOptWpt::slotDeleteRadius);
    connect(toolNogoArea,   &QToolButton::clicked, this, &CScrOptWpt::slotNogoArea);
    connect(toolEditRadius, &QToolButton::clicked, this, &CScrOptWpt::slotEditRadius);


    connect(label, &QLabel::linkActivated, this, &CScrOptWpt::slotLinkActivated);

    adjustSize();
}

CScrOptWpt::~CScrOptWpt()
{
}

void CScrOptWpt::slotDelete()
{
    CGisWorkspace::self().delItemByKey(key);
    close();
}

void CScrOptWpt::slotEdit()
{
    CGisWorkspace::self().editItemByKey(key);
    close();
}

void CScrOptWpt::slotCopy()
{
    CGisWorkspace::self().copyItemByKey(key);
    close();
}

void CScrOptWpt::slotMove()
{
    CGisWorkspace::self().moveWptByKey(key);
    close();
}

void CScrOptWpt::slotProj()
{
    CGisWorkspace::self().projWptByKey(key);
    close();
}

void CScrOptWpt::slotBubble()
{
    CGisWorkspace::self().toggleWptBubble(key);
    close();
}

void CScrOptWpt::slotDeleteRadius()
{
    CGisWorkspace::self().deleteWptRadius(key);
    close();
}

void CScrOptWpt::slotNogoArea()
{
    CGisWorkspace::self().toggleNogoItem(key);
    close();
}

void CScrOptWpt::slotEditRadius()
{
    CGisWorkspace::self().editWptRadius(key);
    close();
}

void CScrOptWpt::slotAddElevation()
{
    QList<IGisItem::key_t> keys;
    keys << key;
    CGisWorkspace::self().addEleToWptTrkByKey(keys);
    close();
}

void CScrOptWpt::draw(QPainter& p)
{
    IGisItem * item = CGisWorkspace::self().getItemByKey(key);
    if(nullptr == item)
    {
        close();
        return;
    }
    item->drawHighlight(p);

    CDraw::bubble(p, geometry(), anchor.toPoint());
}
