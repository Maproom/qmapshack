/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "map/garmin/CGarminImg.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QtMath>
#include <algorithm>
#include <limits>

#include "helpers/CFileExt.h"
#include "helpers/Platform.h"
#include "map/garmin/CGarminStrTbl6.h"
#include "map/garmin/CGarminStrTbl8.h"
#include "map/garmin/CGarminStrTblUtf8.h"

#undef DEBUG_SHOW_SECT_DESC
#undef DEBUG_SHOW_TRE_DATA
#undef DEBUG_SHOW_SUBDIV_DATA
#undef DEBUG_SHOW_MAPLEVELS

int CFileExt::cnt = 0;

static inline bool isCompletelyOutside(const QPolygonF& poly, const QRectF& viewport) {
  // a line cut off by the section's end
  if (poly.isEmpty()) {
    return true;
  }

  qreal north = qDegreesToRadians(-90.0);
  qreal south = qDegreesToRadians(90.0);
  qreal west = qDegreesToRadians(180.0);
  qreal east = qDegreesToRadians(-180.0);

  for (const QPointF& pt : poly) {
    if (north < pt.y()) {
      north = pt.y();
    }
    if (south > pt.y()) {
      south = pt.y();
    }
    if (west > pt.x()) {
      west = pt.x();
    }
    if (east < pt.x()) {
      east = pt.x();
    }
  }

  QRectF ref(west, north, east - west, south - north);

  if (ref.width() == 0) {
    ref.setWidth(0.00001);
  }
  if (ref.height() == 0) {
    ref.setHeight(0.00001);
  }

  return !viewport.intersects(ref);
}

CGarminImg::CGarminImg(const QString& filename) : filename(filename) {}

CGarminImg::~CGarminImg() = default;

void CGarminImg::read() {
  readBasics();
  processPrimaryMapData();
  if (maplevels.isEmpty()) {
    throw exce_t(
        errFormat,
        QCoreApplication::translate("CMapIMG", "The file holds no map tile, e.g. a search index: ") + filename);
  }
}

void CGarminImg::readFile(CFileExt& file, quint32 offset, quint32 size, QByteArray& data) const {
  if (offset + size > file.size()) {
    throw exce_t(eErrOpen, QCoreApplication::translate("CMapIMG", "Failed to read: ") + filename);
  }

  data = QByteArray::fromRawData(file.data(offset, size), size);
  // wenn mask == 0 ist kein xor noetig
  if (mask == 0) {
    return;
  }

#ifdef HOST_IS_64_BIT
  quint64* p64 = reinterpret_cast<quint64*>(data.data());
  for (quint32 i = 0; i < size / 8; ++i) {
    *p64++ ^= mask64;
  }
  quint32 rest = size % 8;
  quint8* p = reinterpret_cast<quint8*>(p64);
#else
  quint32* p32 = reinterpret_cast<quint32*>(data.data());
  for (quint32 i = 0; i < size / 4; ++i) {
    *p32++ ^= mask32;
  }
  quint32 rest = size % 4;
  quint8* p = reinterpret_cast<quint8*>(p32);
#endif

  for (quint32 i = 0; i < rest; ++i) {
    *p++ ^= mask;
  }
}

