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
#include "poi/CPoiPOI.h"

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
    for(const QString& category : categoryActivated.keys(Qt::Checked))
    {
        qreal s = 22. / 32; //For now reduces the poi icons to the same size as wpt icons. Later something more intelligent may be calculated
        QPixmap icon("://icons/poi/png/" + category + ".n.32.png");
        icon = icon.scaled(icon.size() * s, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        for(int minLonM10 = qFloor(xMin * RAD_TO_DEG * 10); minLonM10 < qCeil(xMax * RAD_TO_DEG * 10); minLonM10++)
        {
            for(int minLatM10 = qFloor(yMin * RAD_TO_DEG * 10); minLatM10 < qCeil(yMax * RAD_TO_DEG * 10); minLatM10++)
            {
                if(!loadedPOIs.contains(category) ||
                   !loadedPOIs[category].contains(minLonM10) ||
                   !loadedPOIs[category][minLonM10].contains(minLatM10))
                {
                    mutex.unlock();
                    loadPOIsFromFile(category, minLonM10, minLatM10);
                    mutex.lock();
                }
                if(poi->needsRedraw())
                {
                    mutex.unlock();
                    return;
                }
                for(const rawPoi_t& poiToDraw : loadedPOIs[category][minLonM10][minLatM10])
                {
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
    for(const QString& category : categoryActivated.keys(Qt::Checked))
    {
        int minLonM10 = qFloor(p.x() * RAD_TO_DEG * 10);
        int minLatM10 = qFloor(p.y() * RAD_TO_DEG * 10);
        //The tile that contains the POI has to be loaded, since a position on screen is given
        for(const rawPoi_t& poiItemFound : loadedPOIs[category][minLonM10][minLatM10])
        {
            QPointF pt = poiItemFound.coordinates;
            poi->convertRad2Px(pt);
            QPointF x = px - pt;
            if(x.manhattanLength() < 10)
            {
                poiItem = poiItemFound.toPoi(category);
                return true;
            }
        }
    }
    return false;
}

void CPoiPOI::findPoisIn(const QRectF &degRect, QList<poi_t> &pois)
{
    for(const QString& category : categoryActivated.keys(Qt::Checked))
    {
        for(int minLonM10 = qFloor(degRect.left() * 10); minLonM10 <= qFloor(degRect.right() * 10); minLonM10++)
        {
            for(int minLatM10 = qFloor(degRect.bottom() * 10); minLatM10 <= qFloor(degRect.top() * 10); minLatM10++)
            {
                //Imagine the user moves the screen in an l-shape while updating the selection rectangle. It is possible that some tiles are not laded then
                if(!loadedPOIs.contains(category) ||
                   !loadedPOIs[category].contains(minLonM10) ||
                   !loadedPOIs[category][minLonM10].contains(minLatM10))
                {
                    loadPOIsFromFile(category, minLonM10, minLatM10);
                }
                for(const rawPoi_t& poiItemFound : loadedPOIs[category][minLonM10][minLatM10])
                {
                    //Maybe look through the whole code of selecting items from a map to avoid this conversion
                    if(degRect.contains(poiItemFound.coordinates * RAD_TO_DEG))
                    {
                        pois.append(poiItemFound.toPoi(category));
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
    QMap<QString, CPoiCategory*> parentMap;

    for(const QString& category : tagMap.keys())
    {
        QStringList parentChildNameList = category.split("_");
        QString parentName = parentChildNameList[0];
        QString childName = parentChildNameList.mid(1).join("_");
        if(!parentMap.contains(parentName))
        {
            parentMap[parentName] = new CPoiCategory(parentName, widget);
        }
        new CPoiCategory(category, childName, categoryActivated[category], parentMap[parentName]);
    }
}

void CPoiPOI::slotCheckedStateChanged(QTreeWidgetItem * item)
{
    CPoiCategory* categoryItem = static_cast<CPoiCategory*>(item);
    if(categoryItem == nullptr)
    {
        return;
    }

    QString category = categoryItem->getCategory();
    if(category == "")
    {
        //This is a parent category
        return;
    }

    categoryActivated[category] = categoryItem->checkState();

    loadTimer->start();
}

void CPoiPOI::loadPOIsFromFile(const QString& category, int minLonM10, int minLatM10)
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
    query.prepare("SELECT main.poi_index.maxLat, main.poi_index.maxLon, main.poi_index.minLat, main.poi_index.minLon, main.poi_data.data"
                  " FROM main.poi_index"
                  " INNER JOIN main.poi_data ON main.poi_index.id=main.poi_data.id"
                  " WHERE main.poi_index.maxLat<:maxLat"
                  " AND main.poi_index.minLat>:minLat"
                  " AND main.poi_index.maxLon<:maxLon"
                  " AND main.poi_index.minLon>:minLon"
                  " AND " + tagMap[category]);
    query.bindValue(":maxLat",  QString::number((minLatM10 + 1) / 10., 'f'));
    query.bindValue(":minLat",  QString::number(minLatM10 / 10., 'f'));
    query.bindValue(":maxLon",  QString::number((minLonM10 + 1) / 10., 'f'));
    query.bindValue(":minLon",  QString::number(minLonM10 / 10., 'f'));
    query.exec();
    while (query.next())
    {
        rawPoi_t poi;
        poi.coordinates = QPointF((query.value(eSqlColumnMaxLon).toDouble() + query.value(eSqlColumnMinLon).toDouble()) / 2 * DEG_TO_RAD,
                                  (query.value(eSqlColumnMaxLat).toDouble() + query.value(eSqlColumnMinLat).toDouble()) / 2 * DEG_TO_RAD);
        poi.data = query.value(eSqlColumnData).toString().split("\r");
        mutex.lock();
        loadedPOIs[category][minLonM10][minLatM10].append(poi);
        mutex.unlock();
    }
}

poi_t CPoiPOI::rawPoi_t::toPoi(const QString& defaultName) const
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
        poi.name = defaultName;
    }
    poi.desc = data.join("<br>\n");
    return poi;
}
