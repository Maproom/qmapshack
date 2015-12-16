/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/CGisWidget.h"
#include "gis/prj/IGisProject.h"
#include "widgets/CHistoryListWidget.h"

#include <QtWidgets>

CHistoryListWidget::CHistoryListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setIconSize(QSize(32,32));
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &CHistoryListWidget::itemSelectionChanged,       this, &CHistoryListWidget::slotSelectionChanged);
    connect(this, &CHistoryListWidget::customContextMenuRequested, this, &CHistoryListWidget::slotContextMenu);

    menu = new QMenu(this);
    actionCutHistory = menu->addAction(QIcon("://icons/32x32/CutHistory.png"),tr("Cut history"), this, SLOT(slotCutHistory()));
}

CHistoryListWidget::~CHistoryListWidget()
{
}

void CHistoryListWidget::setupHistory(IGisItem& gisItem)
{
    blockSignals(true);
    clear();

    key = gisItem.getKey();

    const IGisItem::history_t& history = gisItem.getHistory();

    //foreach(const IGisItem::history_event_t& event, history.events)
    for(int i = 0; i < history.events.size(); i++)
    {
        const IGisItem::history_event_t& event = history.events[i];

        QString str;
        QListWidgetItem * item = new QListWidgetItem(this);

        str  = event.time.toString();
        str += "\n";
        str += event.comment;

        item->setText(str);
        item->setIcon(QIcon(event.icon));
        if(event.data.isEmpty())
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        }
    }

    if(history.histIdxCurrent < count())
    {
        setCurrentItem(item(history.histIdxCurrent));
    }
    blockSignals(false);
}

void CHistoryListWidget::slotSelectionChanged()
{
    IGisItem * item = CGisWidget::self().getItemByKey(key);
    if(item == 0)
    {
        return;
    }

    item->loadHistory(currentRow());
    item->updateDecoration(IGisItem::eMarkChanged, IGisItem::eMarkNone);

    emit sigChanged();
}

void CHistoryListWidget::slotContextMenu(const QPoint& point)
{
    if(currentRow() == (count() - 1) || (count() == 0))
    {
        return;
    }

    QPoint p = mapToGlobal(point);
    menu->exec(p);
}


void CHistoryListWidget::slotCutHistory()
{
    if(currentRow() == (count() - 1))
    {
        return;
    }

    IGisItem * item = CGisWidget::self().getItemByKey(key);
    if(item == 0)
    {
        return;
    }

    item->cutHistory();
    item->updateDecoration(IGisItem::eMarkChanged, IGisItem::eMarkNone);

    IGisProject * project = dynamic_cast<IGisProject*>(item->parent());
    if(project)
    {
        project->setChanged();
    }

    emit sigChanged();
}