void CGarminImg::readBasics() {
  char tmpstr[64];
  qint64 fsize = QFileInfo(filename).size();
  // Garmin devices cannot load larger files (FAT32); subfile offsets are 32 bit.
  if (fsize > std::numeric_limits<quint32>::max()) {
    throw exce_t(errFormat, QCoreApplication::translate(
                                "CMapIMG", "The file is larger than 4 GB. Garmin devices cannot load it: ") +
                                filename);
  }

  CFileExt file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw exce_t(eErrOpen, QCoreApplication::translate("CMapIMG", "Failed to open: ") + filename);
  }

  mask = static_cast<quint8>(*file.data(0, 1));

  mask32 = mask;
  mask32 <<= 8;
  mask32 |= mask;
  mask32 <<= 8;
  mask32 |= mask;
  mask32 <<= 8;
  mask32 |= mask;

  mask64 = mask32;
  mask64 <<= 32;
  mask64 |= mask32;

  // read hdr_img_t
  QByteArray imghdr;
  readFile(file, 0, sizeof(hdr_img_t), imghdr);
  hdr_img_t* pImgHdr = reinterpret_cast<hdr_img_t*>(imghdr.data());

  if (strncmp(pImgHdr->signature, "DSKIMG", 7) != 0) {
    throw exce_t(errFormat, QCoreApplication::translate("CMapIMG", "Bad file format: ") + filename);
  }
  if (strncmp(pImgHdr->identifier, "GARMIN", 7) != 0) {
    throw exce_t(errFormat, QCoreApplication::translate("CMapIMG", "Bad file format: ") + filename);
  }

  mapdesc = QByteArray((const char*)pImgHdr->desc1, 20);
  mapdesc += pImgHdr->desc2;
  qDebug() << mapdesc;

  size_t blocksize = pImgHdr->blocksize();

  // 1st read FAT
  QByteArray FATblock;
  readFile(file, sizeof(hdr_img_t), sizeof(FATblock_t), FATblock);
  const FATblock_t* pFATBlock = reinterpret_cast<const FATblock_t*>(FATblock.data());

  size_t dataoffset = sizeof(hdr_img_t);

  // skip dummy blocks at the beginning
  while (dataoffset < static_cast<size_t>(fsize)) {
    if (pFATBlock->flag != 0x00) {
      break;
    }
    dataoffset += sizeof(FATblock_t);
    readFile(file, quint32(dataoffset), quint32(sizeof(FATblock_t)), FATblock);
    pFATBlock = reinterpret_cast<const FATblock_t*>(FATblock.data());
  }

  // start of new subfile part
  /*
      It is taken for granted that the single subfile parts are not
      fragmented within the file. Thus it is not really necessary to
      store and handle all block sequence numbers. Just the first one
      will give us the offset. This also implies that it is not necessary
      to care about FAT blocks with a non-zero part number.

      2007-03-31: Garmin's world base map seems to be coded different.
                  The part field seems to be rather a bit field than
                  a part number. As the total subfile size is given
                  for the first part only (for all others it's zero)
                  I use it to identify the 1st part of a subfile

      2007-05-26: Gmapsupp images by Sendmap code quite some bull shit,
                  too. The size is stored in every part and they do have
                  a part number. I introduced a set of subfile names
                  storing the subfile's name and type. The first part
                  with a size info and it's name / type not stored in the
                  set is used to get the location information.
   */
  QSet<QString> subfileNames;
  while (dataoffset < static_cast<size_t>(fsize)) {
    if (pFATBlock->flag != 0x01) {
      break;
    }

    memcpy(tmpstr, pFATBlock->name, sizeof(pFATBlock->name) + sizeof(pFATBlock->type));
    tmpstr[sizeof(pFATBlock->name) + sizeof(pFATBlock->type)] = 0;

    if (gar_load(quint32, pFATBlock->size) != 0 && !subfileNames.contains(tmpstr) && tmpstr[0] != 0x20) {
      subfileNames << tmpstr;

      memcpy(tmpstr, pFATBlock->name, sizeof(pFATBlock->name));
      tmpstr[sizeof(pFATBlock->name)] = 0;

      // skip MAPSORC.MPS section
      if (strcmp(tmpstr, "MAPSOURC") && strcmp(tmpstr, "SENDMAP2")) {
        subfile_desc_t& subfile = subfiles[tmpstr];
        subfile.name = tmpstr;

        memcpy(tmpstr, pFATBlock->type, sizeof(pFATBlock->type));
        tmpstr[sizeof(pFATBlock->type)] = 0;

        subfile_part_t& part = subfile.parts[tmpstr];
        part.size = gar_load(quint32, pFATBlock->size);
        part.offset = quint32(gar_load(uint16_t, pFATBlock->blocks[0]) * blocksize);
      }
    }

    dataoffset += sizeof(FATblock_t);
    readFile(file, quint32(dataoffset), quint32(sizeof(FATblock_t)), FATblock);
    pFATBlock = reinterpret_cast<const FATblock_t*>(FATblock.data());
  }

  if ((dataoffset == sizeof(hdr_img_t)) || (dataoffset >= static_cast<size_t>(fsize))) {
    throw exce_t(errFormat, QCoreApplication::translate("CMapIMG", "Failed to read file structure: ") + filename);
  }

  // gmapsupp.img files do not have a data offset field
  if (gar_load(quint32, pImgHdr->dataoffset) == 0) {
    pImgHdr->dataoffset = gar_load(quint32, dataoffset);
  }

  // sometimes there are dummy blocks at the end of the FAT
  //    if(gar_load(quint32, pImgHdr->dataoffset) != dataoffset)
  //    {
  //        dataoffset = gar_load(quint32, pImgHdr->dataoffset);
  //    }

#ifdef DEBUG_SHOW_SECT_DESC
  {
    QMap<QString, subfile_desc_t>::const_iterator subfile = subfiles.begin();
    while (subfile != subfiles.end()) {
      qDebug() << "--- subfile" << subfile->name << "---";
      QMap<QString, subfile_part_t>::const_iterator part = subfile->parts.begin();
      while (part != subfile->parts.end()) {
        qDebug() << part.key() << Qt::hex << part->offset << part->size;
        ++part;
      }
      ++subfile;
    }
  }
#endif  // DEBUG_SHOW_SECT_DESC

  maparea = QRectF();
  QMap<QString, subfile_desc_t>::iterator subfile = subfiles.begin();
  while (subfile != subfiles.end()) {
    if ((*subfile).parts.contains("GMP")) {
      throw exce_t(errFormat,
                   QCoreApplication::translate(
                       "CMapIMG", "File is NT format. QMapShack is unable to read map files with NT format: ") +
                       filename);
    }

    readSubfileBasics(*subfile, file);

    ++subfile;
  }

  qDebug() << "dimensions:\t" << "N" << qRadiansToDegrees(maparea.bottom()) << "E" << qRadiansToDegrees(maparea.right())
           << "S" << qRadiansToDegrees(maparea.top()) << "W" << qRadiansToDegrees(maparea.left());
}

