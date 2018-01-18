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

#include "realtime/CRtDraw.h"
#include "realtime/CRtWorkspace.h"
#include "realtime/opensky/CRtOpenSky.h"
#include "helpers/CSettings.h"

#include <QtGui>

CRtWorkspace * CRtWorkspace::pSelf = nullptr;

CRtWorkspace::CRtWorkspace(QWidget *parent)
    : QWidget(parent)
{
    pSelf = this;
    setupUi(this);

    connect(treeWidget, &QTreeWidget::itemChanged, this, &CRtWorkspace::slotItemChanged);

    SETTINGS;
    cfg.beginGroup("Realtime");
    treeWidget->header()->restoreState(cfg.value("treeWks/state", treeWidget->header()->saveState()).toByteArray());
    cfg.endGroup();

    frame->setVisible(treeWidget->topLevelItemCount() == 0);
    labelHelp->setText(tr("To add a real time source do a right click on the list above. "));


//    new CRtOpenSky(treeWidget);
}

CRtWorkspace::~CRtWorkspace()
{
    SETTINGS;
    cfg.beginGroup("Realtime");
    cfg.remove("");
    cfg.setValue("treeWks/state", treeWidget->header()->saveState());
    cfg.endGroup();
}

void CRtWorkspace::draw(QPainter& p, const QPolygonF &viewport, CRtDraw *rt)
{
    QMutexLocker lock(&IRtSource::mutex);
    QList<QRectF> blockedAreas;

    const int N = treeWidget->topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        IRtSource * item = dynamic_cast<IRtSource*>(treeWidget->topLevelItem(n));
        if(item == nullptr)
        {
            continue;
        }

        item->drawItem(p, viewport, blockedAreas, rt);
    }
}

void CRtWorkspace::fastDraw(QPainter& p, const QRectF& viewport, CRtDraw *rt)
{
}

void CRtWorkspace::slotItemChanged(QTreeWidgetItem * item, int column)
{
    QMutexLocker lock(&IRtSource::mutex);

    if(column == IRtSource::eColumnCheckBox)
    {
        emit sigChanged();
    }
}
