/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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

#ifndef CROUTERBROUTERTILESSELECTLAYOUT_H
#define CROUTERBROUTERTILESSELECTLAYOUT_H

#include <QLayout>
#include <QList>

class CRouterBRouterTilesSelectLayout : public QLayout
{
public:
    CRouterBRouterTilesSelectLayout(QWidget * parent) : QLayout(parent) {}

    virtual ~CRouterBRouterTilesSelectLayout() {}

    void addItem(QLayoutItem * item) override { items.append(item); }

    QSize sizeHint() const override { return QSize(200,200); }

    void setGeometry(const QRect & r) override { for (QLayoutItem *item : items) { item->setGeometry(r); }}

    QLayoutItem * itemAt(int index) const override { return items.at(index); }

    QLayoutItem * takeAt(int index) override { return items.takeAt(index); }

    int count() const override { return items.size(); }

private:
    QList<QLayoutItem *> items;
};

#endif //CROUTERBROUTERTILESSELECTLAYOUT_H