void CGarminImg::readSubfileBasics(subfile_desc_t& subfile, CFileExt& file) {
  // test for mandatory subfile parts
  if (!(subfile.parts.contains("TRE") && subfile.parts.contains("RGN"))) {
    return;
  }

  // Both parts are guaranteed to exist by the guard above; bind them once instead of
  // repeating the map lookup for every offset computed below.
  const subfile_part_t& trePart = subfile.parts["TRE"];
  const subfile_part_t& rgnPart = subfile.parts["RGN"];

  QByteArray trehdr;
  readFile(file, trePart.offset, sizeof(hdr_tre_t), trehdr);
  hdr_tre_t* pTreHdr = reinterpret_cast<hdr_tre_t*>(trehdr.data());

  subfile.isTransparent = pTreHdr->POI_flags & 0x02;
  transparent = subfile.isTransparent ? true : transparent;

#ifdef DEBUG_SHOW_TRE_DATA
  qDebug() << "+++" << subfile.name << "+++";
  qDebug() << "TRE header length  :" << gar_load(uint16_t, pTreHdr->length);
  qDebug() << "TRE1 offset        :" << Qt::hex << gar_load(quint32, pTreHdr->tre1_offset);
  qDebug() << "TRE1 size          :" << dec << gar_load(quint32, pTreHdr->tre1_size);
  qDebug() << "TRE2 offset        :" << Qt::hex << gar_load(quint32, pTreHdr->tre2_offset);
  qDebug() << "TRE2 size          :" << dec << gar_load(quint32, pTreHdr->tre2_size);
#endif  // DEBUG_SHOW_TRE_DATA

  copyrights << QString(file.data(trePart.offset + gar_load(uint16_t, pTreHdr->length), 0x7FFF));

  // read map boundaries from header
  qint32 i32;
  i32 = gar_ptr_load(int24_t, &pTreHdr->northbound);
  subfile.north = GARMIN_RAD(i32);
  i32 = gar_ptr_load(int24_t, &pTreHdr->eastbound);
  subfile.east = GARMIN_RAD(i32);
  i32 = gar_ptr_load(int24_t, &pTreHdr->southbound);
  subfile.south = GARMIN_RAD(i32);
  i32 = gar_ptr_load(int24_t, &pTreHdr->westbound);
  subfile.west = GARMIN_RAD(i32);

  if (subfile.east == subfile.west) {
    subfile.east = -subfile.east;
  }

  if (subfile.west > 0 && subfile.east < 0) {
    subfile.east = -subfile.east;
  }

  subfile.area = QRectF(QPointF(subfile.west, subfile.north), QPointF(subfile.east, subfile.south));

  if (maparea.isNull()) {
    maparea = subfile.area;
  } else {
    maparea = maparea.united(subfile.area);
  }

#ifdef DEBUG_SHOW_TRE_DATA
  qDebug() << "bounding area (\260)" << qRadiansToDegrees(subfile.north) << qRadiansToDegrees(subfile.east)
           << qRadiansToDegrees(subfile.south) << qRadiansToDegrees(subfile.west);
  qDebug() << "bounding area (rad)" << subfile.area;
#endif  // DEBUG_SHOW_TRE_DATA

  QByteArray maplevel;
  readFile(file, trePart.offset + gar_load(quint32, pTreHdr->tre1_offset), gar_load(quint32, pTreHdr->tre1_size),
           maplevel);
  const tre_map_level_t* pMapLevel = reinterpret_cast<const tre_map_level_t*>(maplevel.data());

  if (pTreHdr->flag & 0x80) {
    throw exce_t(errLock, QCoreApplication::translate("CMapIMG",
                                                      "File contains locked / encrypted data. Garmin does not "
                                                      "want you to use this file with any other software than "
                                                      "the one supplied by Garmin."));
  }

  quint32 nlevels = gar_load(quint32, pTreHdr->tre1_size) / sizeof(tre_map_level_t);
  quint32 nsubdivs = 0;
  quint32 nsubdivs_last = 0;

  // count subsections
  for (quint32 i = 0; i < nlevels; ++i) {
    maplevel_t ml;
    ml.inherited = TRE_MAP_INHER(pMapLevel);
    ml.level = TRE_MAP_LEVEL(pMapLevel);
    ml.bits = pMapLevel->bits;
    subfile.maplevels << ml;
    nsubdivs += gar_load(uint16_t, pMapLevel->nsubdiv);
    nsubdivs_last = gar_load(uint16_t, pMapLevel->nsubdiv);
#ifdef DEBUG_SHOW_MAPLEVEL_DATA
    qDebug() << "level" << TRE_MAP_LEVEL(pMapLevel) << "inherited" << TRE_MAP_INHER(pMapLevel) << "bits"
             << pMapLevel->bits << "#subdivs" << gar_load(uint16_t, pMapLevel->nsubdiv);
#endif  // DEBUG_SHOW_MAPLEVEL_DATA
    ++pMapLevel;
  }

  quint32 nsubdivs_next = nsubdivs - nsubdivs_last;

  //////////////////////////////////
  // read subdivision information
  //////////////////////////////////
  // point to first map level definition
  pMapLevel = (const tre_map_level_t*)maplevel.data();
  // number of subdivisions per map level
  quint32 nsubdiv = gar_load(uint16_t, pMapLevel->nsubdiv);

  // point to first 16 byte subdivision definition entry
  QByteArray subdiv_n;
  readFile(file, trePart.offset + gar_load(quint32, pTreHdr->tre2_offset), gar_load(quint32, pTreHdr->tre2_size),
           subdiv_n);
  tre_subdiv_next_t* pSubDivN = reinterpret_cast<tre_subdiv_next_t*>(subdiv_n.data());

  QVector<subdiv_desc_t> subdivs;
  subdivs.resize(nsubdivs);
  QVector<subdiv_desc_t>::iterator subdiv = subdivs.begin();
  QVector<subdiv_desc_t>::iterator subdiv_prev = subdivs.end();

  // absolute offset of RGN data
  QByteArray rgnhdr;
  readFile(file, rgnPart.offset, sizeof(hdr_rgn_t), rgnhdr);
  hdr_rgn_t* pRgnHdr = reinterpret_cast<hdr_rgn_t*>(rgnhdr.data());
  quint32 rgnoff = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->offset);

  quint32 rgnOffPolyg2 = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->offset_polyg2);
  quint32 rgnOffPolyl2 = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->offset_polyl2);
  quint32 rgnOffPoint2 = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->offset_point2);

  quint32 rgnLenPolyg2 = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->length_polyg2);
  quint32 rgnLenPolyl2 = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->length_polyl2);
  quint32 rgnLenPoint2 = /*subfile.parts["RGN"].offset +*/ gar_load(quint32, pRgnHdr->length_point2);

  //     qDebug() << "***" << Qt::hex << subfile.parts["RGN"].offset << (subfile.parts["RGN"].offset +
  //     subfile.parts["RGN"].size); qDebug() << "+++" << Qt::hex << rgnOffPolyg2 << (rgnOffPolyg2 + rgnLenPolyg2);
  //     qDebug() << "+++" << Qt::hex << rgnOffPolyl2 << (rgnOffPolyl2 + rgnLenPolyl2);
  //     qDebug() << "+++" << Qt::hex << rgnOffPoint2 << (rgnOffPoint2 + rgnLenPoint2);

  // parse all 16 byte subdivision entries
  quint32 i;
  for (i = 0; i < nsubdivs_next; ++i, --nsubdiv) {
    subdiv->n = i;
    subdiv->next = gar_load(uint16_t, pSubDivN->next);
    subdiv->terminate = TRE_SUBDIV_TERM(pSubDivN);
    subdiv->rgn_start = gar_ptr_load(uint24_t, &pSubDivN->rgn_offset);
    subdiv->rgn_start += rgnoff;
    // skip if this is the first entry
    if (subdiv_prev != subdivs.end()) {
      subdiv_prev->rgn_end = subdiv->rgn_start;
    }

    subdiv->hasPoints = pSubDivN->elements & 0x10;
    subdiv->hasIdxPoints = pSubDivN->elements & 0x20;
    subdiv->hasPolylines = pSubDivN->elements & 0x40;
    subdiv->hasPolygons = pSubDivN->elements & 0x80;

    // if all subdivisions of this level have been parsed, switch to the next one
    if (nsubdiv == 0) {
      ++pMapLevel;
      nsubdiv = gar_load(uint16_t, pMapLevel->nsubdiv);
    }

    subdiv->level = TRE_MAP_LEVEL(pMapLevel);
    subdiv->shift = 24 - pMapLevel->bits;

    qint32 cx = gar_ptr_load(uint24_t, &pSubDivN->center_lng);
    subdiv->iCenterLng = cx;
    qint32 cy = gar_ptr_load(uint24_t, &pSubDivN->center_lat);
    subdiv->iCenterLat = cy;
    qint32 width = TRE_SUBDIV_WIDTH(pSubDivN) << subdiv->shift;
    qint32 height = gar_load(uint16_t, pSubDivN->height) << subdiv->shift;

    subdiv->north = GARMIN_RAD(cy + height + 1);
    subdiv->south = GARMIN_RAD(cy - height);
    subdiv->east = GARMIN_RAD(cx + width + 1);
    subdiv->west = GARMIN_RAD(cx - width);

    subdiv->area = QRectF(QPointF(subdiv->west, subdiv->north), QPointF(subdiv->east, subdiv->south));

    subdiv->offsetPoints2 = 0;
    subdiv->lengthPoints2 = 0;
    subdiv->offsetPolylines2 = 0;
    subdiv->lengthPolylines2 = 0;
    subdiv->offsetPolygons2 = 0;
    subdiv->lengthPolygons2 = 0;

    subdiv_prev = subdiv;
    ++pSubDivN;
    ++subdiv;
  }

  // switch to last map level
  ++pMapLevel;
  // witch pointer to 14 byte subdivision sections
  tre_subdiv_t* pSubDivL = pSubDivN;
  // parse all 14 byte subdivision entries of last map level
  for (; i < nsubdivs; ++i) {
    subdiv->n = i;
    subdiv->next = 0;
    subdiv->terminate = TRE_SUBDIV_TERM(pSubDivL);
    subdiv->rgn_start = gar_ptr_load(uint24_t, &pSubDivL->rgn_offset);
    subdiv->rgn_start += rgnoff;
    subdiv_prev->rgn_end = subdiv->rgn_start;
    subdiv->hasPoints = pSubDivL->elements & 0x10;
    subdiv->hasIdxPoints = pSubDivL->elements & 0x20;
    subdiv->hasPolylines = pSubDivL->elements & 0x40;
    subdiv->hasPolygons = pSubDivL->elements & 0x80;

    subdiv->level = TRE_MAP_LEVEL(pMapLevel);
    subdiv->shift = 24 - pMapLevel->bits;

    qint32 cx = gar_ptr_load(uint24_t, &pSubDivL->center_lng);
    subdiv->iCenterLng = cx;
    qint32 cy = gar_ptr_load(uint24_t, &pSubDivL->center_lat);
    subdiv->iCenterLat = cy;
    qint32 width = TRE_SUBDIV_WIDTH(pSubDivL) << subdiv->shift;
    qint32 height = gar_load(uint16_t, pSubDivL->height) << subdiv->shift;

    subdiv->north = GARMIN_RAD(cy + height + 1);
    subdiv->south = GARMIN_RAD(cy - height);
    subdiv->east = GARMIN_RAD(cx + width + 1);
    subdiv->west = GARMIN_RAD(cx - width);

    subdiv->area = QRectF(QPointF(subdiv->west, subdiv->north), QPointF(subdiv->east, subdiv->south));

    subdiv->offsetPoints2 = 0;
    subdiv->lengthPoints2 = 0;
    subdiv->offsetPolylines2 = 0;
    subdiv->lengthPolylines2 = 0;
    subdiv->offsetPolygons2 = 0;
    subdiv->lengthPolygons2 = 0;

    subdiv_prev = subdiv;
    ++pSubDivL;
    ++subdiv;
  }
  subdivs.last().rgn_end =
      gar_load(quint32, pRgnHdr->hdr_rgn_t::offset) + gar_load(quint32, pRgnHdr->hdr_rgn_t::length);

  // read extended NT elements
  if ((gar_load(uint16_t, pTreHdr->hdr_subfile_part_t::length) >= 0x9A) && pTreHdr->tre7_size &&
      (gar_load(uint16_t, pTreHdr->tre7_rec_size) >= sizeof(tre_subdiv2_t))) {
    // rgnoff = subfile.parts["RGN"].offset;
    //          qDebug() << subdivs.count() << (pTreHdr->tre7_size / pTreHdr->tre7_rec_size) << pTreHdr->tre7_rec_size;
    QByteArray subdiv2;
    readFile(file, trePart.offset + gar_load(quint32, pTreHdr->tre7_offset), gar_load(quint32, pTreHdr->tre7_size),
             subdiv2);
    tre_subdiv2_t* pSubDiv2 = reinterpret_cast<tre_subdiv2_t*>(subdiv2.data());

    //        const quint32 entries1 = gar_load(quint32, pTreHdr->tre7_size) / gar_load(quint32,
    //        pTreHdr->tre7_rec_size); const quint32 entries2 = subdivs.size();

    bool skipPois = (gar_load(uint16_t, pTreHdr->tre7_rec_size) != sizeof(tre_subdiv2_t));

    //         for(int i = 0; i < pTreHdr->tre7_rec_size; ++i){
    //             if(i%4 == 0) fprintf(stderr,"\n");
    //             fprintf(stderr,"%02X ", ((quint8*)pSubDiv2)[i]);
    //         }
    //         fprintf(stderr,"\n");

    subdiv = subdivs.begin();
    subdiv_prev = subdivs.begin();
    subdiv->offsetPolygons2 = gar_load(quint32, pSubDiv2->offsetPolygons) + rgnOffPolyg2;
    subdiv->offsetPolylines2 = gar_load(quint32, pSubDiv2->offsetPolyline) + rgnOffPolyl2;
    subdiv->offsetPoints2 = skipPois ? 0 : gar_load(quint32, pSubDiv2->offsetPoints) + rgnOffPoint2;

    ++subdiv;
    pSubDiv2 = reinterpret_cast<tre_subdiv2_t*>(reinterpret_cast<quint8*>(pSubDiv2) +
                                                gar_endian(uint16_t, pTreHdr->tre7_rec_size));

    while (subdiv != subdivs.end()) {
      //             for(int i = 0; i < pTreHdr->tre7_rec_size; ++i){
      //                 if(i%4 == 0) fprintf(stderr,"\n");
      //                 fprintf(stderr,"%02X ", ((quint8*)pSubDiv2)[i]);
      //             }
      //             fprintf(stderr,"\n");

      subdiv->offsetPolygons2 = gar_load(quint32, pSubDiv2->offsetPolygons) + rgnOffPolyg2;
      subdiv->offsetPolylines2 = gar_load(quint32, pSubDiv2->offsetPolyline) + rgnOffPolyl2;
      subdiv->offsetPoints2 = skipPois ? 0 : gar_load(quint32, pSubDiv2->offsetPoints) + rgnOffPoint2;

      subdiv_prev->lengthPolygons2 = subdiv->offsetPolygons2 - subdiv_prev->offsetPolygons2;
      subdiv_prev->lengthPolylines2 = subdiv->offsetPolylines2 - subdiv_prev->offsetPolylines2;
      subdiv_prev->lengthPoints2 = skipPois ? 0 : subdiv->offsetPoints2 - subdiv_prev->offsetPoints2;

      subdiv_prev = subdiv;

      ++subdiv;
      pSubDiv2 = reinterpret_cast<tre_subdiv2_t*>(reinterpret_cast<quint8*>(pSubDiv2) +
                                                  gar_endian(uint16_t, pTreHdr->tre7_rec_size));
    }

    subdiv_prev->lengthPolygons2 = rgnOffPolyg2 + rgnLenPolyg2 - subdiv_prev->offsetPolygons2;
    subdiv_prev->lengthPolylines2 = rgnOffPolyl2 + rgnLenPolyl2 - subdiv_prev->offsetPolylines2;
    subdiv_prev->lengthPoints2 = skipPois ? 0 : rgnOffPoint2 + rgnLenPoint2 - subdiv_prev->offsetPoints2;
  }

  subfile.subdivs = subdivs;

