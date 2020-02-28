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
#include "gis/search/CGeoSearchWeb.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/wpt/CProjWpt.h"
#include "gis/wpt/CScrOptWpt.h"
#include "helpers/CDraw.h"
#include "mouse/CScrOptSemaphoreLocker.h"
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
    move(anchor.toPoint() + QPoint(-width()/2, SCR_OPT_OFFSET));
    show();

    connect(toolDelete,           &QToolButton::clicked, this, &CScrOptWpt::slotDelete);
    connect(toolTags,             &QToolButton::clicked, this, &CScrOptWpt::slotTags);
    connect(toolEdit,             &QToolButton::clicked, this, &CScrOptWpt::slotEdit);
    connect(toolCopy,             &QToolButton::clicked, this, &CScrOptWpt::slotCopy);
    connect(toolCoordToClipboard, &QToolButton::clicked, this, &CScrOptWpt::slotCoordToClipboard);
    connect(toolMove,             &QToolButton::clicked, this, &CScrOptWpt::slotMove);
    connect(toolProj,             &QToolButton::clicked, this, &CScrOptWpt::slotProj);
    connect(toolAddElevation,     &QToolButton::clicked, this, &CScrOptWpt::slotAddElevation);
    connect(toolBubble,           &QToolButton::clicked, this, &CScrOptWpt::slotBubble);
    connect(toolDelRadius,        &QToolButton::clicked, this, &CScrOptWpt::slotDeleteRadius);
    connect(toolNogoArea,         &QToolButton::clicked, this, &CScrOptWpt::slotNogoArea);
    connect(toolEditRadius,       &QToolButton::clicked, this, &CScrOptWpt::slotEditRadius);
    connect(toolSearchWeb,        &QToolButton::clicked, this, &CScrOptWpt::slotSearchWeb);

    connect(label, &QLabel::linkActivated, this, &CScrOptWpt::slotLinkActivated);

    adjustSize();
}

CScrOptWpt::~CScrOptWpt()
{
}

void CScrOptWpt::slotDelete()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().delItemByKey(key);
    close();
}

void CScrOptWpt::slotEdit()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().editItemByKey(key);
    close();
}

void CScrOptWpt::slotCopy()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().copyItemByKey(key);
    close();
}

void CScrOptWpt::slotCoordToClipboard()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().copyWptCoordByKey(key);
    close();
}

void CScrOptWpt::slotMove()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().moveWptByKey(key);
    close();
}

void CScrOptWpt::slotProj()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().projWptByKey(key);
    close();
}

void CScrOptWpt::slotBubble()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().toggleWptBubble(key);
    close();
}

void CScrOptWpt::slotDeleteRadius()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().deleteWptRadius(key);
    close();
}

void CScrOptWpt::slotNogoArea()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().toggleNogoItem(key);
    close();
}

void CScrOptWpt::slotEditRadius()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().editWptRadius(key);
    close();
}

void CScrOptWpt::slotAddElevation()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().addEleToWptTrkByKey({key});
    close();
}

void CScrOptWpt::slotSearchWeb()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().searchWebByKey(key);
    close();
}

void CScrOptWpt::slotTags()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().tagItemsByKey({key});
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

    CDraw::bubble(p, geometry(), anchor.toPoint(), backgroundColor);
}

