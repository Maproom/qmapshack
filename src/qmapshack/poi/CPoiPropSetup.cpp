/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "helpers/Signals.h"
#include "poi/CPoiDraw.h"
#include "poi/CPoiPOI.h"
#include "poi/CPoiPropSetup.h"
#include "poi/IPoi.h"

CPoiPropSetup::CPoiPropSetup(IPoi* poifile, CPoiDraw* poi)
    : IPoiProp(poifile, poi)
{
    setupUi(this);
    CPoiPropSetup::slotPropertiesChanged();

    connect(sliderOpacity, &QSlider::valueChanged, poifile, &IPoi::slotSetOpacity);
    connect(sliderOpacity, &QSlider::valueChanged, poi, &CPoiDraw::emitSigCanvasUpdate);

    poifile->addTreeWidgetItems(treeWidgetCategories);
    treeWidgetCategories->sortItems(eTreeColumnDisplayName, Qt::SortOrder::AscendingOrder);
    connect(treeWidgetCategories, SIGNAL(itemChanged(QTreeWidgetItem*,int)), poifile, SLOT(slotCheckedStateChanged(QTreeWidgetItem*)));
}


void CPoiPropSetup::slotPropertiesChanged()
{
    X______________BlockAllSignals______________X(this);
    sliderOpacity->setValue(poifile->getOpacity());

    poi->emitSigCanvasUpdate();
    X_____________UnBlockAllSignals_____________X(this);
}
