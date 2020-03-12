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
#include <gis/CGisListWks.h>
#include <gis/search/CSearchLineEdit.h>

CProjectFilterItem::CProjectFilterItem(IGisProject *parent) : QTreeWidgetItem ((QTreeWidgetItem*)parent)
{
    this->parent = parent;
}

CProjectFilterItem::~CProjectFilterItem()
{
}

void CProjectFilterItem::showLineEdit(CSearch *search)
{
    if(treeWidget() != nullptr)
    {
        //new CSearchLineEdit, since destructor is called when replacing ItemWidget
        //using QPointer and checking for isNull() is not enough, since it happens after this point
        if(!lineEdit.isNull())
        {
            lineEdit->deleteLater();
        }
        lineEdit = new CSearchLineEdit(treeWidget(), parent, search);

        treeWidget()->setItemWidget(this, CGisListWks::eColumnName, lineEdit);
    }
}
