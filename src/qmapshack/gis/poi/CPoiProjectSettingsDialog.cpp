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

#include "CPoiProjectSettingsDialog.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QTreeWidgetItem>

CPoiProjectSettingsDialog::CPoiProjectSettingsDialog(const QString &connectionName, QList<int> selectedCategories, QWidget *parent) : QDialog(parent), connectionName(connectionName)
{
    setupUi(this);

    //setup sql connection
    QSqlQuery query("SELECT id, name, parent FROM \"main\".\"poi_categories\" ORDER BY \"id\" DESC", QSqlDatabase::database(connectionName));

    //Setup tree view
    // Columns are "selected","id","name"
    treeWidgetCategories->setColumnCount(4);
    while (query.next())
    {
        QString parentID = query.value(eSqlColumnParent).toString();
        if(parentID == "")
        {
            continue; //This is the root item
        }
        auto parentList = treeWidgetCategories->findItems(parentID, Qt::MatchExactly, 1);
        if(parentList.count() > 1)
        {
            qDebug() << "something went wrong reading categories of poi file";
        }
        else
        {
            CPoiCategory* category = nullptr;
            if (parentList.count() == 1 && static_cast<CPoiCategory*>(parentList[0]) != nullptr)
            {
                category = new CPoiCategory(static_cast<CPoiCategory*>(parentList[0]));
            }
            else
            {
                category = new CPoiCategory(treeWidgetCategories);
            }
            category->setData(eTreeColumnName, Qt::DisplayRole, query.value(eSqlColumnName));
            category->setData(eTreeColumnID, Qt::DisplayRole, query.value(eSqlColumnID));
            if(selectedCategories.contains(query.value(eSqlColumnID).toInt()))
            {
                category->setCheckState(eTreeColumnCheckbox, Qt::Checked);
            }
            else
            {
                category->setCheckState(eTreeColumnCheckbox, Qt::Unchecked);
            }
        }
    }

    connect(treeWidgetCategories, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(collectNumOfItems(QTreeWidgetItem*)));
    connect(treeWidgetCategories, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(collectNumOfItems(QTreeWidgetItem*)));
    treeWidgetCategories->sortItems(eTreeColumnName, Qt::SortOrder::AscendingOrder);
    treeWidgetCategories->setHeaderLabels(QStringList()
                                          << tr("Selected")
                                          << tr("ID")
                                          << tr("Name")
                                          << tr("Number of POIs"));
    for(int i = 0; i < treeWidgetCategories->columnCount(); i++)
    {
        treeWidgetCategories->resizeColumnToContents(i);
    }
    treeWidgetCategories->hideColumn(eTreeColumnID);
}

CPoiProjectSettingsDialog::~CPoiProjectSettingsDialog()
{
}

QList<int> CPoiProjectSettingsDialog::getSelectedCategories()
{
    QList<int> selectedCategories;
    for(int i = 0; i < treeWidgetCategories->topLevelItemCount(); i++)
    {
        selectedCategories << getSelectedCategories(static_cast<CPoiCategory *>(treeWidgetCategories->topLevelItem(i)));
    }
    return selectedCategories;
}

void CPoiProjectSettingsDialog::collectNumOfItems(QTreeWidgetItem * category)
{
    for(int i = 0; i < category->childCount(); i++)
    {
        if(category->child(i)->data(eTreeColumnNumOfItems, Qt::DisplayRole).toString() == "")
        {
            setNumOfItems(static_cast<CPoiProjectSettingsDialog::CPoiCategory*>(category->child(i)));
        }
    }
}

void CPoiProjectSettingsDialog::setNumOfItems(CPoiProjectSettingsDialog::CPoiCategory*category)
{
    QSqlQuery numOfItemsQuery("SELECT COUNT(*) FROM (SELECT * FROM \"main\".\"poi_category_map\" WHERE category=" + category->data(eTreeColumnID, Qt::DisplayRole).toString() + ")",
                              QSqlDatabase::database(connectionName));
    auto queryOk = numOfItemsQuery.next();
    if(queryOk)
    {
        category->setNumOfOwnItems(numOfItemsQuery.value(0).toInt());
    }
    else
    {
        qDebug() << numOfItemsQuery.lastError();
    }
}

QList<int> CPoiProjectSettingsDialog::getSelectedCategories(CPoiProjectSettingsDialog::CPoiCategory *category)
{
    QList<int> selectedCategories;
    if(category->checkState(eTreeColumnCheckbox) == Qt::CheckState::Checked)
    {
        selectedCategories << category->data(eTreeColumnID, Qt::DisplayRole).toInt();
    }
    for(int i = 0; i < category->childCount(); i++)
    {
        selectedCategories << getSelectedCategories(static_cast<CPoiCategory *>(category->child(i)));
    }
    return selectedCategories;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//CPoiProjectSettingsDialog::CPoiCategroy
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CPoiProjectSettingsDialog::CPoiCategory::setup()
{
    setFlags(flags() | Qt::ItemIsTristate);
}

void CPoiProjectSettingsDialog::CPoiCategory::setNumOfOwnItems(int numOfItems)
{
    setData(eTreeColumnNumOfItems, Qt::DisplayRole, numOfItems);
    if(parentItem != nullptr)
    {
        parentItem->updateNumOfItems(numOfItems);
    }
}

void CPoiProjectSettingsDialog::CPoiCategory::updateNumOfItems(int delta)
{
    setData(eTreeColumnNumOfItems, Qt::DisplayRole, data(eTreeColumnNumOfItems, Qt::DisplayRole).toInt() + delta);
    if(parentItem != nullptr)
    {
        parentItem->updateNumOfItems(delta);
    }
}
