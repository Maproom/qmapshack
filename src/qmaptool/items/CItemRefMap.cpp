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

#include "items/CItemRefMap.h"
#include "overlay/COverlayRefMap.h"
#include "overlay/refmap/COverlayRefMapPoint.h"

#include <QtWidgets>

CItemRefMap::CItemRefMap(const QString &filename, QStackedWidget *stackedWidget, QListWidget *parent)
    : CItemFile(filename, parent)
{
    overlay = new COverlayRefMap(this, stackedWidget);
    connect(overlay, &COverlayRefMap::sigChanged, this, &CItemRefMap::sigChanged);
}

CItemRefMap::~CItemRefMap()
{
    overlay->deleteLater();
}

void CItemRefMap::saveSettings(QSettings& cfg)
{
    CItemFile::saveSettings(cfg);
    overlay->saveSettings(cfg);
}

void CItemRefMap::loadSettings(QSettings& cfg)
{
    CItemFile::loadSettings(cfg);
    overlay->loadSettings(cfg);
}

void CItemRefMap::addRefPoints(QList<COverlayRefMapPoint*>& points)
{
    overlay->addRefPoints(points);
}

QString CItemRefMap::getMapProjection() const
{
    return overlay->getMapProjection();
}

const QList<COverlayRefMapPoint *> CItemRefMap::getRefPoints() const
{
    return overlay->getRefPoints();
}


void CItemRefMap::toFront()
{
    overlay->toFront();
}

bool CItemRefMap::drawFx(QPainter& p, CCanvas::redraw_e needsRedraw)
{
    CItemFile::drawFx(p, needsRedraw);
    overlay->drawFx(p, needsRedraw);
    return true;
}

void CItemRefMap::mouseMoveEventFx(QMouseEvent *e)
{
    CItemFile::mouseMoveEventFx(e);
    if(!mapIsMoving)
    {
        overlay->mouseMoveEventFx(e);
    }
}

void CItemRefMap::mouseReleaseEventFx(QMouseEvent *e)
{
    if(!mapDidMove)
    {
        overlay->mouseReleaseEventFx(e);
    }
    CItemFile::mouseReleaseEventFx(e);
}

bool CItemRefMap::keyPressEventFx(QKeyEvent *e)
{
    return overlay->keyPressEventFx(e);
}

void CItemRefMap::leaveEventFx(QEvent *e)
{
    CItemFile::leaveEventFx(e);
    overlay->abortStep();
}

QCursor CItemRefMap::getCursorFx()
{
    return overlay->getCursorFx();
}

bool CItemRefMap::isOk() const
{
    return overlay->isOk();
}
