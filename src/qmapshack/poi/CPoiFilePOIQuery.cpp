/**********************************************************************************************
    Copyright (C) 2026 Gert Pellin <gert@pellin.be>

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

#include "poi/CPoiFilePOIQuery.h"

#include <QSqlQuery>
#include <QStringList>

CPoiFilePOIQuery::Index CPoiFilePOIQuery::indexForVersion(const QString& version) {
  if (version == "2" || version == "4") {
    return Index::eRTree;
  }
  if (version == "3") {
    return Index::eLatLon;
  }
  return Index::eUnsupported;
}

QString CPoiFilePOIQuery::selectPoisInSquare() {
  return "SELECT (main.poi_index.minLat + main.poi_index.maxLat) / 2, "
         "(main.poi_index.minLon + main.poi_index.maxLon) / 2, "
         "main.poi_data.data, main.poi_data.id "
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
         "AND main.poi_data.id = main.poi_index.id";
}

void CPoiFilePOIQuery::bindSquare(QSqlQuery& query, quint64 categoryID, qint32 minLonM10, qint32 minLatM10) {
  query.bindValue(":maxLat", (minLatM10 + 1) / 10.);
  query.bindValue(":minLat", minLatM10 / 10.);
  query.bindValue(":maxLon", (minLonM10 + 1) / 10.);
  query.bindValue(":minLon", minLonM10 / 10.);
  query.bindValue(":categoryID", categoryID);
}

QString CPoiFilePOIQuery::selectPoisInRow(qsizetype categoryCount) {
  QStringList categories;
  for (qsizetype i = 0; i < categoryCount; i++) {
    categories << QString(":category%1").arg(i);
  }

  // CROSS JOIN fixes the order: walk the latitude index (it holds the id), look up the categories of each POI by
  // its id, and read position and data of the matches only. The unary + keeps SQLite from using the longitude and
  // category columns as index instead, which scans the whole file.
  return QString(
             "SELECT position.lat, position.lon, main.poi_data.data, main.poi_data.id, "
             "main.poi_category_map.category "
             "FROM main.poi_index AS band "
             "CROSS JOIN main.poi_category_map ON main.poi_category_map.id = band.id "
             "CROSS JOIN main.poi_index AS position ON position.id = band.id "
             "CROSS JOIN main.poi_data ON main.poi_data.id = band.id "
             "WHERE band.lat<:maxLat "
             "AND band.lat>=:minLat "
             "AND +main.poi_category_map.category IN (%1) "
             "AND +position.lon<:maxLon "
             "AND +position.lon>=:minLon")
      .arg(categories.join(", "));
}

void CPoiFilePOIQuery::bindRow(QSqlQuery& query, const QList<quint64>& categoryIDs, qint32 minLonM10, qint32 maxLonM10,
                               qint32 minLatM10) {
  query.bindValue(":maxLat", (minLatM10 + 1) / 10.);
  query.bindValue(":minLat", minLatM10 / 10.);
  query.bindValue(":maxLon", (maxLonM10 + 1) / 10.);
  query.bindValue(":minLon", minLonM10 / 10.);
  for (qsizetype i = 0; i < categoryIDs.size(); i++) {
    // a quint64 is bound as text, which the unary + leaves without the column's integer affinity
    query.bindValue(QString(":category%1").arg(i), static_cast<qint64>(categoryIDs[i]));
  }
}
