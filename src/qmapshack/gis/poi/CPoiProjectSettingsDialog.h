/**********************************************************************************************
    Copyright (C) 2021 Henri Hornburg <pingurus@t-online.de>

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

#ifndef CPOIPROJECTSETTINGSDIALOG_H
#define CPOIPROJECTSETTINGSDIALOG_H

#include "ui_IPoiProjectSettingsDialog.h"
#include <QDialog>

class CPoiProjectSettingsDialog : public QDialog, Ui_CPoiProjectSettingsDialog
{
    Q_OBJECT
    class CPoiCategory;

public:
    explicit CPoiProjectSettingsDialog( const QString& connectionName, QList<int> selectedCategories, QWidget *parent = nullptr);
    ~CPoiProjectSettingsDialog();
    QList<int> getSelectedCategories();

private slots:
    void collectNumOfItems(QTreeWidgetItem *category);

private:
    void setNumOfItems(CPoiCategory * category);
    QList<int> getSelectedCategories(CPoiCategory * category);

    QString connectionName;
    class CPoiCategory : public QTreeWidgetItem
    {
public:
        CPoiCategory(CPoiCategory *parentItem) : QTreeWidgetItem(parentItem), parentItem(parentItem) {setup();}
        CPoiCategory(QTreeWidget *parentTree) : QTreeWidgetItem(parentTree) {setup();}
        void setNumOfOwnItems(int numOfItems);
private:
        void setup();
        void updateNumOfItems(int delta);
private:
        CPoiCategory* parentItem = nullptr;
    };

    enum treeColumn_e
    {
        eTreeColumnCheckbox,
        eTreeColumnID,
        eTreeColumnName,
        eTreeColumnNumOfItems
    };

    enum sqlColumn_e
    {
        eSqlColumnID,
        eSqlColumnName,
        eSqlColumnParent
    };
};

#endif // CPOIPROJECTSETTINGSDIALOG_H