#ifdef DEBUG_SHOW_SUBDIV_DATA
  {
    QVector<subdiv_desc_t>::iterator subdiv = subfile.subdivs.begin();
    while (subdiv != subfile.subdivs.end()) {
      qDebug() << "--- subdiv" << subdiv->n << "---";
      qDebug() << "RGN start          " << Qt::hex << subdiv->rgn_start;
      qDebug() << "RGN end            " << Qt::hex << subdiv->rgn_end;
      qDebug() << "center lng         " << GARMIN_DEG(subdiv->iCenterLng);
      qDebug() << "center lat         " << GARMIN_DEG(subdiv->iCenterLat);
      qDebug() << "has points         " << subdiv->hasPoints;
      qDebug() << "has indexed points " << subdiv->hasIdxPoints;
      qDebug() << "has polylines      " << subdiv->hasPolylines;
      qDebug() << "has polygons       " << subdiv->hasPolygons;
      qDebug() << "bounding area (m)  " << subdiv->area.topLeft() << subdiv->area.bottomRight();
      qDebug() << "map level          " << subdiv->level;
      qDebug() << "left shifts        " << subdiv->shift;

      qDebug() << "polyg off.         " << Qt::hex << subdiv->offsetPolygons2;
      qDebug() << "polyg len.         " << Qt::hex << subdiv->lengthPolygons2;
      qDebug() << "polyl off.         " << Qt::hex << subdiv->offsetPolylines2;
      qDebug() << "polyl len.         " << Qt::hex << subdiv->lengthPolylines2;
      qDebug() << "point off.         " << Qt::hex << subdiv->offsetPoints2;
      qDebug() << "point len.         " << Qt::hex << subdiv->lengthPoints2;
      ++subdiv;
    }
  }
