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
#include "realtime/opensky/CRtOpenSky.h"
#include "realtime/opensky/CRtOpenSkyInfo.h"
#include "realtime/opensky/CRtOpenSkyRecord.h"

#include <QtWidgets>

CRtOpenSkyInfo::CRtOpenSkyInfo(CRtOpenSky &source, QWidget *parent)
    : IRtInfo(&source, parent)
{
    setupUi(this);
    connect(&source, &CRtOpenSky::sigChanged, this, &CRtOpenSkyInfo::slotUpdate);
    connect(checkShowNames, &QCheckBox::toggled, &source, &CRtOpenSky::slotSetShowNames);
    connect(toolPause, &QToolButton::toggled, toolReset, &QToolButton::setEnabled);
    connect(toolPause, &QToolButton::toggled, toolFile, &QToolButton::setEnabled);
    connect(toolPause, &QToolButton::toggled, toolToTrack, &QToolButton::setEnabled);
    connect(toolPause, &QToolButton::toggled, lineKey, &QLineEdit::setEnabled);
    connect(toolFile, &QToolButton::clicked, this, &CRtOpenSkyInfo::slotSetFilename);
    connect(toolReset, &QToolButton::clicked, this, &CRtOpenSkyInfo::slotResetRecord);
    connect(toolToTrack, &QToolButton::clicked, this, &CRtOpenSkyInfo::slotToTrack);
}

void CRtOpenSkyInfo::loadSettings(QSettings& cfg)
{
    lineKey->setText(cfg.value("callsign", "").toString());
    startRecord(cfg.value("filename", "").toString());
}

void CRtOpenSkyInfo::saveSettings(QSettings& cfg) const
{
    cfg.setValue("callsign", lineKey->text());
    cfg.setValue("filename", toolFile->toolTip());
}

void CRtOpenSkyInfo::slotUpdate()
{
    CRtOpenSky * _source = dynamic_cast<CRtOpenSky*>(source.data());
    CRtOpenSkyRecord * _record = dynamic_cast<CRtOpenSkyRecord*>(record.data());

    checkShowNames->setChecked(_source->getShowNames());
    labelTimestamp->setText(_source->getTimestamp().toString());
    labelNumberOfAircrafts->setText(QString::number(_source->getNumberOfAircrafts()));

    if(!record.isNull() && toolRecord->isChecked())
    {
        bool ok = false;
        const CRtOpenSky::aircraft_t& aircraft = _source->getAircraftByKey(lineKey->text(), ok);
        if(ok)
        {
            if(!_record->writeEntry(aircraft))
            {
                QMessageBox::critical(this, tr("Error..."), record->getError(), QMessageBox::Ok);
                toolPause->setChecked(true);
            }
        }
    }
}


void CRtOpenSkyInfo::startRecord(const QString& filename)
{
    delete record;

    toolFile->setToolTip(filename);
    toolRecord->setEnabled(false);

    if(filename.trimmed().isEmpty())
    {
        return;
    }

    record = new CRtOpenSkyRecord(this);

    if(!record->setFile(filename))
    {
        delete record;
        QMessageBox::critical(this, tr("Failed..."), record->getError(), QMessageBox::Ok);
        return;
    }

    toolRecord->setEnabled(true);
}

void CRtOpenSkyInfo::fillTrackData(CTrackData& data)
{
    CTrackData::trkseg_t seg;
    seg.pts = record->getTrack();
    data.segs << seg;
    data.name = lineKey->text();
}
