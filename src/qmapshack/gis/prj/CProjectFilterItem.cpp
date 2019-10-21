/**********************************************************************************************
    Copyright (C) 2019 Henri Hornburg hrnbg@t-online.de

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
#include "CProjectFilterItem.h"

CProjectFilterItem::CProjectFilterItem()
{
    lineEdit = new CSearchLineEdit(treeWidget());
    QObject::connect(lineEdit, &CSearchLineEdit::searchChanged, this, &CProjectFilterItem::slotSearchChanged);
    QObject::connect(lineEdit, &CSearchLineEdit::searchCleared, this, &CProjectFilterItem::slotSearchCleared);
}

CProjectFilterItem::~CProjectFilterItem()
{
    delete lineEdit;
}

void CProjectFilterItem::showLineEdit()
{
    treeWidget()->setItemWidget(this, 1, lineEdit);
}