#endif  // DEBUG_SHOW_SUBDIV_DATA

  //     qDebug() << "***" << Qt::hex << subfile.parts["RGN"].offset << (subfile.parts["RGN"].offset +
  //     subfile.parts["RGN"].size); qDebug() << "+++" << Qt::hex << rgnOffPolyg2 << (rgnOffPolyg2 +
  //     pRgnHdr->length_polyg2); qDebug() << "+++" << Qt::hex << rgnOffPolyl2 << (rgnOffPolyl2 +
  //     pRgnHdr->length_polyl2); qDebug() << "+++" << Qt::hex << rgnOffPoint2 << (rgnOffPoint2 +
  //     pRgnHdr->length_point2);

  if (subfile.parts.contains("LBL")) {
    const subfile_part_t& lblPart = subfile.parts["LBL"];

    QByteArray lblhdr;
    readFile(file, lblPart.offset, sizeof(hdr_lbl_t), lblhdr);
    hdr_lbl_t* pLblHdr = reinterpret_cast<hdr_lbl_t*>(lblhdr.data());

    quint32 offsetLbl1 = lblPart.offset + gar_load(quint32, pLblHdr->lbl1_offset);
    quint32 offsetLbl6 = lblPart.offset + gar_load(quint32, pLblHdr->lbl6_offset);

    QByteArray nethdr;
    quint32 offsetNet1 = 0;
    hdr_net_t* pNetHdr = nullptr;
    if (subfile.parts.contains("NET")) {
      const subfile_part_t& netPart = subfile.parts["NET"];
      readFile(file, netPart.offset, sizeof(hdr_net_t), nethdr);
      pNetHdr = reinterpret_cast<hdr_net_t*>(nethdr.data());
      offsetNet1 = netPart.offset + gar_load(quint32, pNetHdr->net1_offset);
    }

    quint16 codepage = 0;
    if (gar_load(uint16_t, pLblHdr->length) > 0xAA) {
      codepage = gar_load(uint16_t, pLblHdr->codepage);
    }

    //         qDebug() << file.fileName() << Qt::hex << offsetLbl1 << offsetLbl6 << offsetNet1;

    switch (pLblHdr->coding) {
      case 0x06:
        strtbls.push_back(std::make_unique<CGarminStrTbl6>(codepage, mask, nullptr));
        subfile.strtbl = strtbls.back().get();
        break;

      case 0x09:
        strtbls.push_back(std::make_unique<CGarminStrTbl8>(codepage, mask, nullptr));
        subfile.strtbl = strtbls.back().get();
        break;

      case 0x0A:
        strtbls.push_back(std::make_unique<CGarminStrTblUtf8>(codepage, mask, nullptr));
        subfile.strtbl = strtbls.back().get();
        break;

      default:
        qWarning() << "Unknown label coding" << Qt::hex << pLblHdr->coding;
    }

    if (nullptr != subfile.strtbl) {
      subfile.strtbl->registerLBL1(offsetLbl1, gar_load(quint32, pLblHdr->lbl1_length), pLblHdr->addr_shift);
      subfile.strtbl->registerLBL6(offsetLbl6, gar_load(quint32, pLblHdr->lbl6_length));
      if (nullptr != pNetHdr) {
        subfile.strtbl->registerNET1(offsetNet1, gar_load(quint32, pNetHdr->net1_length), pNetHdr->net1_addr_shift);
      }
    }
  }
}

