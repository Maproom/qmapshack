/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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
#ifndef CGARMINPLACES_H
#define CGARMINPLACES_H

#include <QHash>
#include <QString>
#include <QVector>
#include <functional>

#include "map/garmin/CGarminPoint.h"

class CFileExt;
class IGarminStrTbl;

/**
   @brief The place tables of a tile's LBL part (country, region, city, zip) and the POI records (LBL6) that refer
          to them, as mkgmap's POIRecord and PlacesFile write them.

   Indexes into the tables are 1-based. A city is either a label or a reference to an indexed point of a
   subdivision, whose label is the city's name.
 */
class CGarminPlaces {
 public:
  /** One table: file offset, size and record size. */
  struct section_t {
    quint32 offset = 0;
    quint32 size = 0;
    quint16 recSize = 0;
    quint32 count() const { return recSize == 0 ? 0 : size / recSize; }
  };

  struct layout_t {
    section_t country;
    section_t region;
    section_t city;
    section_t zip;
    section_t highway;          /**< LBL9 */
    section_t exitFacility;     /**< LBL10 */
    quint32 offsetPoiProps = 0; /**< file offset of LBL6 */
    quint32 sizePoiProps = 0;
    quint8 globalFlags = 0; /**< the fields a POI record has unless it carries its own flags */
  };

  /** Read @p size bytes at the file offset; the XOR mask removed. */
  using read_t = std::function<void(CFileExt& file, quint32 offset, quint32 size, QByteArray& data)>;
  /** The label of indexed point @p index (1-based) in subdivision @p subdiv (1-based); empty if none. */
  using point_label_t = std::function<QString(CFileExt& file, quint32 subdiv, quint32 index)>;

  CGarminPlaces(const layout_t& layout, IGarminStrTbl& strtbl, const read_t& read);

  /**
     @brief Read house number, street, city, region, country, zip, phone and highway exit of the POI record at
            @p offset into LBL6.

     @param pointLabel  resolves a city given as a point reference
   */
  void readDetails(CFileExt& file, quint32 offset, const point_label_t& pointLabel, CGarminPoint& p);

  /**
     @brief A number written as base-11 digits, two per byte: '0'-'9' and 10 as '-'.

     The first and the last byte have bit 7 set; a single byte is followed by 0xF8. Trailing '-' are dropped, as
     mkgmap's reader does.

     @param data  the bytes, the first one with bit 7 set; advanced past the number
     @param end   end of the bytes
     @return the number, empty if it runs past @p end
   */
  static QString base11(const quint8*& data, const quint8* end);

 private:
  struct city_t {
    QString name;
    QString region;
    QString country;
  };

  /** The fields of one POI record, as readDetails() fills them into a point. */
  struct details_t {
    QString houseNumber;
    QString street;
    QString city;
    QString region;
    QString country;
    QString zip;
    QString phone;
    bool isExit = false;
    QString exitHighway;
    QString exitTo;
    bool exitOvernightParking = false;
    QVector<CGarminPoint::exit_facility_t> exitFacilities;
  };

  void decodeDetails(CFileExt& file, quint32 offset, const point_label_t& pointLabel, details_t& p);

  QString label(CFileExt& file, quint32 offset);
  /** The first 3 bytes of a table record at @p index (1-based), a label offset; 0xFFFFFFFF if out of range */
  quint32 recordLabel(CFileExt& file, const section_t& section, quint32 index);
  /** The facilities of an exit, from record @p index on to the one marked last. */
  void readFacilities(CFileExt& file, quint32 index, details_t& p);
  QString country(CFileExt& file, quint32 index);
  const city_t& city(CFileExt& file, quint32 index, const point_label_t& pointLabel);

  layout_t layout;
  IGarminStrTbl& strtbl;
  read_t read;

  /** Resolved cities by index and POI records by LBL6 offset. Only the draw thread decodes, so no lock. */
  QHash<quint32, city_t> cities;
  QHash<quint32, details_t> details;
};

#endif  // CGARMINPLACES_H
