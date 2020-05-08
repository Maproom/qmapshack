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
#include "helpers/CSettings.h"
#include "mouse/CMouseAdapter.h"
#include "mouse/CScrOptSelect.h"
#include "mouse/IMouse.h"

#include <QtWidgets>

CScrOptSelect::CScrOptSelect(IMouse *mouse)
    : IScrOpt(mouse)
{
    setupUi(this);
    adjustSize();

    SETTINGS;
    cfg.beginGroup("Selection");
    toolModeExact->setChecked(cfg.value("modeExact", toolModeExact->isChecked()).toBool());
    toolModeIntersect->setChecked(cfg.value("modeIntersect", toolModeIntersect->isChecked()).toBool());
    toolItemTrk->setChecked(cfg.value("itemTrk", toolItemTrk->isChecked()).toBool());
    toolItemWpt->setChecked(cfg.value("itemWpt", toolItemWpt->isChecked()).toBool());
    toolItemRte->setChecked(cfg.value("itemRte", toolItemRte->isChecked()).toBool());
    toolItemOvl->setChecked(cfg.value("itemOvl", toolItemOvl->isChecked()).toBool());
    cfg.endGroup(); //Selection

    modeSelection |= toolModeExact->isChecked() ? IGisItem::eSelectionExact : 0;
    modeSelection |= toolModeIntersect->isChecked() ? IGisItem::eSelectionIntersect : 0;
    modeSelection |= toolItemTrk->isChecked() ? IGisItem::eSelectionTrk : 0;
    modeSelection |= toolItemWpt->isChecked() ? IGisItem::eSelectionWpt : 0;
    modeSelection |= toolItemRte->isChecked() ? IGisItem::eSelectionRte : 0;
    modeSelection |= toolItemOvl->isChecked() ? IGisItem::eSelectionOvl : 0;

    connect(toolModeExact,     &QToolButton::toggled, this, [this](bool checked){slotModeSwitch(IGisItem::eSelectionExact, checked);});
    connect(toolModeIntersect, &QToolButton::toggled, this, [this](bool checked){slotModeSwitch(IGisItem::eSelectionIntersect, checked);});
    connect(toolItemTrk,       &QToolButton::toggled, this, [this](bool checked){slotModeSwitch(IGisItem::eSelectionTrk, checked);});
    connect(toolItemWpt,       &QToolButton::toggled, this, [this](bool checked){slotModeSwitch(IGisItem::eSelectionWpt, checked);});
    connect(toolItemRte,       &QToolButton::toggled, this, [this](bool checked){slotModeSwitch(IGisItem::eSelectionRte, checked);});
    connect(toolItemOvl,       &QToolButton::toggled, this, [this](bool checked){slotModeSwitch(IGisItem::eSelectionOvl, checked);});
}

CScrOptSelect::~CScrOptSelect()
{
    SETTINGS;
    cfg.beginGroup("Selection");
    cfg.setValue("modeExact", toolModeExact->isChecked());
    cfg.setValue("modeIntersect", toolModeIntersect->isChecked());
    cfg.setValue("itemTrk", toolItemTrk->isChecked());
    cfg.setValue("itemWpt", toolItemWpt->isChecked());
    cfg.setValue("itemRte", toolItemRte->isChecked());
    cfg.setValue("itemOvl", toolItemOvl->isChecked());
    cfg.endGroup(); //Selection

    mouse->getCanvas()->reportStatus("CScrOptSelect", "");
}

void CScrOptSelect::slotModeSwitch(IGisItem::selection_e mode, bool checked)
{
    CCanvas * canvas = mouse->getCanvas();

    if(checked)
    {
        modeSelection |= mode;

        switch(mode)
        {
        case IGisItem::eSelectionExact:
            canvas->reportStatus("CScrOptSelect", tr("<b>Exact Mode</b><br/>All selected items have to be completely inside the selected area.<br/>"));
            break;

        case IGisItem::eSelectionIntersect:
            canvas->reportStatus("CScrOptSelect", tr("<b>Intersecting Mode</b><br/>All selected items have to be inside or at least intersect the selected area.<br/>"));
            break;

        case IGisItem::eSelectionTrk:
            canvas->reportStatus("CScrOptSelect", tr("<b>Add Tracks</b><br/>Add tracks to list of selected items<br/>"));
            break;

        case IGisItem::eSelectionWpt:
            canvas->reportStatus("CScrOptSelect", tr("<b>Add Waypoints</b><br/>Add waypoints to list of selected items<br/>"));
            break;

        case IGisItem::eSelectionRte:
            canvas->reportStatus("CScrOptSelect", tr("<b>Add Routes</b><br/>Add routes to list of selected items<br/>"));
            break;

        case IGisItem::eSelectionOvl:
            canvas->reportStatus("CScrOptSelect", tr("<b>Add Areas</b><br/>Add areas to list of selected items<br/>"));
            break;
        }
    }
    else
    {
        modeSelection &= ~mode;

        switch(mode)
        {
        case IGisItem::eSelectionTrk:
            canvas->reportStatus("CScrOptSelect", tr("<b>Ignore Tracks</b><br/>Ignore tracks in list of selected items<br/>"));
            break;

        case IGisItem::eSelectionWpt:
            canvas->reportStatus("CScrOptSelect", tr("<b>Ignore Waypoints</b><br/>Ignore waypoints in list of selected items<br/>"));
            break;

        case IGisItem::eSelectionRte:
            canvas->reportStatus("CScrOptSelect", tr("<b>Ignore Routes</b><br/>Ignore routes in list of selected items<br/>"));
            break;

        case IGisItem::eSelectionOvl:
            canvas->reportStatus("CScrOptSelect", tr("<b>Ignore Areas</b><br/>Ignore areas in list of selected items<br/>"));
            break;
        }
    }

    canvas->update();
}