void CGarminImg::processPrimaryMapData() {
  /*
   * Query all subfiles for possible maplevels.
   * Exclude basemap to avoid pollution.
   */
  for (const subfile_desc_t& subfile : std::as_const(subfiles)) {
    for (const maplevel_t& maplevel : subfile.maplevels) {
      if (!maplevel.inherited) {
        map_level_t ml;
        ml.bits = maplevel.bits;
        ml.level = maplevel.level;
        ml.useBaseMap = false;
        maplevels << ml;
      }
    }
  }

  /* Sort all entries, note that stable sort should insure that basemap is preferred when available. */
  std::stable_sort(maplevels.begin(), maplevels.end(), map_level_t::GreaterThan);
  /* Delete any duplicates for obvious performance reasons. */
  auto where = std::unique(maplevels.begin(), maplevels.end());
  maplevels.erase(where, maplevels.end());

#ifdef DEBUG_SHOW_MAPLEVELS
  for (int i = 0; i < maplevels.count(); ++i) {
    map_level_t& ml = maplevels[i];
    qDebug() << ml.bits << ml.level << ml.useBaseMap;
  }
#endif
}

qint32 CGarminImg::level(const subfile_desc_t& subfile, quint8 bits) {
  const maplevel_t* finest = nullptr;
  const maplevel_t* coarsest = nullptr;
  for (const maplevel_t& maplevel : subfile.maplevels) {
    // an inherited level holds no data
    if (maplevel.inherited) {
      continue;
    }
    if (maplevel.bits <= bits && (finest == nullptr || maplevel.bits >= finest->bits)) {
      finest = &maplevel;
    }
    if (coarsest == nullptr || maplevel.bits < coarsest->bits) {
      coarsest = &maplevel;
    }
  }
  const maplevel_t* chosen = finest != nullptr ? finest : coarsest;
  return chosen != nullptr ? qint32(chosen->level) : -1;
}

void CGarminImg::load(quint8 bits, const QRectF& viewport, const filter_t& filter, const std::function<bool()>& abort,
                      polytype_t& polygons, polytype_t& polylines, pointtype_t& points, pointtype_t& pois) {
#ifndef Q_OS_WIN32
  CFileExt file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    return;
  }
