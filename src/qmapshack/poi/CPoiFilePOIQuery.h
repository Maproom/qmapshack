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

#ifndef CPOIFILEPOIQUERY_H
#define CPOIFILEPOIQUERY_H

#include <QList>
#include <QString>

class QSqlQuery;

/**
   @brief SQL for the POI position index of a mapsforge POI file

   Version 2 and 4 keep the positions in an R-tree (minLat, maxLat, minLon, maxLon). Version 3 keeps them in a plain
   table (lat, lon) with an index on each column. There a query has to visit every POI of a full latitude row,
   whatever its longitude range, and look up its categories. So POIs are loaded by row segment and for several
   categories at once. Everything else CPoiFilePOI reads is the same in all three versions.

   Areas are given in squares of 0.1 degree, as multiples of 0.1 degree of their south west corner.
 */
class CPoiFilePOIQuery {
 public:
  enum class Index { eUnsupported, eRTree, eLatLon };

  /// Result columns of selectPoisInSquare() and selectPoisInRow()
  enum SqlColumnPoi_e {
    eSqlColumnPoiLat,
    eSqlColumnPoiLon,
    eSqlColumnPoiData,
    eSqlColumnPoiId,
    eSqlColumnPoiCategory /**< selectPoisInRow() only */
  };

  /**
     @brief Get the index layout for the value of the `version` metadata entry
     @param version  the version as stored in the file
     @return eUnsupported for any version that cannot be read
   */
  static Index indexForVersion(const QString& version);

  /**
     @brief Get the statement selecting all POIs of one category in one square, for Index::eRTree

     Bind the values with bindSquare().

     @return the SQL statement
   */
  static QString selectPoisInSquare();

  /**
     @brief Bind the values of the statement from selectPoisInSquare()
     @param query       the prepared query
     @param categoryID  the category of the POIs
     @param minLonM10   longitude of the square
     @param minLatM10   latitude of the square
   */
  static void bindSquare(QSqlQuery& query, quint64 categoryID, qint32 minLonM10, qint32 minLatM10);

  /**
     @brief Get the statement selecting all POIs of several categories in a row of squares, for Index::eLatLon

     A POI in more than one of the categories is returned once per category. Bind the values with bindRow().

     @param categoryCount  the number of categories, at least 1
     @return the SQL statement
   */
  static QString selectPoisInRow(qsizetype categoryCount);

  /**
     @brief Bind the values of the statement from selectPoisInRow()
     @param query        the prepared query
     @param categoryIDs  the categories of the POIs, as many as passed to selectPoisInRow()
     @param minLonM10    longitude of the first square of the row
     @param maxLonM10    longitude of the last square of the row
     @param minLatM10    latitude of the row
   */
  static void bindRow(QSqlQuery& query, const QList<quint64>& categoryIDs, qint32 minLonM10, qint32 maxLonM10,
                      qint32 minLatM10);
};

#endif  // CPOIFILEPOIQUERY_H
