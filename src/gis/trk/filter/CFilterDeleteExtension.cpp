/**********************************************************************************************
    Copyright (C) 2016 Christian Eichler code@christian-eichler.de

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
#include "gis/trk/CKnownExtension.h"
#include "gis/trk/filter/CFilterDeleteExtension.h"



CFilterDeleteExtension::CFilterDeleteExtension(CGisItemTrk &trk, QWidget *parent)
    : QWidget(parent)
    , trk(trk)
{
    setupUi(this);

    update();

    connect(toolApply, &QToolButton::clicked, this, &CFilterDeleteExtension::slotApply);
}

CFilterDeleteExtension::~CFilterDeleteExtension()
{
}

void CFilterDeleteExtension::update()
{
    comboExtensions->clear();

    foreach(const QString &key, trk.getExistingDataSources())
    {
        const CKnownExtension &ext = CKnownExtension::get(key);
        if(!ext.derivedQMS)
        {
            QIcon icon(ext.icon);
            comboExtensions->addItem(icon, ext.known ? ext.name : key, key);
        }
    }

    bool enabled = (0 != comboExtensions->count());
    toolApply->setEnabled(enabled);
    comboExtensions->setEnabled(enabled);

    if(!enabled)
    {
        comboExtensions->addItem(tr("No extension available"), "");
    }
}

void CFilterDeleteExtension::slotApply()
{
    CCanvas::setOverrideCursor(Qt::WaitCursor,"CFilterDeleteExtension");

    int idx = comboExtensions->currentIndex();
    trk.filterDeleteExtension(comboExtensions->itemData(idx).toString());

    CCanvas::restoreOverrideCursor("CFilterDeleteExtension");
}