#endif

  for (const subfile_desc_t& subfile : std::as_const(subfiles)) {
    //        qDebug() << "-------";
    //        qDebug() << (viewport.topLeft() * RAD_TO_DEG) << (viewport.bottomRight() * RAD_TO_DEG);
    //        qDebug() << (subfile.area.topLeft() * RAD_TO_DEG) << (subfile.area.bottomRight() * RAD_TO_DEG);
    //        qDebug() << subfile.area.intersects(viewport);

    if (!subfile.area.intersects(viewport)) {
      continue;
    }

    if (abort()) {
      break;
    }

#ifdef Q_OS_WIN32
    CFileExt file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
      return;
    }
#endif

    QByteArray rgndata;
    readFile(file, subfile.parts["RGN"].offset, subfile.parts["RGN"].size, rgndata);

    // qDebug() << "rgn range" << Qt::hex << subfile.parts["RGN"].offset << (subfile.parts["RGN"].offset +
    // subfile.parts["RGN"].size);

    const qint32 tileLevel = level(subfile, bits);
    const QVector<subdiv_desc_t>& subdivs = subfile.subdivs;
    // collect polylines
    for (const subdiv_desc_t& subdiv : subdivs) {
      // if(subdiv.level == level) qDebug() << "subdiv:" << subdiv.level << level <<  subdiv.area << viewport <<
      // subdiv.area.intersects(viewport);
      if (qint32(subdiv.level) != tileLevel || !subdiv.area.intersects(viewport)) {
        continue;
      }
      if (abort()) {
        break;
      }
      loadSubDiv(file, subdiv, subfile.strtbl, rgndata, filter, viewport, polylines, polygons, points, pois);
    }

#ifdef Q_OS_WIN32
    file.close();
#else
    file.free();
#endif
  }

#ifndef Q_OS_WIN32
  file.close();
#endif
}

