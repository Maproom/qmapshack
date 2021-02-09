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

#include "gis/Poi.h"
#include "helpers/CDraw.h"
#include "poi/CPoiCategory.h"
#include "poi/CPoiDraw.h"
#include "poi/CPoiIconCategory.h"
#include "poi/CPoiPOI.h"
#include "poi/IPoi.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtWidgets>

CPoiPOI::CPoiPOI(const QString &filename, CPoiDraw *parent)
    : IPoi(parent)
    , filename(filename)
    , loadTimer(new QTimer(this))
{
    // Set true if the file could be open and loaded successfully
    // If not set true the system will take care to destroy this object
    isActivated = true;

    //Set up a timer to only start reloading and redrawing the map when the user has made his selections
    loadTimer->setSingleShot(true);
    loadTimer->setInterval(500);
    connect(loadTimer, &QTimer::timeout, poi, &CPoiDraw::emitSigCanvasUpdate);
}


void CPoiPOI::draw(IDrawContext::buffer_t& buf)
{
    // !!!! NOTE !!!!
    // This is running in it's own thread, not the main thread.
    // Use CPoiPOI::mutex whenever you access data shared between
    // the main thread and this thread. Do not block it more than
    // neccessary.


    // do that on a regular base while you draw the buffer
    // to abort drawing ass soon as possible if there is
    // another draw request pending
    if(poi->needsRedraw())
    {
        return;
    }

    QPainter p(&buf.image);
    USE_ANTI_ALIASING(p, true);
    p.setOpacity(getOpacity() / 100.0);

    QPointF pp = buf.ref1;
    poi->convertRad2Px(pp);
    p.translate(-pp);
    p.setBrush(Qt::white);
    p.setPen(Qt::NoPen);

    // calculate maximum viewport
    qreal xMin = qMin(buf.ref1.x(), buf.ref4.x());
    qreal xMax = qMax(buf.ref2.x(), buf.ref3.x());
    qreal yMin = qMin(buf.ref3.y(), buf.ref4.y());
    qreal yMax = qMax(buf.ref1.y(), buf.ref2.y());

    if(xMin < -180.0 * DEG_TO_RAD)
    {
        xMin = -180 * DEG_TO_RAD;
    }
    if(xMax >  180.0 * DEG_TO_RAD)
    {
        xMax =  180 * DEG_TO_RAD;
    }

    // draw POI
    mutex.lock(); //Does this have to be so broad?
    for(quint64 categoryID : categoryActivated.keys(Qt::Checked))
    {
        for(int minLonM10 = qFloor(xMin * RAD_TO_DEG * 10); minLonM10 < qCeil(xMax * RAD_TO_DEG * 10); minLonM10++)
        {
            for(int minLatM10 = qFloor(yMin * RAD_TO_DEG * 10); minLatM10 < qCeil(yMax * RAD_TO_DEG * 10); minLatM10++)
            {
                if(!loadedPOIs.contains(categoryID) ||
                   !loadedPOIs[categoryID].contains(minLonM10) ||
                   !loadedPOIs[categoryID][minLonM10].contains(minLatM10))
                {
                    mutex.unlock();
                    loadPOIsFromFile(categoryID, minLonM10, minLatM10);
                    mutex.lock();
                }
                if(poi->needsRedraw())
                {
                    mutex.unlock();
                    return;
                }
                for(const rawPoi_t& poiToDraw : loadedPOIs[categoryID][minLonM10][minLatM10])
                {
                    QPixmap icon;
                    getPoiIcon(icon, poiToDraw);
                    icon = icon.scaled(IPoi::iconSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    QPointF pt = poiToDraw.coordinates;
                    poi->convertRad2Px(pt);

                    QRectF r1 = icon.rect();
                    r1.moveCenter(pt);

                    p.drawRect(r1.adjusted(-1, -1, 1, 1));
                    p.drawPixmap(r1.topLeft(), icon);
                }
            }
        }
    }
    mutex.unlock();
}

bool CPoiPOI::findPoiCloseBy(const QPoint &px, poi_t & poiItem) const
{
    QPointF p = px;
    poi->convertPx2Rad(p);
    for(quint64 categoryID : categoryActivated.keys(Qt::Checked))
    {
        int minLonM10 = qFloor(p.x() * RAD_TO_DEG * 10);
        int minLatM10 = qFloor(p.y() * RAD_TO_DEG * 10);
        //The tile that contains the POI has to be loaded, since a position on screen is given
        for(const rawPoi_t& poiItemFound : loadedPOIs[categoryID][minLonM10][minLatM10])
        {
            QPointF pt = poiItemFound.coordinates;
            poi->convertRad2Px(pt);
            QPointF d = px - pt;
            if(qMax(qAbs(d.x()), qAbs(d.y())) < iconSize().height() / 2)
            {
                poiItem = poiItemFound.toPoi(categoryNames[categoryID]);
                return true;
            }
        }
    }
    return false;
}

void CPoiPOI::findPoisIn(const QRectF &degRect, QList<poi_t> &pois)
{
    QSet<quint64> copiedItems; //SOme Items may appear in multiple categories. We only want to copy those once.
    for(quint64 categoryID : categoryActivated.keys(Qt::Checked))
    {
        for(int minLonM10 = qFloor(degRect.left() * 10); minLonM10 <= qFloor(degRect.right() * 10); minLonM10++)
        {
            for(int minLatM10 = qFloor(degRect.bottom() * 10); minLatM10 <= qFloor(degRect.top() * 10); minLatM10++)
            {
                //Imagine the user moves the screen in an l-shape while updating the selection rectangle. It is possible that some tiles are not laded then
                if(!loadedPOIs.contains(categoryID) ||
                   !loadedPOIs[categoryID].contains(minLonM10) ||
                   !loadedPOIs[categoryID][minLonM10].contains(minLatM10))
                {
                    loadPOIsFromFile(categoryID, minLonM10, minLatM10);
                }
                for(const rawPoi_t& poiItemFound : loadedPOIs[categoryID][minLonM10][minLatM10])
                {
                    if(!copiedItems.contains(poiItemFound.key))
                    {
                        //Maybe look through the whole code of selecting items from a map to avoid this conversion
                        if(degRect.contains(poiItemFound.coordinates * RAD_TO_DEG))
                        {
                            pois.append(poiItemFound.toPoi(categoryNames[categoryID]));
                            copiedItems.insert(poiItemFound.key);
                        }
                    }
                }
            }
        }
    }
}

bool CPoiPOI::getToolTip(const QPoint &px, QString &str) const
{
    poi_t poiFound;
    bool success = findPoiCloseBy(px, poiFound);
    if(success)
    {
        str += "<b>" + poiFound.name + "</b><br>";
        str += poiFound.desc;
    }
    return success;
}

void CPoiPOI::addTreeWidgetItems(QTreeWidget* widget)
{
    // Open database here so it belongs to the right thread
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

    QSqlQuery query("SELECT id, name, parent FROM main.poi_categories ORDER BY id DESC", QSqlDatabase::database(filename));

    QMap<uint, CPoiCategory*> categoryMap;

    while (query.next())
    {
        bool hasParent = false;
        quint64 parentID = query.value(eSqlColumnCategoryParent).toUInt(&hasParent);
        if(!hasParent)
        {
            continue; //This is the root item
        }
        QString categoryName = query.value(eSqlColumnCategoryName).toString();
        quint64 categoryID = query.value(eSqlColumnCategoryId).toUInt();
        categoryNames[categoryID] = categoryName;
        // This assumes, that the parent categories always have a higher ID than their children. This seems to be the case for all Mapsforge poi files
        if (categoryMap.keys().contains(parentID))
        {
            categoryMap[categoryID] = new CPoiCategory(categoryName, categoryID, categoryActivated[categoryID], categoryMap[parentID]);
        }
        else
        {
            categoryMap[categoryID] = new CPoiCategory(categoryName, categoryID, widget);
        }
    }

    //Close it, so it can be reopened from the right thread
    QSqlDatabase::removeDatabase(filename);
}

void CPoiPOI::slotCheckedStateChanged(QTreeWidgetItem * item)
{
    CPoiCategory* categoryItem = static_cast<CPoiCategory*>(item);
    if(categoryItem == nullptr)
    {
        return;
    }

    categoryActivated[categoryItem->getId()] = categoryItem->checkState();

    loadTimer->start();
}

void CPoiPOI::getPoiIcon(QPixmap& icon, const CPoiPOI::rawPoi_t &poi, const QString &definingTag)
{
    if(!definingTag.isEmpty() && tagMap.contains(definingTag))
    {
        icon = tagMap[definingTag].getIcon(poi.data);
        return;
    }
    for(const QString& tag : poi.data)
    {
        if(tagMap.contains(tag))
        {
            icon = tagMap[tag].getIcon(poi.data);
            return;
        }
    }
    icon = QPixmap("://icons/poi/SJJB/png/poi_point_of_interest.n.32.png");
}

void CPoiPOI::loadPOIsFromFile(quint64 categoryID, int minLonM10, int minLatM10)
{
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
    query.bindValue(":maxLat",  QString::number((minLatM10 + 1) / 10., 'f'));
    query.bindValue(":minLat",  QString::number(minLatM10 / 10., 'f'));
    query.bindValue(":maxLon",  QString::number((minLonM10 + 1) / 10., 'f'));
    query.bindValue(":minLon",  QString::number(minLonM10 / 10., 'f'));
    query.bindValue(":categoryID", categoryID);
    query.exec();
    while (query.next())
    {
        rawPoi_t poi;
        poi.coordinates = QPointF((query.value(eSqlColumnPOIMaxLon).toDouble() + query.value(eSqlColumnPOIMinLon).toDouble()) / 2 * DEG_TO_RAD,
                                  (query.value(eSqlColumnPOIMaxLat).toDouble() + query.value(eSqlColumnPOIMinLat).toDouble()) / 2 * DEG_TO_RAD);
        poi.data = query.value(eSqlColumnPOIData).toString().split("\r");
        poi.key = query.value(eSqlColumnPOIID).toUInt();
        mutex.lock();
        loadedPOIs[categoryID][minLonM10][minLatM10].append(poi);
        mutex.unlock();
    }
    //Close it, so it can be reopened from the right thread
    QSqlDatabase::removeDatabase(filename);
}

poi_t CPoiPOI::rawPoi_t::toPoi(const QString& categoryName) const
{
    poi_t poi;
    poi.pos = coordinates;
    for(const QRegularExpression& regex : {QRegularExpression("name:" + QLocale::system().name() + "=(.+)", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("name:en=(.+)", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("name=(.+)", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("name:\\w\\w=(.+)", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("brand=(.+)", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("operator=(.+)", QRegularExpression::UseUnicodePropertiesOption)})
    {
        const QStringList& matches = data.filter(regex);
        if (!matches.isEmpty())
        {
            poi.name = regex.match(matches[0]).captured(1).trimmed();
            break;
        }
    }
    if(poi.name.isEmpty())
    {
        poi.name = categoryName;
    }
    poi.desc = data.join("<br>\n");
    poi.desc.prepend("From category <b>" + categoryName + "</b><br>\n");
    return poi;
}
