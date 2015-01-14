/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CMAPIMG_H
#define CMAPIMG_H

#include "map/CMapDraw.h"
#include "map/IMap.h"
#include "map/garmin/CGarminPoint.h"
#include "map/garmin/CGarminPolygon.h"
#include "map/garmin/CGarminTyp.h"
#include "map/garmin/Garmin.h"

#include <QMap>

class CMapDraw;
class CFileExt;
class IGarminStrTbl;

typedef QVector<CGarminPolygon> polytype_t;
typedef QVector<CGarminPoint> pointtype_t;


class CMapIMG : public IMap
{
    Q_OBJECT
public:
    struct maplevel_t
    {
        bool inherited;
        quint8 level;
        quint8 bits;
    };

    /// subfile part (TRE, RGN, ...) location information
    struct subfile_part_t
    {
        subfile_part_t() : offset(0), size(0)
        {
        }
        /// file offset of subfile part
        quint32 offset;
        /// size of the subfile part
        quint32 size;
    };

    /// subdivision  information
    struct subdiv_desc_t
    {
        quint32 n;
        /// section of next level
        quint16 next;
        /// end of section group
        bool terminate;
        /// offset into the subfile's RGN part
        quint32 rgn_start;
        /// end of section in RGN part (last offset = rgn_end - 1)
        quint32 rgn_end;

        /// there are points stored in the RGN subsection
        bool hasPoints;
        /// there are indexd points stored in the RGN subsection
        bool hasIdxPoints;
        /// there are polylines stored in the RGN subsection
        bool hasPolylines;
        /// there are polygons stored in the RGN subsection
        bool hasPolygons;

        /// the center longitude of the area covered by this subdivision
        qint32 iCenterLng;
        /// the center latiude of the area covered by this subdivision
        qint32 iCenterLat;

        /// north boundary of area covered by this subsection []
        qreal north;
        /// east boundary of area covered by this subsection []
        qreal east;
        /// south boundary of area covered by this subsection []
        qreal south;
        /// west boundary of area covered by this subsection []
        qreal west;

        /// area in meter coordinates covered by this subdivision []
        QRectF area;

        /// number of left shifts for RGN data
        quint32 shift;
        /// map level this subdivision is shown
        quint32 level;

        quint32 offsetPoints2;
        qint32 lengthPoints2;
        quint32 offsetPolylines2;
        qint32 lengthPolylines2;
        quint32 offsetPolygons2;
        qint32 lengthPolygons2;
    };

    struct subfile_desc_t
    {
        subfile_desc_t() : north(0.0), east(0.0), south(0.0), west(0.0), isTransparent(false), strtbl(0)
        {
        }

        /// the name of the subfile (not really needed)
        QString name;
        /// location information of all parts
        QMap<QString,subfile_part_t> parts;

        /// north boundary of area covered by this subfile [rad]
        qreal north;
        /// east boundary of area covered by this subfile [rad]
        qreal east;
        /// south boundary of area covered by this subfile [rad]
        qreal south;
        /// west boundary of area covered by this subfile [rad]
        qreal west;

        /// area in [] covered by this subfile
        QRectF area;

        /// list of subdivisions
        QVector<subdiv_desc_t> subdivs;
        /// used maplevels
        QVector<maplevel_t> maplevels;
        /// bit 1 of POI_flags (TRE header @ 0x3F)
        bool isTransparent;
        /// object to manage the string tables
        IGarminStrTbl * strtbl;
    };

    CMapIMG(const QString &filename, CMapDraw *parent);
    virtual ~CMapIMG();

    void draw(IDrawContext::buffer_t& buf);

    void getInfo(const QPoint& px, QString& str);
    void getToolTip(const QPoint& px, QString& infotext);

    /**
       @brief Find a matching street polyline

       The polyline must be close enough in terms of pixel to point 1 and 2. "Close enough" is defined by
       the threshold. The returned poylline uses lon/lat as coordinates.

       @param pt1           first point in [rad]
       @param pt2           second point in [rad]
       @param threshold     the "close enough" threshold in [pixel]
       @param polyline      the resulting polyline, if any, in [rad]
       @return              Return true if a line has been found.
     */
    bool findPolylineCloseBy(QPointF& pt1, QPointF& pt2, qint32 threshold, QPolygonF& polyline);


private:
    enum exce_e {eErrOpen, eErrAccess, errFormat, errLock, errAbort};
    struct exce_t
    {
        exce_t(exce_e err, const QString& msg) : err(err), msg(msg)
        {
        }
        exce_e err;
        QString msg;
    };
    struct strlbl_t
    {
        strlbl_t() : type(CGarminTyp::eStandard)
        {
        }