void CGarminImg::loadSubDiv(CFileExt& file, const subdiv_desc_t& subdiv, IGarminStrTbl* strtbl,
                            const QByteArray& rgndata, const filter_t& filter, const QRectF& viewport,
                            polytype_t& polylines, polytype_t& polygons, pointtype_t& points, pointtype_t& pois) {
  if (subdiv.rgn_start == subdiv.rgn_end && !subdiv.lengthPolygons2 && !subdiv.lengthPolylines2 &&
      !subdiv.lengthPoints2) {
    return;
  }
  // fprintf(stderr, "loadSubDiv\n");
  //      qDebug() << "---------" << file.fileName() << "---------";

  const quint8* pRawData = reinterpret_cast<const quint8*>(rgndata.constData());

  quint32 opnt = 0, oidx = 0, opline = 0, opgon = 0;
  quint32 objCnt = subdiv.hasIdxPoints + subdiv.hasPoints + subdiv.hasPolylines + subdiv.hasPolygons;

  const quint16* pOffset = reinterpret_cast<const quint16*>(pRawData + subdiv.rgn_start);

  // test for points
  if (subdiv.hasPoints) {
    opnt = (objCnt - 1) * sizeof(quint16) + subdiv.rgn_start;
  }
  // test for indexed points
  if (subdiv.hasIdxPoints) {
    if (opnt) {
      oidx = gar_load(uint16_t, *pOffset);
      oidx += subdiv.rgn_start;
      ++pOffset;
    } else {
      oidx = (objCnt - 1) * sizeof(quint16) + subdiv.rgn_start;
    }
  }
  // test for polylines
  if (subdiv.hasPolylines) {
    if (opnt || oidx) {
      opline = gar_load(uint16_t, *pOffset);
      opline += subdiv.rgn_start;
      ++pOffset;
    } else {
      opline = (objCnt - 1) * sizeof(quint16) + subdiv.rgn_start;
    }
  }
  // test for polygons
  if (subdiv.hasPolygons) {
    if (opnt || oidx || opline) {
      opgon = gar_load(uint16_t, *pOffset);
      opgon += subdiv.rgn_start;
      ++pOffset;
    } else {
      opgon = (objCnt - 1) * sizeof(quint16) + subdiv.rgn_start;
    }
  }

#ifdef DEBUG_SHOW_POLY_DATA
  qDebug() << "--- Subdivision" << subdiv.n << "---";
  qDebug() << "address:" << Qt::hex << subdiv.rgn_start << "- " << subdiv.rgn_end;
  qDebug() << "points:            " << Qt::hex << opnt;
  qDebug() << "indexed points:    " << Qt::hex << oidx;
  qDebug() << "polylines:         " << Qt::hex << opline;
  qDebug() << "polygons:          " << Qt::hex << opgon;
#endif  // DEBUG_SHOW_POLY_DATA

  CGarminPolygon p;

  // decode points
  if (subdiv.hasPoints && !filter.fast && filter.points) {
    const quint8* pData = pRawData + opnt;
    const quint8* pEnd = pRawData + (oidx ? oidx : opline ? opline : opgon ? opgon : subdiv.rgn_end);
    while (pData < pEnd) {
      CGarminPoint p;
      pData += p.decode(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, pData);

      // skip points outside our current viewport
      if (!viewport.contains(p.pos)) {
        continue;
      }

      if (strtbl) {
        p.isLbl6 ? strtbl->get(file, p.lbl_ptr, IGarminStrTbl::poi, p.labels)
                 : strtbl->get(file, p.lbl_ptr, IGarminStrTbl::norm, p.labels);
      }

      points.push_back(p);
    }
  }

  // decode indexed points
  if (subdiv.hasIdxPoints && !filter.fast && filter.points) {
    const quint8* pData = pRawData + oidx;
    const quint8* pEnd = pRawData + (opline ? opline : opgon ? opgon : subdiv.rgn_end);
    while (pData < pEnd) {
      CGarminPoint p;
      pData += p.decode(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, pData);

      // skip points outside our current viewport
      if (!viewport.contains(p.pos)) {
        continue;
      }

      if (strtbl) {
        p.isLbl6 ? strtbl->get(file, p.lbl_ptr, IGarminStrTbl::poi, p.labels)
                 : strtbl->get(file, p.lbl_ptr, IGarminStrTbl::norm, p.labels);
      }

      pois.push_back(p);
    }
  }

  // decode polylines
  if (subdiv.hasPolylines && !filter.fast && filter.polylines) {
    CGarminPolygon::cnt = 0;
    const quint8* pData = pRawData + opline;
    const quint8* pEnd = pRawData + (opgon ? opgon : subdiv.rgn_end);
    while (pData < pEnd) {
      pData += p.decode(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, true, pData, pEnd);

      // skip points outside our current viewport
      if (isCompletelyOutside(p.pixel, viewport)) {
        continue;
      }

      if (strtbl && !p.lbl_in_NET && p.lbl_info) {
        strtbl->get(file, p.lbl_info, IGarminStrTbl::norm, p.labels);
      } else if (strtbl && p.lbl_in_NET && p.lbl_info) {
        strtbl->get(file, p.lbl_info, IGarminStrTbl::net, p.labels);
      }

      polylines.push_back(p);
    }
  }

  // decode polygons
  if (subdiv.hasPolygons && filter.polygons) {
    CGarminPolygon::cnt = 0;
    const quint8* pData = pRawData + opgon;
    const quint8* pEnd = pRawData + subdiv.rgn_end;

    while (pData < pEnd) {
      pData += p.decode(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, false, pData, pEnd);

      // skip points outside our current viewport
      if (isCompletelyOutside(p.pixel, viewport)) {
        continue;
      }

      if (strtbl && !p.lbl_in_NET && p.lbl_info && !filter.fast) {
        strtbl->get(file, p.lbl_info, IGarminStrTbl::norm, p.labels);
      } else if (strtbl && p.lbl_in_NET && p.lbl_info && !filter.fast) {
        strtbl->get(file, p.lbl_info, IGarminStrTbl::net, p.labels);
      }
      polygons.push_back(p);
    }
  }

  //         qDebug() << "--- Subdivision" << subdiv.n << "---";
  //         qDebug() << "adress:" << Qt::hex << subdiv.rgn_start << "- " << subdiv.rgn_end;
  //         qDebug() << "polyg off: " << Qt::hex << subdiv.offsetPolygons2;
  //         qDebug() << "polyg len: " << Qt::hex << subdiv.lengthPolygons2 << dec << subdiv.lengthPolygons2;
  //         qDebug() << "polyg end: " << Qt::hex << subdiv.lengthPolygons2 + subdiv.offsetPolygons2;
  //         qDebug() << "polyl off: " << Qt::hex << subdiv.offsetPolylines2;
  //         qDebug() << "polyl len: " << Qt::hex << subdiv.lengthPolylines2 << dec << subdiv.lengthPolylines2;
  //         qDebug() << "polyl end: " << Qt::hex << subdiv.lengthPolylines2 + subdiv.offsetPolylines2;
  //         qDebug() << "point off: " << Qt::hex << subdiv.offsetPoints2;
  //         qDebug() << "point len: " << Qt::hex << subdiv.lengthPoints2 << dec << subdiv.lengthPoints2;
  //         qDebug() << "point end: " << Qt::hex << subdiv.lengthPoints2 + subdiv.offsetPoints2;

  if (subdiv.lengthPolygons2 && filter.polygons) {
    const quint8* pData = pRawData + subdiv.offsetPolygons2;
    const quint8* pEnd = pData + subdiv.lengthPolygons2;
    while (pData < pEnd) {
      //             qDebug() << "rgn offset:" << Qt::hex << (rgnoff + (pData - pRawData));
      pData += p.decode2(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, false, pData, pEnd);

      // skip points outside our current viewport
      if (isCompletelyOutside(p.pixel, viewport)) {
        continue;
      }

      if (strtbl && !p.lbl_in_NET && p.lbl_info && !filter.fast) {
        strtbl->get(file, p.lbl_info, IGarminStrTbl::norm, p.labels);
      }

      polygons.push_back(p);
    }
  }

  if (subdiv.lengthPolylines2 && !filter.fast && filter.polylines) {
    const quint8* pData = pRawData + subdiv.offsetPolylines2;
    const quint8* pEnd = pData + subdiv.lengthPolylines2;
    while (pData < pEnd) {
      //             qDebug() << "rgn offset:" << Qt::hex << (rgnoff + (pData - pRawData));
      pData += p.decode2(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, true, pData, pEnd);

      // skip points outside our current viewport
      if (isCompletelyOutside(p.pixel, viewport)) {
        continue;
      }

      if (strtbl && !p.lbl_in_NET && p.lbl_info) {
        strtbl->get(file, p.lbl_info, IGarminStrTbl::norm, p.labels);
      }

      polylines.push_back(p);
    }
  }

  if (subdiv.lengthPoints2 && !filter.fast && filter.points) {
    const quint8* pData = pRawData + subdiv.offsetPoints2;
    const quint8* pEnd = pData + subdiv.lengthPoints2;
    while (pData < pEnd) {
      CGarminPoint p;
      //             qDebug() << "rgn offset:" << Qt::hex << (rgnoff + (pData - pRawData));
      pData += p.decode2(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, pData, pEnd);

      // skip points cut off by the section's end and points outside our current viewport
      if (p.type == 0 || !viewport.contains(p.pos)) {
        continue;
      }

      if (strtbl) {
        p.isLbl6 ? strtbl->get(file, p.lbl_ptr, IGarminStrTbl::poi, p.labels)
                 : strtbl->get(file, p.lbl_ptr, IGarminStrTbl::norm, p.labels);
      }
      pois.push_back(p);
    }
  }
}
