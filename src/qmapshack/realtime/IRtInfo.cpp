/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/CGisWorkspace.h"
#include "gis/trk/CGisItemTrk.h"
#include "helpers/CSettings.h"
#include "realtime/IRtInfo.h"

#include <QtWidgets>

IRtInfo::IRtInfo(IRtSource* source, QWidget* parent)
    : QWidget(parent)
    , source(source)
{
}


void IRtInfo::slotSetFilename()
{
    SETTINGS;
    QString path = cfg.value("Paths/realtimeData", QDir::homePath()).toString();
    QString filename = QFileDialog::getSaveFileName( this, tr("Select record file"), path, "QMapShack Record (*.rec)");

    if(filename.isEmpty())
    {
        return;
    }
    QFileInfo fi(filename);
    if(fi.suffix().toLower() != "rec")
    {
        filename += ".rec";
    }

    startRecord(filename);

    path = fi.absolutePath();
    cfg.setValue("Paths/realtimeData", path);
}

void IRtInfo::slotResetRecord()
{
    if(record == nullptr)
    {
        return;
    }

    int res = QMessageBox::question(this, tr("Reset record..."), tr("Do you really want to reset the current record?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(res == QMessageBox::Yes)
    {
        record->reset();
        emit source->sigChanged();
    }
}

void IRtInfo::slotToTrack()
{
    if(record == nullptr)
    {
        return;
    }

    IGisProject* prj = CGisWorkspace::self().selectProject(false);
    if(prj == nullptr)
    {
        return;
    }

    CTrackData data;
    fillTrackData(data);

    new CGisItemTrk(data, prj);
}


void IRtInfo::draw(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw* rt)
{
    if(record != nullptr)
    {
        record->draw(p, viewport, blockedAreas, rt);
    }
}
