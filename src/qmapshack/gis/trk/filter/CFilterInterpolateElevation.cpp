/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/filter/CFilterInterpolateElevation.h"
#include "helpers/CSettings.h"

CFilterInterpolateElevation::CFilterInterpolateElevation(CGisItemTrk &trk, QWidget *parent)
    : QWidget(parent)
    , trk(trk)
{
    setupUi(this);

    comboQuality->addItem(tr("coarse"), CGisItemTrk::eQualityCoarse);
    comboQuality->addItem(tr("medium"), CGisItemTrk::eQualityMedium);
    comboQuality->addItem(tr("fine"), CGisItemTrk::eQualityFine);

    bool enabled = trk.isInterpolationEnabled();
    checkPreview->setChecked(enabled);
    toolApply->setEnabled(enabled);

    SETTINGS;
    comboQuality->setCurrentIndex(comboQuality->findData(cfg.value("TrackDetails/Filter/Interp/quality", CGisItemTrk::eQualityCoarse)));

    connect(toolApply, &QToolButton::clicked, this, &CFilterInterpolateElevation::slotApply);
    connect(checkPreview, &QCheckBox::toggled, this, &CFilterInterpolateElevation::slotPreview);
    connect(comboQuality, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CFilterInterpolateElevation::slotPreview);
}

CFilterInterpolateElevation::~CFilterInterpolateElevation()
{
    SETTINGS;
    cfg.setValue("TrackDetails/Filter/Interp/quality", comboQuality->currentData());
    trk.setupInterpolation(false, CGisItemTrk::eQualityCoarse);
}

void CFilterInterpolateElevation::slotApply()
{
    CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
    trk.filterInterpolateElevation();
    checkPreview->setChecked(trk.isInterpolationEnabled());
}

void CFilterInterpolateElevation::slotPreview()
{
    CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
    bool yes = checkPreview->isChecked();
    qint32 Q = comboQuality->currentData().toInt();
    trk.setupInterpolation(yes, Q);

    yes = trk.isInterpolationEnabled();
    checkPreview->setChecked(yes);
    toolApply->setEnabled(yes);
}
