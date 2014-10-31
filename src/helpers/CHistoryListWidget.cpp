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

#include "helpers/CHistoryListWidget.h"
#include "gis/CGisWidget.h"

#include <QtWidgets>

CHistoryListWidget::CHistoryListWidget(QWidget *parent)
    : QListWidget(parent)
{
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
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

    item->loadHistoryEntry(currentRow());
    item->setText(1,"*");
    item->parent()->setText(1,"*");

    emit sigChanged();
}
