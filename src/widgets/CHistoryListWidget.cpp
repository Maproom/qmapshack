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

#include "CMainWindow.h"

#include <QtWidgets>

CHistoryListWidget::CHistoryListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setIconSize(QSize(32,32));
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &CHistoryListWidget::itemSelectionChanged,       this, &CHistoryListWidget::slotSelectionChanged);
    connect(this, &CHistoryListWidget::customContextMenuRequested, this, &CHistoryListWidget::slotContextMenu);

    menu = new QMenu(this);
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

    //for(const IGisItem::history_event_t& event : history.events)
    for(int i = 0; i < history.events.size(); i++)
    {
        const IGisItem::history_event_t& event = history.events[i];

        QString str;
        QListWidgetItem * item = new QListWidgetItem(this);

        str  = event.time.toString();
        if(!event.who.isEmpty())
        {
            str += tr(" by %1").arg(event.who);
        }

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
    if(nullptr == item)
    {
        return;
    }

    item->loadHistory(currentRow());
    item->updateDecoration(IGisItem::eMarkChanged, IGisItem::eMarkNone);

    emit sigChanged();
}

void CHistoryListWidget::slotContextMenu(const QPoint& point)
{
    if ((count() == 0) || (count() == 1)) // nothing can be done if there is 0 (should not happen) or 1 event in history
    {
        return;
    }

    menu->clear();

    if (currentRow() > 0)
    {
        actionCutHistoryBefore = menu->addAction(QIcon("://icons/32x32/CutHistoryBefore.png"), tr("Cut history before"), this, SLOT(slotCutHistoryBefore()));
    }

    if (currentRow() < count() - 1)
    {
        actionCutHistoryAfter = menu->addAction(QIcon("://icons/32x32/CutHistoryAfter.png"), tr("Cut history after"), this, SLOT(slotCutHistoryAfter()));
    }

    QPoint p = mapToGlobal(point);
    menu->exec(p);
}


void CHistoryListWidget::slotCutHistoryAfter()
{
    if(currentRow() == (count() - 1))
    {
        return;
    }

    IGisItem * item = CGisWidget::self().getItemByKey(key);
    if(nullptr == item)
    {
        return;
    }

    item->cutHistoryAfter();
    item->updateDecoration(IGisItem::eMarkChanged, IGisItem::eMarkNone);

    IGisProject * project = dynamic_cast<IGisProject*>(item->parent());
    if(project)
    {
        project->setChanged();
    }

    emit sigChanged();
}



void CHistoryListWidget::slotCutHistoryBefore()
{
    if (currentRow() == 0)
    {
        return;
    }

    IGisItem * item = CGisWidget::self().getItemByKey(key);
    if (nullptr == item)
    {
        return;
    }

    int res = QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("History removal")
        , tr("The removal is permanent and cannot be undone. "
          "<b>Do you really want to delete history before this step?</b>")
        , QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (res == QMessageBox::No)
    {
        return;
    }

    item->cutHistoryBefore();
    item->updateDecoration(IGisItem::eMarkChanged, IGisItem::eMarkNone);

    IGisProject * project = dynamic_cast<IGisProject*>(item->parent());
    if (project)
    {
        project->setChanged();
    }

    emit sigChanged();
}
