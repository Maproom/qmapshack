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

#include "gis/trk/CSelectActivityColor.h"
#include <QtWidgets>

CSelectActivityColor::CSelectActivityColor(QWidget *parent)
    : QWidget(parent)
    , INotifyTrk(CGisItemTrk::eVisualColorAct)
{
    setupUi(this);
}

CSelectActivityColor::~CSelectActivityColor()
{
    if(trk != nullptr)
    {
        trk->unregisterVisual(this);
    }
}

void CSelectActivityColor::setTrack(CGisItemTrk * p)
{
    if(trk != nullptr)
    {
        trk->unregisterVisual(this);
    }

    trk = p;

    if(trk != nullptr)
    {
        trk->registerVisual(this);
    }

    updateData();
}

void CSelectActivityColor::updateData()
{
    qDeleteAll(allActLabels);
    qDeleteAll(allActColors);
    allActLabels.clear();
    allActColors.clear();

    if((trk == nullptr) || !isEnabled())
    {
        return;
    }

    const QSet<trkact_t>& acts = trk->getActivities().getAllActivities();
    for(quint32 i = 0; i < CTrackData::trkpt_t::eAct20MaxNum; i++)
    {
        if(!acts.contains(trkact_t(i)))
        {
            continue;
        }
        const CActivityTrk::desc_t& desc = CActivityTrk::getDescriptor(trkact_t(i));

        QLabel * label = new QLabel(this);
        label->setMinimumSize(16, 16);
        label->setMaximumSize(16, 16);
        label->setPixmap(desc.iconSmall);
        label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
        horizontalLayout->addWidget(label);

        allActLabels << label;

        QPixmap pixmap(16, 16);
        pixmap.fill(desc.color);

        QToolButton * button = new QToolButton(this);
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        button->setAutoRaise(true);
        button->setIcon(QIcon(pixmap));
        button->setProperty("color", desc.color.name());
        button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
        horizontalLayout->addWidget(button);

        connect(button, &QToolButton::clicked, this, [this, button, i](){slotSetColor(button, trkact_t(i));});

        allActColors << button;
    }

    QSpacerItem * spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    horizontalLayout->addItem(spacer);
}

void CSelectActivityColor::slotSetColor(QToolButton * button, trkact_t act)
{
    qint32 colorIdx = IGisItem::selectColor(this);
    if(colorIdx == NOIDX)
    {
        return;
    }

    const IGisItem::color_t& color = IGisItem::getColorMap()[colorIdx];
    QPixmap pixmap(16, 16);
    pixmap.fill(color.color);
    button->setIcon(QIcon(pixmap));

    CActivityTrk::setColor(act, color);

    if(trk != nullptr)
    {
        trk->updateVisuals(CGisItemTrk::eVisualDetails | CGisItemTrk::eVisualPlot | CGisItemTrk::eVisualProject | CGisItemTrk::eVisualTrkInfo, "CSelectActivityColor::slotSetColor()");
    }
}