        QPoint pt;
        QRect rect;
        QString str;
        CGarminTyp::label_type_e type;
    };


    quint8 scale2bits(const QPointF &scale);
    void setupTyp();
    void readBasics();
    void readSubfileBasics(subfile_desc_t& subfile, CFileExt &file);
    void processPrimaryMapData();
    void readFile(CFileExt& file, quint32 offset, quint32 size, QByteArray& data);
    void loadVisibleData(bool fast, polytype_t& polygons, polytype_t& polylines, pointtype_t& points, pointtype_t& pois, unsigned level, const QRectF& viewport,QPainter& p);
    void loadSubDiv(CFileExt &file, const subdiv_desc_t& subdiv, IGarminStrTbl * strtbl, const QByteArray& rgndata, bool fast, const QRectF& viewport, polytype_t& polylines, polytype_t& polygons, pointtype_t& points, pointtype_t& pois);
    void drawPolygons(QPainter& p, polytype_t& lines);
    void drawPolylines(QPainter& p, polytype_t& lines, const QPointF &scale);
    void drawPoints(QPainter& p, pointtype_t& pts, QVector<QRectF> &rectPois);
    void drawPois(QPainter& p, pointtype_t& pts, QVector<QRectF>& rectPois);
    void drawLabels(QPainter& p, const QVector<strlbl_t> &lbls);
    void drawText(QPainter& p);

    void drawLine(QPainter& p, CGarminPolygon& l, const CGarminTyp::polyline_property& property, const QFontMetricsF& metrics, const QFont& font, const QPointF& scale);
    void drawLine(QPainter& p, const CGarminPolygon& l);

    void collectText(const CGarminPolygon& item, const QPolygonF& line, const QFont& font, const QFontMetricsF& metrics, qint32 lineWidth);

    void getInfoPoints(const QPoint& pt, QMultiMap<QString, QString>& dict);
    void getInfoPois(const QPoint& pt, QMultiMap<QString, QString>& dict);
    void getInfoPolylines(const QPoint& pt, QMultiMap<QString, QString>& dict);
    void getInfoPolygons(const QPoint& pt, QMultiMap<QString, QString>& dict);

#pragma pack(1)
    // Garmin IMG file header structure, to the start of the FAT blocks
    struct hdr_img_t
    {
        quint8 xorByte;          ///< 0x00000000
        quint8 byte0x00000001_0x0000000F[15];
        char signature[7];       ///< 0x00000010 .. 0x00000016
        quint8 byte0x00000017_0x00000040[42];
        ///< 0x00000041 .. 0x00000047
        char identifier[7];
        quint8 byte0x00000048;
        char desc1[20];          ///< 0x00000049 .. 0x0000005C
        quint8 byte0x0000005D_0x00000060[4];
        quint8 e1;               ///< 0x00000061
        quint8 e2;               ///< 0x00000062
        quint8 byte0x00000063_0x00000064[2];
        char desc2[31];          ///< 0x00000065 .. 0x00000083
        quint8 byte0x00000084_0x0000040B[904];
        quint32 dataoffset;      ///< 0x0000040C .. 0x0000040F
        quint8 byte0x00000410_0x0000041F[16];
        quint16 blocks[240];     ///< 0x00000420 .. 0x000005FF

        quint32 blocksize()
        {
            return 1 << (e1 + e2);
        }
    };
    struct FATblock_t
    {
        quint8 flag;             ///< 0x00000000
        char name[8];            ///< 0x00000001 .. 0x00000008
        char type[3];            ///< 0x00000009 .. 0x0000000B
        quint32 size;            ///< 0x0000000C .. 0x0000000F
        quint16 part;            ///< 0x00000010 .. 0x00000011
        quint8 byte0x00000012_0x0000001F[14];
        quint16 blocks[240];     ///< 0x00000020 .. 0x000001FF
    };

    // common header of the RGN, TRE, LBL, NET, ... parts of the IMG file
    struct hdr_subfile_part_t
    {
        quint16 length;          ///< 0x00000000 .. 0x00000001
        char type[10];           ///< 0x00000002 .. 0x0000000B
        quint8 byte0x0000000C;
        quint8 flag;             ///< 0x0000000D
        quint8 byte0x0000000E_0x00000014[7];
    };

