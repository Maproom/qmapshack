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

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/qlb/CQlbProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "qlgt/CQlb.h"
#include "qlgt/CQlgtDiary.h"
#include "qlgt/CQlgtRoute.h"
#include "qlgt/CQlgtTrack.h"
#include "qlgt/CQlgtWpt.h"
#include "qlgt/IQlgtOverlay.h"

#include <QtWidgets>

CQlbProject::CQlbProject(const QString& filename, CGisListWks* parent)
    : IGisProject(eTypeQlb, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/QlbProject.png"));

    // create file instance
    QFile file(filename);

    // if the file does not exist, the filename is assumed to be a name for a new project
    if(!file.exists() || QFileInfo(filename).suffix().toLower() != "qlb")
    {
        IGisProject::filename.clear();
        setupName(filename);
        setToolTip(CGisListWks::eColumnName, getInfo());
        valid = true;
        return;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Failed to open..."), tr("Failed to open %1").arg(filename), QMessageBox::Abort);
        return;
    }

    load(filename);

    markAsSaved();
    setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
    setToolTip(CGisListWks::eColumnName, getInfo());
    updateItems();
    valid = true;
}

void CQlbProject::load(const QString& filename)
{
    CQlb qlb(nullptr);
    qlb.load(filename);

    loadWpts(qlb.waypoints());
    loadTrks(qlb.tracks());
    loadRtes(qlb.routes());
    loadOvls(qlb.overlays());
}

void CQlbProject::loadWpts(QByteArray& array)
{
    QDataStream stream(&array, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_4_5);

    while(!stream.atEnd())
    {
        CQlgtWpt wpt(0, nullptr);
        stream >> wpt;
        new CGisItemWpt(wpt, this);
    }
}

void CQlbProject::loadTrks(QByteArray& array)
{
    QDataStream stream(&array, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_4_5);

    while(!stream.atEnd())
    {
        CQlgtTrack trk(0, nullptr);
        stream >> trk;
        new CGisItemTrk(trk, this);
    }
}

void CQlbProject::loadRtes(QByteArray& array)
{
    QDataStream stream(&array, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_4_5);

    while(!stream.atEnd())
    {
        CQlgtRoute rte(0, nullptr);
        stream >> rte;
        new CGisItemRte(rte, this);
    }
}

void CQlbProject::loadOvls(QByteArray& array)
{
    QDataStream stream(&array, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_4_5);

    bool warningDone = false;
    while(!stream.atEnd())
    {
        IQlgtOverlay ovl(0, nullptr);
        stream >> ovl;
        if(ovl.type == "Area")
        {
            new CGisItemOvlArea(ovl, this);
        }
        else if(ovl.type == "Distance")
        {
            new CGisItemTrk(ovl, this);
        }
        else if(!warningDone)
        {
            QMessageBox::warning(CMainWindow::self().getBestWidgetForParent(), tr("Could not convert...")
                                 , tr("The file contains overlays that can not be converted. This is because "
                                      "QMapShack does not support all overlay types of QLandkarte."), QMessageBox::Ok);
            warningDone = true;
        }
    }
}

