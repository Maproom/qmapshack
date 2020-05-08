/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "items/CItemCutMap.h"
#include "overlay/COverlayCutMap.h"

#include <QtWidgets>

CItemCutMap::CItemCutMap(const QString &filename, QStackedWidget *stackedWidget, QListWidget *parent)
    : CItemFile(filename, parent)
{
    overlay = new COverlayCutMap(this, stackedWidget);
    connect(overlay, &COverlayCutMap::sigChanged, this, &CItemCutMap::sigChanged);
}

CItemCutMap::~CItemCutMap()
{
    overlay->deleteLater();
}

void CItemCutMap::saveSettings(QSettings& cfg)
{
    CItemFile::saveSettings(cfg);
    overlay->saveSettings(cfg);
}

void CItemCutMap::loadSettings(QSettings& cfg)
{
    CItemFile::loadSettings(cfg);
    overlay->loadSettings(cfg);
}

void CItemCutMap::toFront()
{
    overlay->toFront();
}

bool CItemCutMap::drawFx(QPainter& p, CCanvas::redraw_e needsRedraw)
{
    CItemFile::drawFx(p, needsRedraw);
    overlay->drawFx(p, needsRedraw);
    return true;
}

void CItemCutMap::mouseMoveEventFx(QMouseEvent *e)
{
    CItemFile::mouseMoveEventFx(e);
    if(!mapIsMoving)
    {
        overlay->mouseMoveEventFx(e);
    }
}

void CItemCutMap::mouseReleaseEventFx(QMouseEvent *e)
{
    if(!mapDidMove)
    {
        overlay->mouseReleaseEventFx(e);
    }
    CItemFile::mouseReleaseEventFx(e);
}

void CItemCutMap::leaveEventFx(QEvent *e)
{
    CItemFile::leaveEventFx(e);
    overlay->abortStep();
}

QCursor CItemCutMap::getCursorFx()
{
    return overlay->getCursorFx();
}

void CItemCutMap::saveShape(const QString& filename) const
{
    overlay->saveShape(filename);
}

bool CItemCutMap::isOk() const
{
    return overlay->isOk();
}