    // TRE part header, to 0xB7
    struct hdr_tre_t : public hdr_subfile_part_t
    {
        quint24 northbound;      ///< 0x00000015 .. 0x00000017
        quint24 eastbound;       ///< 0x00000018 .. 0x0000001A
        quint24 southbound;      ///< 0x0000001B .. 0x0000001D
        quint24 westbound;       ///< 0x0000001E .. 0x00000020
        quint32 tre1_offset;     ///< 0x00000021 .. 0x00000024
        quint32 tre1_size;       ///< 0x00000025 .. 0x00000028
        quint32 tre2_offset;     ///< 0x00000029 .. 0x0000002C
        quint32 tre2_size;       ///< 0x0000002D .. 0x00000030
        quint32 tre3_offset;     ///< 0x00000031 .. 0x00000034
        quint32 tre3_size;       ///< 0x00000035 .. 0x00000038
                                 ///< 0x00000039 .. 0x0000003A
        quint16 tre3_rec_size;
        quint8 byte0x0000003B_0x0000003E[4];
        quint8 POI_flags;        ///< 0x0000003F
        quint8 byte0x00000040_0x00000049[10];
        quint32 tre4_offset;     ///< 0x0000004A .. 0x0000004D
        quint32 tre4_size;       ///< 0x0000004E .. 0x00000051
                                 ///< 0x00000052 .. 0x00000053
        quint16 tre4_rec_size;
        quint8 byte0x00000054_0x00000057[4];
        quint32 tre5_offset;     ///< 0x00000058 .. 0x0000005B
        quint32 tre5_size;       ///< 0x0000005C .. 0x0000005F
                                 ///< 0x00000060 .. 0x00000061
        quint16 tre5_rec_size;
        quint8 byte0x00000062_0x00000065[4];
        quint32 tre6_offset;     ///< 0x00000066 .. 0x00000069
        quint32 tre6_size;       ///< 0x0000006A .. 0x0000006D
                                 ///< 0x0000006E .. 0x0000006F
        quint16 tre6_rec_size;
        quint8 byte0x00000070_0x00000073[4];
        /*-----------------------------------------------------*/
        quint8 byte0x00000074_0x0000007B[8];
        // Object groups V2 (CTreGroup2).
        quint32 tre7_offset;     ///< 0x0000007C .. 0x0000007F //Groups2Offset
        quint32 tre7_size;       ///< 0x00000080 .. 0x00000083  //dwGroups2Length
                                 ///< 0x00000084 .. 0x00000085 //wGroup2RecSize
        quint16 tre7_rec_size;
        quint8 byte0x00000086_0x00000089[4];
        // Order: polyline, polygon, POI; each sorted by type (1 type 1 levels 1 subtype)
        quint32 tre8_offset;     ///< 0x0000008A .. 0x0000008D
        quint32 tre8_size;       ///< 0x0000008E .. 0x00000091
                                 ///< 0x00000092 .. 0x00000093
        quint16 tre8_rec_size;

        ///< 0x00000094 .. 0x00000095
        quint16 polyl2_types_num;
        ///< 0x00000096 .. 0x00000097
        quint16 polyg2_types_num;
        ///< 0x00000098 .. 0x00000099
        quint16 poi2_types_num;

        /*-----------------------------------------------------*/
        quint8 key[20];          ///< 0x0000009A .. 0x000000AD
        quint32 tre9_offset;     ///< 0x000000AE .. 0x000000B1
        quint32 tre9_size;       ///< 0x000000B2 .. 0x000000B5
                                 ///< 0x000000B6 .. 0x000000B7
        quint16 tre9_rec_size;
    };

    // RGN part header
    struct hdr_rgn_t : public hdr_subfile_part_t
    {
        quint32 offset;          ///< 0x00000015 .. 0x00000018
        quint32 length;          ///< 0x00000019 .. 0x0000001C
                                 ///< 0x0000001D .. 0x00000020
        quint32 offset_polyg2;
        ///< 0x00000021 .. 0x00000024
        quint32 length_polyg2;
        quint8 byte0x00000025_0x00000038[20];
        ///< 0x00000039 .. 0x0000003C
        quint32 offset_polyl2;
        ///< 0x0000003D .. 0x00000040
        quint32 length_polyl2;
        quint8 byte0x00000041_0x00000054[20];
        ///< 0x00000055 .. 0x00000058
        quint32 offset_point2;
        ///< 0x00000059 .. 0x0000005C
        quint32 length_point2;
    };

