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

#include <gis/search/CSearchLineEdit.h>
#include <QTreeWidgetItem>

class CProjectFilterItem : public QTreeWidgetItem, public QObject
{
    Q_OBJECT
public:
    CProjectFilterItem();
    virtual ~CProjectFilterItem();
    void showLineEdit();

signals:
    void searchChanged(CSearch newSearch);
    void searchCleared();

private slots:
    void slotSearchChanged(CSearch newSearch)
    {
        emit searchChanged(newSearch);
    }
    void slotSearchCleared()
    {
        emit searchCleared();
    }

private:
    CSearchLineEdit* lineEdit;
};

#endif // CPROJECTFILTERITEM_H
