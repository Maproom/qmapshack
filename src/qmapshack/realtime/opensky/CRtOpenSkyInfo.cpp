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

#include <QtWidgets>

CRtOpenSkyInfo::CRtOpenSkyInfo(CRtOpenSky &source, QWidget *parent)
    : QWidget(parent)
    , source(source)
{
    setupUi(this);
    connect(&source, &CRtOpenSky::sigChanged, this, &CRtOpenSkyInfo::slotUpdate);
    connect(checkShowNames, &QCheckBox::toggled, &source, &CRtOpenSky::slotSetShowNames);
    connect(toolPause, &QToolButton::toggled, toolDelete, &QToolButton::setEnabled);
    connect(toolPause, &QToolButton::toggled, toolFile, &QToolButton::setEnabled);
    connect(toolPause, &QToolButton::toggled, lineCallsign, &QLineEdit::setEnabled);
    connect(toolFile, &QToolButton::clicked, this, &CRtOpenSkyInfo::slotSetFilename);
}

void CRtOpenSkyInfo::loadSettings(QSettings& cfg)
{
    lineCallsign->setText(cfg.value("callsign", "").toString());
    toolFile->setToolTip(cfg.value("filename", "").toString());
}

void CRtOpenSkyInfo::saveSettings(QSettings& cfg) const
{
    cfg.setValue("callsign", lineCallsign->text());
    cfg.setValue("filename", toolFile->toolTip());
}

void CRtOpenSkyInfo::slotUpdate()
{
    checkShowNames->setChecked(source.getShowNames());
    labelTimestamp->setText(source.getTimestamp().toString());
    labelNumberOfAircrafts->setText(QString::number(source.getNumberOfAircrafts()));
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

    toolFile->setToolTip(filename);

    path = fi.absolutePath();
    cfg.setValue("Paths/realtimeData", path);
}
