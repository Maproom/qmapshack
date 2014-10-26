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

#include <QtWidgets>

CHistoryListWidget::CHistoryListWidget(QWidget *parent)
    : QListWidget(parent)
{

}

CHistoryListWidget::~CHistoryListWidget()
{

}

void CHistoryListWidget::setupHistory(const QList<IGisItem::history_t>& history)
{
    clear();

    foreach(const IGisItem::history_t& entry, history)
    {
        QString str;
        QListWidgetItem * item = new QListWidgetItem(this);

        str  = entry.time.toString();
        str += "\n";
        str += entry.comment;

        item->setText(str);
        item->setIcon(QIcon(entry.icon));
        if(entry.data.isEmpty())
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        }
    }
}