    // LBL part header
    struct hdr_lbl_t : public hdr_subfile_part_t
    {
        quint32 lbl1_offset;     ///< 0x00000015 .. 0x00000018
        quint32 lbl1_length;     ///< 0x00000019 .. 0x0000001C
        quint8 addr_shift;       ///< 0x0000001D
        quint8 coding;           ///< 0x0000001E
        quint32 lbl2_offset;     ///< 0x0000001F .. 0x00000022
        quint32 lbl2_length;     ///< 0x00000023 .. 0x00000026
                                 ///< 0x00000027 .. 0x00000028
        quint16 lbl2_rec_size;
        quint8 byte0x00000029_0x0000002C[4];
        quint32 lbl3_offset;     ///< 0x0000002D .. 0x00000030
        quint32 lbl3_length;     ///< 0x00000031 .. 0x00000034
                                 ///< 0x00000035 .. 0x00000036
        quint16 lbl3_rec_size;
        quint8 byte0x00000037_0x0000003A[4];
        quint32 lbl4_offset;     ///< 0x0000003B .. 0x0000003E
        quint32 lbl4_length;     ///< 0x0000003F .. 0x00000042
                                 ///< 0x00000043 .. 0x00000044
        quint16 lbl4_rec_size;
        quint8 byte0x00000045_0x00000048[4];
        quint32 lbl5_offset;     ///< 0x00000049 .. 0x0000004C
        quint32 lbl5_length;     ///< 0x0000004D .. 0x00000050
                                 ///< 0x00000051 .. 0x00000052
        quint16 lbl5_rec_size;
        quint8 byte0x00000053_0x00000056[4];
        quint32 lbl6_offset;     ///< 0x00000057 .. 0x0000005A
        quint32 lbl6_length;     ///< 0x0000005B .. 0x0000005E
                                 ///< 0x0000005F
        quint8 lbl6_addr_shift;
        ///< 0x00000060
        quint8 lbl6_glob_mask;
        quint8 byte0x00000061_0x00000063[3];
        quint32 lbl7_offset;     ///< 0x00000064 .. 0x00000067
        quint32 lbl7_length;     ///< 0x00000068 .. 0x0000006B
                                 ///< 0x0000006C .. 0x0000006D
        quint16 lbl7_rec_size;
        quint8 byte0x0000006E_0x00000071[4];
        quint32 lbl8_offset;     ///< 0x00000072 .. 0x00000075
        quint32 lbl8_length;     ///< 0x00000076 .. 0x00000079
                                 ///< 0x0000007A .. 0x0000007B
        quint16 lbl8_rec_size;
        quint8 byte0x0000007C_0x0000007F[4];
        quint32 lbl9_offset;     ///< 0x00000080 .. 0x00000083
        quint32 lbl9_length;     ///< 0x00000084 .. 0x00000087
                                 ///< 0x00000088 .. 0x00000089
        quint16 lbl9_rec_size;
        quint8 byte0x0000008A_0x0000008D[4];
        quint32 lbl10_offset;    ///< 0x0000008E .. 0x00000091
        quint32 lbl10_length;    ///< 0x00000092 .. 0x00000095
                                 ///< 0x00000096 .. 0x00000097
        quint16 lbl10_rec_size;
        quint8 byte0x00000098_0x0000009B[4];
        quint32 lbl11_offset;    ///< 0x0000009C .. 0x0000009F
        quint32 lbl11_length;    ///< 0x000000A0 .. 0x000000A3
                                 ///< 0x000000A4 .. 0x000000A5
        quint16 lbl11_rec_size;
        quint8 byte0x000000A6_0x000000A9[4];
        quint16 codepage;        ///< 0x000000AA .. 0x000000AB  optional check length
    };

    // NET part header
    struct hdr_net_t : public hdr_subfile_part_t
    {
        quint32 net1_offset;     ///< 0x00000015 .. 0x00000018
        quint32 net1_length;     ///< 0x00000019 .. 0x0000001C
                                 ///< 0x0000001D
        quint8 net1_addr_shift;
        quint32 net2_offset;     ///< 0x0000001E .. 0x00000021
        quint32 net2_length;     ///< 0x00000022 .. 0x00000025
                                 ///< 0x00000026
        quint8 net2_addr_shift;
        quint32 net3_offset;     ///< 0x00000027 .. 0x0000002A
        quint32 net3_length;     ///< 0x0000002B .. 0x0000002E
    };

