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

#ifndef CPBFREADER_H
#define CPBFREADER_H

#include <QByteArray>
#include <QFile>
#include <QStringList>
#include <QVector>
#include <functional>

/**
   @brief Minimal reader for OpenStreetMap .osm.pbf files

   Reads nodes (plain and dense) and ways with their tags. Relations, metadata and history are
   skipped. Only uncompressed and zlib compressed blobs are supported, which covers the extracts
   of the common download services (e.g. Geofabrik).

   Locations are returned as integers in 1e-7 degrees, exactly as libosmium stores them.
 */
class CPbfReader {
 public:
  struct header_t {
    bool hasBBox = false;
    qreal left = 0;  ///< bounding box in degrees
    qreal right = 0;
    qreal top = 0;
    qreal bottom = 0;
    qint64 timestamp = 0;  ///< osmosis replication timestamp, seconds since epoch, 0 if unknown
    QStringList requiredFeatures;
    QString writingProgram;
    QString source;
  };

  struct way_t {
    qint64 id = 0;
    QVector<quint32> keys;  ///< indices into the block's string table
    QVector<quint32> vals;
    QVector<qint64> refs;  ///< node ids
  };

  enum what_e { eNodes = 0x01, eWays = 0x02 };

  /// called per node: id, latitude and longitude in 1e-7 degrees
  using fNode = std::function<void(qint64 id, qint32 lat, qint32 lon)>;
  /// called per way, with the string table of its block
  using fWay = std::function<void(const way_t& way, const QVector<QByteArray>& strings)>;
  /// called after each block with the file position, return false to abort
  using fProgress = std::function<bool(qint64 pos)>;

  explicit CPbfReader(const QString& filename);
  ~CPbfReader() = default;

  /// open the file and read its header block
  bool open();
  const header_t& header() const { return hdr; }
  qint64 size() const { return file.size(); }
  QString errorString() const { return error; }

  /**
     @brief Read all data blocks from the start of the file
     @param what      a combination of what_e
     @param onNode    called for each node if eNodes is set
     @param onWay     called for each way if eWays is set
     @param progress  called after each block, may be empty
     @return false on error or abort, see errorString()
   */
  bool readData(qint32 what, const fNode& onNode, const fWay& onWay, const fProgress& progress);

  /// decompress and parse a single PrimitiveBlock (exposed for tests)
  static void parsePrimitiveBlock(const QByteArray& data, qint32 what, const fNode& onNode, const fWay& onWay);
  static void parseHeaderBlock(const QByteArray& data, header_t& hdr);

 private:
  bool nextBlob(QByteArray& type, QByteArray& data);

  QFile file;
  QString error;
  header_t hdr;
  qint64 posData = 0;
};

#endif  // CPBFREADER_H
