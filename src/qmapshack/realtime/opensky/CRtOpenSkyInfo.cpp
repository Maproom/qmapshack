/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#include "helpers/CSettings.h"
#include "realtime/opensky/CRtOpenSky.h"
#include "realtime/opensky/CRtOpenSkyInfo.h"
#include "realtime/opensky/CRtOpenSkyRecord.h"

#include <QtWidgets>

CRtOpenSkyInfo::CRtOpenSkyInfo(CRtOpenSky &source, QWidget *parent)
    : QWidget(parent)
    , source(source)
{
    setupUi(this);
    connect(&source, &CRtOpenSky::sigChanged, this, &CRtOpenSkyInfo::slotUpdate);
    connect(checkShowNames, &QCheckBox::toggled, &source, &CRtOpenSky::slotSetShowNames);
    connect(toolPause, &QToolButton::toggled, toolReset, &QToolButton::setEnabled);
    connect(toolPause, &QToolButton::toggled, toolFile, &QToolButton::setEnabled);
    connect(toolPause, &QToolButton::toggled, lineKey, &QLineEdit::setEnabled);
    connect(toolFile, &QToolButton::clicked, this, &CRtOpenSkyInfo::slotSetFilename);
    connect(toolReset, &QToolButton::clicked, this, &CRtOpenSkyInfo::slotResetRecord);
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

void CRtOpenSkyInfo::draw(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt)
{
    if(record != nullptr)
    {
        record->draw(p, viewport, blockedAreas, rt);
    }
}

void CRtOpenSkyInfo::slotUpdate()
{
    checkShowNames->setChecked(source.getShowNames());
    labelTimestamp->setText(source.getTimestamp().toString());
    labelNumberOfAircrafts->setText(QString::number(source.getNumberOfAircrafts()));

    if(!record.isNull() && toolRecord->isChecked())
    {
        bool ok = false;
        const CRtOpenSky::aircraft_t& aircraft = source.getAircraftByKey(lineKey->text(), ok);
        if(ok)
        {
            if(!record->writeEntry(aircraft))
            {
                QMessageBox::critical(this, tr("Error..."), tr("Failed to write record."), QMessageBox::Ok);
                toolPause->setChecked(true);
            }
        }
    }
}

void CRtOpenSkyInfo::slotSetFilename()
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

void CRtOpenSkyInfo::slotResetRecord()
{
    if(record == nullptr)
    {
        return;
    }

    int res = QMessageBox::question(this, tr("Reset record..."), tr("Do you really want to reset the current record?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
    if(res == QMessageBox::Yes)
    {
        record->reset();
        emit source.sigChanged();
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
        QMessageBox::critical(this, tr("Failed..."), tr("Failed to open record."), QMessageBox::Ok);
        delete record;
        return;
    }

    toolRecord->setEnabled(true);
}
