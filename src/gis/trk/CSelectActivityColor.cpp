/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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
#include "widgets/CColorChooser.h"

#include <QtWidgets>
#include <functional>

using std::bind;

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

    const CActivityTrk& act = trk->getActivities();
    quint32 flags           = act.getAllFlags();
    quint32 mask            = 0x80000000;
    qint32 cnt              = 0;
    for(quint32 i = 0; i < CGisItemTrk::trkpt_t::eActMaxNum; i++)
    {
        if((flags & mask) != 0)
        {
            const CActivityTrk::desc_t& desc = CActivityTrk::getDescriptor(mask);

            QLabel * label = new QLabel(this);
            label->setMinimumSize(16,16);
            label->setMaximumSize(16,16);
            label->setPixmap(desc.iconSmall);
            label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
            horizontalLayout->addWidget(label);

            allActLabels << label;

            QPixmap pixmap(16,16);
            pixmap.fill(desc.color);

            QToolButton * button = new QToolButton(this);
            button->setToolButtonStyle(Qt::ToolButtonIconOnly);
            button->setAutoRaise(true);
            button->setIcon(QIcon(pixmap));
            button->setProperty("color", desc.color.name());
            button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
            horizontalLayout->addWidget(button);

            auto setColorFunc = bind(&CSelectActivityColor::slotSetColor, this, button, mask);
            connect(button, &QToolButton::clicked, this, setColorFunc);

            allActColors << button;

            ++cnt;
        }

        mask >>= 1;
    }

    QSpacerItem * spacer = new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Minimum);
    horizontalLayout->addItem(spacer);
}

void CSelectActivityColor::slotSetColor(QToolButton * button, quint32 activityFlag)
{
    CColorChooser dlg(button);
    dlg.move(button->parentWidget()->mapToGlobal(button->geometry().topRight()));
    if(dlg.exec() == QDialog::Accepted)
    {
        CActivityTrk::setColor(activityFlag, button->property("color").toString());
    }
}
