/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>
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

#include "helpers/CDraw.h"
#include "helpers/CTryMutexLocker.h"
#include "poi/CPoiCategory.h"
#include "poi/CPoiDraw.h"
#include "poi/CPoiIconCategory.h"
#include "poi/CPoiItem.h"
#include "poi/CPoiPOI.h"
#include "poi/IPoiFile.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtWidgets>

CPoiPOI::CPoiPOI(const QString& filename, CPoiDraw* parent)
    : IPoiFile(parent)
    , filename(filename)
{
    // Set true if the file could be open and loaded successfully
    // If not set true the system will take care to destroy this object
    isActivated = true;

    // Open database here so it belongs to the right thread
    if(!QSqlDatabase::contains(filename + "_bbox"))
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", filename + "_bbox");
        db.setDatabaseName(filename);
        if(!db.open())
        {
            qDebug() << "failed to open database" << db.lastError();
            isActivated = false;
            return;
        }
    }

    QSqlQuery query("SELECT value FROM main.metadata WHERE name='bounds'", QSqlDatabase::database(filename + "_bbox"));

    if(query.next())
    {
        const QStringList& corners = query.value(0).toString().split(",");
        qreal top = corners[2].toDouble();
        qreal left = corners[1].toDouble();
        qreal width = corners[3].toDouble() - left;
        qreal height = corners[0].toDouble() - top;
        bbox = QRectF(left, top, width, height);
    }
    else
    {
        qDebug() << "failed to retrieve bounding box of " << filename;
        isActivated = false;
    }
    //Database is no longer needed
    QSqlDatabase::removeDatabase(filename + "_bbox");
}

void CPoiPOI::addTreeWidgetItems(QTreeWidget* widget)
{
    QMutexLocker lock(&mutex);

    // Open database here so it belongs to the right thread
    if(!QSqlDatabase::contains(filename + "_widget"))
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", filename + "_widget");
        db.setDatabaseName(filename);
        if(!db.open())
        {
            qDebug() << "failed to open database" << db.lastError();
            isActivated = false;
            return;
        }
    }

    QSqlQuery query("SELECT id, name, parent FROM main.poi_categories ORDER BY id DESC", QSqlDatabase::database(filename + "_widget"));

    QMap<uint, CPoiCategory*> categoryMap;

    while (query.next())
    {
        bool hasParent = false;
        quint64 parentID = query.value(eSqlColumnCategoryParent).toUInt(&hasParent);
        if(!hasParent)
        {
            continue; //This is the root item
        }
        const QString& categoryName = query.value(eSqlColumnCategoryName).toString();
        quint64 categoryID = query.value(eSqlColumnCategoryId).toUInt();
        categoryNames[categoryID] = categoryName;
        // This assumes, that the parent categories always have a higher ID than their children. This seems to be the case for all Mapsforge poi files
        if (categoryMap.contains(parentID))
        {
            categoryMap[categoryID] = new CPoiCategory(categoryName, categoryID, categoryActivated[categoryID], categoryMap[parentID]);
        }
        else
        {
            categoryMap[categoryID] = new CPoiCategory(categoryName, categoryID, widget);
        }
    }
}

void CPoiPOI::getPoiIcon(QPixmap& icon, const CPoiPOI::poiGroup_t& poiGroup)
{
    if(poiGroup.pois.count() > 1)
    {
        icon = QPixmap("://icons/poi/SJJB/png/poi_point_of_interest.n.32.png");
    }
    else
    {
        getPoiIcon(icon, loadedPois[*poiGroup.pois.begin()]);
    }
}

void CPoiPOI::getPoiIcon(QPixmap& icon, const CPoiItem* poi)
{
    const CPoiItemPOI* poiPOI = (const CPoiItemPOI*)poi;
    for(const QString& tag : poiPOI->getRawData())
    {
        if(tagMap.contains(tag))
        {
            icon = tagMap[tag].getIcon(poiPOI->getRawData());
            return;
        }
    }
    icon = QPixmap("://icons/poi/SJJB/png/poi_point_of_interest.n.32.png");
}



void CPoiPOI::loadPOIsFromFile(quint64 categoryID, int minLonM10, int minLatM10)
{
    QMutexLocker lock(&mutex);

    // check if query is within bounds
    if(!bbox.intersects({minLonM10 / 10.0, minLatM10 / 10.0, 0.1, 0.1}))
    {
        return;
    }

    //Open Database here so it is owned by the right thread
    if(!QSqlDatabase::contains(filename))
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", filename);
        db.setDatabaseName(filename);
        if(!db.open())
        {
            qDebug() << "failed to open database" << db.lastError();
            isActivated = false;
            return;
        }
    }

    QSqlQuery query(QSqlDatabase::database(filename));
    query.prepare("SELECT main.poi_index.maxLat, main.poi_index.maxLon, main.poi_index.minLat, main.poi_index.minLon, main.poi_data.data, main.poi_data.id "
                  "FROM main.poi_data, main.poi_index "
                  "WHERE main.poi_data.id IN "
                  "("
                  "    SELECT main.poi_category_map.id "
                  "    FROM main.poi_category_map "
                  "    WHERE main.poi_category_map.id IN "
                  "    ( "
                  "        SELECT main.poi_index.id "
                  "        FROM main.poi_index "
                  "        WHERE main.poi_index.maxLat<:maxLat "
                  "        AND main.poi_index.minLat>=:minLat "
                  "        AND main.poi_index.maxLon<:maxLon "
                  "        AND main.poi_index.minLon>=:minLon "
                  "    ) "
                  "    AND main.poi_category_map.category=:categoryID "
                  ") "
                  "AND main.poi_data.id = main.poi_index.id");
    query.bindValue(":maxLat", QString::number((minLatM10 + 1) / 10., 'f'));
    query.bindValue(":minLat", QString::number(minLatM10 / 10., 'f'));
    query.bindValue(":maxLon", QString::number((minLonM10 + 1) / 10., 'f'));
    query.bindValue(":minLon", QString::number(minLonM10 / 10., 'f'));
    query.bindValue(":categoryID", categoryID);
    query.exec();
    while (query.next())
    {
        quint64 key = query.value(eSqlColumnPoiId).toUInt();
        const QStringList& data = query.value(eSqlColumnPoiData).toString().split("\r");
        QString garminIcon;
        for(const QString& tag : data)
        {
            if(tagMap.contains(tag))
            {
                garminIcon = tagMap[tag].getGarminSym();
                break;
            }
        }
        loadedPoisByArea[categoryID][minLonM10][minLatM10].append(key);
        // TODO: this overwrites a POI if it already was loaded. The difference between those will be the category. Some better handling should be done
        loadedPois[key] = new CPoiItemPOI(data,
                                          QPointF((query.value(eSqlColumnPoiMaxLon).toDouble() + query.value(eSqlColumnPoiMinLon).toDouble()) / 2 * DEG_TO_RAD,
                                                  (query.value(eSqlColumnPoiMaxLat).toDouble() + query.value(eSqlColumnPoiMinLat).toDouble()) / 2 * DEG_TO_RAD),
                                          key, categoryNames[categoryID], garminIcon);
    }
    // Close database, as this method is called from mutiple threads.
    QSqlDatabase::removeDatabase(filename);
}
