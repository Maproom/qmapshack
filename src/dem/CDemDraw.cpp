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

#include "CCanvas.h"
#include "CMainWindow.h"
#include "dem/CDemDraw.h"
#include "dem/CDemList.h"
#include "dem/CDemPathSetup.h"
#include "units/IUnit.h"

#include <QtWidgets>


QList<CDemDraw*> CDemDraw::dems;
QStringList CDemDraw::demPaths;
QStringList CDemDraw::supportedFormats = QString("*.vrt").split('|');


CDemDraw::CDemDraw(CCanvas *canvas)
    : IDrawContext(canvas)
{

    demList = new CDemList(canvas);
    CMainWindow::self().addDemList(demList, canvas->objectName());
    connect(canvas, SIGNAL(destroyed()), demList, SLOT(deleteLater()));
    connect(demList, SIGNAL(sigChanged()), this, SLOT(emitSigCanvasUpdate()));

    dems << this;
}

CDemDraw::~CDemDraw()
{
    dems.removeOne(this);
}

void CDemDraw::setupDemPath()
{
    CDemPathSetup dlg(demPaths);
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    foreach(CDemDraw * dem, dems)
    {
        QStringList keys;
//        map->saveActiveMapsList(keys);
//        map->buildMapList();
//        map->restoreActiveMapsList(keys);
    }
}

void CDemDraw::saveDemPath(QSettings& cfg)
{
    cfg.setValue("demPaths", demPaths);
}

void CDemDraw::loadDemPath(QSettings& cfg)
{
    demPaths = cfg.value("demPaths", demPaths).toStringList();
}

void CDemDraw::saveConfig(QSettings& cfg)
{

}

void CDemDraw::loadConfig(QSettings& cfg)
{

}


qreal CDemDraw::getElevation(const QPointF& pos)
{
    return NOFLOAT;
}

void CDemDraw::drawt(buffer_t& currentBuffer)
{
    msleep(500);
}
