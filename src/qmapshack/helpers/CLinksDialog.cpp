/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CLinksDialog.h"

#include <QtWidgets>

CLinksDialog::CLinksDialog(QList<IGisItem::link_t> &links, QWidget *parent)
    : QDialog(parent)
    , links(links)
{
    setupUi(this);

    connect(toolAdd,    &QToolButton::clicked,              this, &CLinksDialog::slotAddLink);
    connect(toolDelete, &QToolButton::clicked,              this, &CLinksDialog::slotDelLink);
    connect(treeWidget, &QTreeWidget::itemSelectionChanged, this, &CLinksDialog::slotItemSelectionChanged);

    for(const IGisItem::link_t& link : links)
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(treeWidget);
        item->setText(0, link.type);
        item->setText(1, link.text);
        item->setText(2, link.uri.toString());
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }
}

CLinksDialog::~CLinksDialog()
{
}

void CLinksDialog::slotItemSelectionChanged()
{
    QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
    toolDelete->setEnabled(!items.isEmpty());
}

void CLinksDialog::slotAddLink()
{
    QTreeWidgetItem * item = new QTreeWidgetItem(treeWidget);
    item->setText(0, "");
    item->setText(1, "enter a text");
    item->setText(2, "enter a link");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
}

void CLinksDialog::slotDelLink()
{
    QList<QTreeWidgetItem *> items = treeWidget->selectedItems();
    qDeleteAll(items);
}

void CLinksDialog::accept()
{
    links.clear();

    for(int i = 0; i < treeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * item = treeWidget->topLevelItem(i);

        IGisItem::link_t link;
        link.type = item->text(0);
        link.text = item->text(1);
        link.uri  = item->text(2);

        links << link;
    }

    QDialog::accept();
}
