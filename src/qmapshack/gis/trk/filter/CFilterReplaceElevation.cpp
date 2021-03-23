/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "CMainWindow.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/filter/CFilterReplaceElevation.h"

CFilterReplaceElevation::CFilterReplaceElevation(CGisItemTrk &trk, QWidget *parent)
    : QWidget(parent)
    , trk(trk)
{
    setupUi(this);
    updateUi();

    connect(toolApply, &QToolButton::clicked, this, &CFilterReplaceElevation::slotApply);
    connect(comboView, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CFilterReplaceElevation::updateUi);
}

void CFilterReplaceElevation::slotApply()
{
    CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);

    CCanvas * canvas = comboView->currentData().value<CCanvas*>();
    if(canvas != nullptr)
    {
        trk.filterReplaceElevation(canvas);
    }
}


void CFilterReplaceElevation::updateUi()
{
    comboView->blockSignals(true);
    QString current = comboView->currentText();
    comboView->clear();

    const QList<CCanvas*>& list = CMainWindow::self().getCanvas();
    for(CCanvas * canvas : list)
    {
        comboView->addItem(canvas->objectName(), QVariant::fromValue<CCanvas*>(canvas));
    }

    if(!current.isEmpty())
    {
        comboView->setCurrentIndex(comboView->findText(current));
    }

    toolApply->setEnabled(comboView->currentIndex() != NOIDX);
    comboView->blockSignals(false);
}
