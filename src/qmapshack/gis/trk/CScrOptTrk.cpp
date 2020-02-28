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

#include "CMainWindow.h"
#include "gis/CGisWorkspace.h"
#include "gis/prj/IGisProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CScrOptTrk.h"
#include "helpers/CDraw.h"
#include "mouse/CScrOptSemaphoreLocker.h"
#include "mouse/IMouse.h"

CScrOptTrk::CScrOptTrk(CGisItemTrk * trk, const QPoint& point, IMouse *parent)
    : IScrOpt(parent)
    , key(trk->getKey())
{
    setupUi(this);
    setOrigin(point);
    label->setFont(CMainWindow::self().getMapFont());
    label->setText(trk->getInfo(IGisItem::eFeatureShowName|IGisItem::eFeatureShowActivity|IGisItem::eFeatureShowLinks));
    adjustSize();

    toolProfile->setChecked(trk->hasUserFocus());

    bool isOnDevice = trk->isOnDevice();
    toolCut->setDisabled(isOnDevice);
    toolEdit->setDisabled(isOnDevice);
    toolReverse->setDisabled(isOnDevice);
    toolRange->setDisabled(isOnDevice);
    toolCopyWithWpt->setEnabled(trk->getNumberOfAttachedWpt() != 0);
    toolNogo->setChecked(trk->isNogo());

    IGisProject * project = trk->getParentProject();
    if(project != nullptr)
    {
        toolCombine->setEnabled(project->getItemCountByType(IGisItem::eTypeTrk) > 1);
    }

    anchor = trk->getPointCloseBy(point);
    if((anchor - point).manhattanLength() > 50)
    {
        anchor = point;
        toolCut->setEnabled(false);
    }

    move(anchor.toPoint() + QPoint(-width()/2, SCR_OPT_OFFSET));
    show();

    connect(toolEditDetails, &QToolButton::clicked, this, &CScrOptTrk::slotEditDetails);
    connect(toolTags,        &QToolButton::clicked, this, &CScrOptTrk::slotTags);
    connect(toolDelete,      &QToolButton::clicked, this, &CScrOptTrk::slotDelete);
    connect(toolCopy,        &QToolButton::clicked, this, &CScrOptTrk::slotCopy);
    connect(toolProfile,     &QToolButton::toggled, this, &CScrOptTrk::slotProfile);
    connect(toolCut,         &QToolButton::clicked, this, &CScrOptTrk::slotCut);
    connect(toolEdit,        &QToolButton::clicked, this, &CScrOptTrk::slotEdit);
    connect(toolReverse,     &QToolButton::clicked, this, &CScrOptTrk::slotReverse);
    connect(toolCombine,     &QToolButton::clicked, this, &CScrOptTrk::slotCombine);
    connect(toolRange,       &QToolButton::clicked, this, &CScrOptTrk::slotRange);
    connect(toolActivity,    &QToolButton::clicked, this, &CScrOptTrk::slotActivity);
    connect(toolColor,       &QToolButton::clicked, this, &CScrOptTrk::slotColor);
    connect(toolCopyWithWpt, &QToolButton::clicked, this, &CScrOptTrk::slotCopyWithWpt);
    connect(toolNogo,        &QToolButton::clicked, this, &CScrOptTrk::slotNogo);
    connect(toolAddElevation, &QToolButton::clicked, this, &CScrOptTrk::slotAddElevation);
    connect(toolAddInfo,     &QToolButton::clicked, this, &CScrOptTrk::slotAddInfo);

    connect(label, &QLabel::linkActivated, this, &CScrOptTrk::slotLinkActivated);

    // reset user focus if the track has it
    trk->setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseMove, "CScrOptTrk");
    trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseClick, "CScrOptTrk");
}

CScrOptTrk::~CScrOptTrk()
{
}

void CScrOptTrk::slotDelete()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().delItemByKey(key);
    close();
}

void CScrOptTrk::slotCopy()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().copyItemByKey(key);
    close();
}

void CScrOptTrk::slotEditDetails()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().editItemByKey(key);
    close();
}

void CScrOptTrk::slotProfile(bool on)
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().focusTrkByKey(on, key);
    close();
}

void CScrOptTrk::slotCut()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().cutTrkByKey(key);
    close();
}

void CScrOptTrk::slotEdit()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().editTrkByKey(key);
    close();
}

void CScrOptTrk::slotReverse()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().reverseTrkByKey(key);
    close();
}

void CScrOptTrk::slotCombine()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().combineTrkByKey(key);
    close();
}

void CScrOptTrk::slotRange()
{
    CScrOptSemaphoreLocker lock(*this);
    mouse = nullptr;
    CGisWorkspace::self().rangeTrkByKey(key);
    close();
}

void CScrOptTrk::slotActivity()
{
    CScrOptSemaphoreLocker lock(*this);
    CActivityTrk::getMenu(key, this, true);
    close();
}

void CScrOptTrk::slotColor()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().colorTrkByKey({key});
    close();
}

void CScrOptTrk::slotCopyWithWpt()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().copyTrkWithWptByKey(key);
    close();
}

void CScrOptTrk::slotNogo()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().toggleNogoItem(key);
    close();
}


void CScrOptTrk::slotAddElevation()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().addEleToWptTrkByKey({key});
    close();
}

void CScrOptTrk::slotAddInfo()
{
    close();

    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().addTrkInfoByKey(key);
}

void CScrOptTrk::slotTags()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().tagItemsByKey({key});
    close();
}

void CScrOptTrk::draw(QPainter& p)
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
