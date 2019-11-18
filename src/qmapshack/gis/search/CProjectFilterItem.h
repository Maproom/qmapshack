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
#ifndef CPROJECTFILTERITEM_H
#define CPROJECTFILTERITEM_H

#include "CSearchLineEdit.h"
#include <QPointer>
#include <QTreeWidgetItem>
class IGisProject;
class CSearch;

class CProjectFilterItem : public QTreeWidgetItem
{
public:
    CProjectFilterItem(IGisProject *parent);
    virtual ~CProjectFilterItem();
    void showLineEdit(CSearch* search=nullptr);
    const CSearchLineEdit* getLineEdit()
    {
        return lineEdit;
    }

private:
    QPointer<CSearchLineEdit> lineEdit;
    IGisProject* parent;
};

#endif // CPROJECTFILTERITEM_H