    struct hdr_dem_t : public hdr_subfile_part_t
    {
        quint32 dem_flags;       ///< 0x00000015 .. 0x00000018
        quint16 levels;          ///< 0x00000019 .. 0x0000001A
        quint8 byte0x0000001B_0x0000001E[4];
        quint16 blk3_size;       ///< 0x0000001f .. 0x00000020
        quint32 blk3_offset;     ///< 0x00000021 .. 0x00000024
    };

    struct dem_level_t
    {
        quint16 index;
        quint32 nPixelPerTileX;
        quint32 nPixelPerTileY;
        quint32 Unknown1;
        quint32 Unknown2;
        quint16 Unknown3;
        quint32 nTilesX;
        quint32 nTilesY;
        quint16 format;
        quint16 blk1_size;
        quint32 blk1_offset;
        quint32 blk2_offset;
        quint32 westernBound;
        quint32 northernBound;
        quint32 PixelPerMeterX;
        quint32 PixelPerMeterY;
        quint16 minHeight;
        quint16 maxHeight;
    };

#define TRE_MAP_LEVEL(r) ((r)->zoom & 0x0f)
#define TRE_MAP_INHER(r) (((r)->zoom & 0x80) != 0)

    // map level definition
    struct tre_map_level_t
    {
        quint8 zoom;
        quint8 bits;
        quint16 nsubdiv;
    };

    // map subdivision definition, without pointer to the lower level subparts
    struct tre_subdiv_t
    {
        quint24 rgn_offset;
        quint8 elements;
        quint24 center_lng;
        quint24 center_lat;
        quint16 width_trm;
#define TRE_SUBDIV_WIDTH(r)    (gar_load(uint16_t, (r)->width_trm) & 0x7FFF)
#define TRE_SUBDIV_TERM(r)     ((gar_load(uint16_t, (r)->width_trm) & 0x8000) != 0)
        quint16 height;
    };

    // pointer to the lower level subparts
    struct tre_subdiv_next_t : public tre_subdiv_t
    {
        quint16 next;
    };

    struct tre_subdiv2_t
    {
        quint32 offsetPolygons;
        quint32 offsetPolyline;
        quint32 offsetPoints;
        quint8 btObjects;
    };

#ifdef WIN32
#pragma pack()
#else
#pragma pack(0)
#endif
    struct map_level_t
    {
        quint8 bits;
        quint8 level;
        bool useBaseMap;

        bool operator==(const map_level_t &ml)  const
        {
            if (ml.bits != bits || ml.level != level || ml.useBaseMap != useBaseMap)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        static bool GreaterThan(const map_level_t &ml1, const map_level_t &ml2)
        {
            return ml1.bits < ml2.bits;
        }
    };

    QString filename;
    quint8 mask;
    quint32 mask32;
    quint64 mask64;
    QString mapdesc;
    /// hold all subfile descriptors
    /**
        In a normal *.img file there is only one subfile. However
        gmapsupp.img files can hold several subfiles each with it's
        own subfile parts.
     */
    QMap<QString,subfile_desc_t> subfiles;
    /// relay the transparent flags from the subfiles
    bool transparent;

    QRectF maparea;

    QFontMetrics fm;

    /// combined maplevels of all tiles
    QVector<map_level_t> maplevels;

    QMap<quint32, CGarminTyp::polyline_property> polylineProperties;
    QMap<quint32, CGarminTyp::polygon_property> polygonProperties;
    QList<quint32> polygonDrawOrder;
    QMap<quint32, CGarminTyp::point_property> pointProperties;
    QMap<quint8, QString> languages;

    polytype_t polygons;
    polytype_t polylines;
    pointtype_t points;
    pointtype_t pois;

    QVector<strlbl_t> labels;

    struct textpath_t
    {
        //            QPainterPath    path;
        QPolygonF polyline;
        QString text;
        QFont font;
        QVector<qreal>  lengths;
        qint32 lineWidth;
    };

    QVector<textpath_t> textpaths;
    qint8 selectedLanguage;
    QSet<QString> copyrights;
};

#endif //CMAPIMG_H

