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

#include "map/CMapJNX.h"
#include "map/CMap.h"
#include "GeoMath.h"
#include "CCanvas.h"


#include <QtGui>

static void readCString(QDataStream& stream, QByteArray& ba)
{
    quint8 byte;

    ba.clear();

    stream >> byte;
    while(byte != 0)
    {
        ba += byte;
        stream >> byte;
    }

}

static quint32 scale2jnx(qreal scale)
{
    /*
    Ok, I've made some calculations, and got the following formula to
    calculate the JNX scale (S) depending on the map's meters/pixel
    ratio (R):

      S(R) =
        qRound(
          76437 *
          exp(
            ln(2.000032708011) *
            qRound(
              ln(R * 130.2084 / 76437) /
              ln(2.000032708011)
            )
          )
        )


    where
      qRound - is a function which returns the closest integer from
        floating point value, [unfortunately its defined in C99 but not standard C++]
      exp - exponent,
      ln - natural logarithm.

    Magic number 130.2084 - is an average value for
      (JNX scale) / (maps meters per pixel)
    ratio among all zoom levels in metric system.

    Magic number 2.000032708011 is a ratio on which our standard scale
    table is built. It is (76437 / 4777) ^ (1/4).
    */

    return (uint32_t)floor(0.5 + 76437 * exp(log(2.000032708011) * floor(0.5 + log(scale * 10 * 130.2084 / 76437) / log(2.000032708011) ) ) );
}



CMapJNX::CMapJNX(const QString &filename, CMap *parent)
    : IMap(parent)   
    , filename(filename)
    , lon1(180.0)
    , lat1(-90)
    , lon2(-180)
    , lat2(90)
{
    qDebug() << "------------------------------";
    qDebug() << "JNX: try to open" << filename;

    qint32 productId = -1;
    readFile(filename, productId);

    pjsrc = pj_init_plus("+proj=merc +ellps=WGS84 +datum=WGS84 +units=m +no_defs +towgs84=0,0,0");

    isActivated = true;
}


void CMapJNX::readFile(const QString& fn, qint32& productId)
{

    hdr_t hdr;

    qDebug() << fn;

    QFile file(fn);
    file.open(QIODevice::ReadOnly);

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> hdr.version;       // byte 00000000..00000003
    stream >> hdr.devid;         // byte 00000004..00000007
    stream >> hdr.lat1;          // byte 00000008..0000000B
    stream >> hdr.lon2;          // byte 0000000C..0000000F
    stream >> hdr.lat2;          // byte 00000010..00000013
    stream >> hdr.lon1;          // byte 00000014..00000017
    stream >> hdr.details;       // byte 00000018..0000001B
    stream >> hdr.expire;        // byte 0000001C..0000001F
    stream >> hdr.productId;     // byte 00000020..00000023
    stream >> hdr.crc;           // byte 00000024..00000027
    stream >> hdr.signature;     // byte 00000028..0000002B
                                 // byte 0000002C..0000002F
    stream >> hdr.signature_offset;

    if(hdr.version > 3)
    {
        stream >> hdr.zorder;
    }
    else
    {
        hdr.zorder = -1;
    }

    if(productId != -1 && hdr.productId != productId)
    {
        return;
    }

    productId = hdr.productId;

    files.append(file_t());
    file_t& mapFile = files.last();

    mapFile.filename = fn;

    mapFile.lat1 = hdr.lat1 * 180.0 / 0x7FFFFFFF;
    mapFile.lat2 = hdr.lat2 * 180.0 / 0x7FFFFFFF;
    mapFile.lon1 = hdr.lon1 * 180.0 / 0x7FFFFFFF;
    mapFile.lon2 = hdr.lon2 * 180.0 / 0x7FFFFFFF;
    mapFile.bbox = QRectF(QPointF(mapFile.lon1, mapFile.lat1), QPointF(mapFile.lon2, mapFile.lat2));


    qDebug() << hex << "Version:" << hdr.version << "DevId" <<  hdr.devid;
    qDebug() << mapFile.lon1 << mapFile.lat1 << mapFile.lon2 << mapFile.lat2;
    qDebug() << hex <<  hdr.lon1 <<  hdr.lat1 <<  hdr.lon2 <<  hdr.lat2;
    qDebug() << hex << "Details:" <<  hdr.details << "Expire:" <<  hdr.expire << "CRC:" <<  hdr.crc ;
    qDebug() << hex << "Signature:" <<  hdr.signature << "Offset:" <<  hdr.signature_offset;

    QString strTopLeft, strBottomRight;
    GPS_Math_Deg_To_Str(mapFile.lon1, mapFile.lat1, strTopLeft);
    GPS_Math_Deg_To_Str(mapFile.lon2, mapFile.lat2, strBottomRight);

    qDebug() << "Levels:";
    mapFile.levels.resize(hdr.details);
    for(quint32 i = 0; i < hdr.details; i++)
    {
        level_t& level = mapFile.levels[i];
        stream >> level.nTiles >> level.offset >> level.scale;

        if(hdr.version > 3)
        {
            quint32 dummy;
            QTextCodec * codec = QTextCodec::codecForName("utf-8");
            QByteArray ba;

            stream >> dummy;
            readCString(stream, ba);
            level.copyright1 = codec->toUnicode(ba);

        }
        qDebug() << i << hex << level.nTiles << level.offset << level.scale;
    }

    quint32 infoBlockVersion;
    stream >> infoBlockVersion;
    if(infoBlockVersion == 0x9)
    {
        QTextCodec * codec = QTextCodec::codecForName("utf-8");
        QByteArray ba;
        quint8 dummy;
        QString groupId;
        QString groupName;
        QString groupTitle;

        readCString(stream, ba);
        groupId = codec->toUnicode(ba);
        readCString(stream, ba);
        groupName = codec->toUnicode(ba);

        stream >> dummy >> dummy >> dummy;
        readCString(stream, ba);
        groupTitle = codec->toUnicode(ba);
        qDebug() << groupId << groupName << groupTitle;

        for(quint32 i = 0; i < hdr.details; i++)
        {
            level_t& level = mapFile.levels[i];

            stream >> level.level;
            readCString(stream, ba);
            level.name1 = codec->toUnicode(ba);
            readCString(stream, ba);
            level.name2 = codec->toUnicode(ba);
            readCString(stream, ba);
            level.copyright2 = codec->toUnicode(ba);
        }
    }

    for(quint32 i = 0; i < hdr.details; i++)
    {

        level_t& level = mapFile.levels[i];
        const quint32 M = level.nTiles;
        file.seek(level.offset);

        level.tiles.resize(M);

        for(quint32 m = 0; m < M; m++)
        {

            qint32 top, right, bottom, left;
            tile_t& tile = level.tiles[m];

            stream >> top >> right >> bottom >> left;
            stream >> tile.width >> tile.height >> tile.size >> tile.offset;

            tile.area.setTop(top * 180.0 / 0x7FFFFFFF);
            tile.area.setRight(right * 180.0 / 0x7FFFFFFF);
            tile.area.setBottom(bottom * 180.0 / 0x7FFFFFFF);
            tile.area.setLeft(left * 180.0 / 0x7FFFFFFF);
        }
    }

    if(mapFile.lon1 < lon1) lon1  = mapFile.lon1;
    if(mapFile.lat1 > lat1) lat1  = mapFile.lat1;
    if(mapFile.lon2 > lon2) lon2  = mapFile.lon2;
    if(mapFile.lat2 < lat2) lat2  = mapFile.lat2;

}

qint32 CMapJNX::scale2level(qreal s, const file_t& file)
{
    qint32 idxLvl    = -1;
    quint32 actScale = scale2jnx(s);

    for(int i = 0; i < file.levels.size(); i++)
    {
        const level_t& level = file.levels[i];

        if(actScale <= level.scale)
        {
            idxLvl = i;
        }
    }

    return idxLvl;
}


void CMapJNX::draw(buffer_t& buf)
{

    if(map->needsRedraw())
    {
        return;
    }


    // convert top left buffer corner
    // into buffer's coordinate system
    QPointF pp = buf.ref1;
    pj_transform(pjtar,buf.pjsrc,1,0,&pp.rx(),&pp.ry(),0);

    QPointF bufferScale = buf.scale * buf.zoomFactor;

    double u1 = buf.ref1.x() * RAD_TO_DEG;
    double v1 = buf.ref1.y() * RAD_TO_DEG;
    double u2 = buf.ref3.x() * RAD_TO_DEG;
    double v2 = buf.ref3.y() * RAD_TO_DEG;

    QRectF viewport;
    viewport.setTop(v2);
    viewport.setRight(u2);
    viewport.setBottom(v1);
    viewport.setLeft(u1);

    // ----- start drawing -----
    QPainter p(&buf.image);
    USE_ANTI_ALIASING(p,true);

    foreach(const file_t& mapFile, files)
    {
        if(!viewport.intersects(mapFile.bbox))
        {
            continue;
        }

        if(map->needsRedraw())
        {
            break;
        }


        qint32 level = scale2level(bufferScale.x()/5, mapFile);

        if(level < 0)
        {
            // no scalable level found, draw bounding box of map
            // derive maps corner coordinate
            QPointF pt1;
            pt1.rx() = mapFile.lon1 * DEG_TO_RAD;
            pt1.ry() = mapFile.lat1 * DEG_TO_RAD;
            QPointF pt2;
            pt2.rx() = mapFile.lon2 * DEG_TO_RAD;
            pt2.ry() = mapFile.lat1 * DEG_TO_RAD;
            QPointF pt3;
            pt3.rx() = mapFile.lon2 * DEG_TO_RAD;
            pt3.ry() = mapFile.lat2 * DEG_TO_RAD;
            QPointF pt4;
            pt4.rx() = mapFile.lon1 * DEG_TO_RAD;
            pt4.ry() = mapFile.lat2 * DEG_TO_RAD;

            // transform the tile's corner coordinate from map's projection into buffer's projecton
            pj_transform(pjtar,buf.pjsrc,1,0,&pt1.rx(),&pt1.ry(),0);
            pj_transform(pjtar,buf.pjsrc,1,0,&pt2.rx(),&pt2.ry(),0);
            pj_transform(pjtar,buf.pjsrc,1,0,&pt3.rx(),&pt3.ry(),0);
            pj_transform(pjtar,buf.pjsrc,1,0,&pt4.rx(),&pt4.ry(),0);

            // adjust the tiles width and height to fit the buffer's scale
            qreal dx1   = pt1.x() - pt2.x();
            qreal dy1   = pt1.y() - pt2.y();
            qreal dx2   = pt1.x() - pt4.x();
            qreal dy2   = pt1.y() - pt4.y();
            qreal w    = ceil( sqrt(dx1*dx1 + dy1*dy1) / bufferScale.x());
            qreal h    = ceil(-sqrt(dx2*dx2 + dy2*dy2) / bufferScale.y());

            // calculate offset into buffer
            pt1 = (pt1 - pp) / (bufferScale);
            // calculate rotation. This is not really a reprojection but might be good enough for close zoom levels
            qreal a = atan(dy1/dx1) * RAD_TO_DEG;

            // finally scale, rotate and draw tile
            p.translate(pt1);
            p.rotate(-a);
            p.setPen(QPen(Qt::darkBlue,2));
            p.setBrush(QBrush(QColor(230,230,255,100) ));
            p.drawRect(0,0,w,h);
            p.resetTransform();
            continue;
        }

        QByteArray data(1024*1024*4,0);
        //(char) typecast needed to avoid MSVC compiler warning
        //in MSVC, char is a signed type.
        //Maybe the QByteArray declaration should be fixed ;-)
        data[0] = (char) 0xFF;
        data[1] = (char) 0xD8;
        char * pData = data.data() + 2;

        QFile file(mapFile.filename);
        file.open(QIODevice::ReadOnly);

        const QVector<tile_t>& tiles = mapFile.levels[level].tiles;
        const quint32 M = tiles.size();
        for(quint32 m = 0; m < M; m++)
        {

            if(map->needsRedraw())
            {
                break;
            }

            const tile_t& tile = tiles[m];

            if(viewport.intersects(tile.area))
            {
                QImage img;
                file.seek(tile.offset);
                file.read(pData, tile.size);
                img.loadFromData(data);

                qreal imgw = img.width();
                qreal imgh = img.height();

                QPointF pt1;
                pt1.rx() = tile.area.left()     * DEG_TO_RAD;
                pt1.ry() = tile.area.top()      * DEG_TO_RAD;
                QPointF pt2;
                pt2.rx() = tile.area.right()    * DEG_TO_RAD;
                pt2.ry() = tile.area.top()      * DEG_TO_RAD;
                QPointF pt3;
                pt3.rx() = tile.area.right()    * DEG_TO_RAD;
                pt3.ry() = tile.area.bottom()   * DEG_TO_RAD;
                QPointF pt4;
                pt4.rx() = tile.area.left()     * DEG_TO_RAD;
                pt4.ry() = tile.area.bottom()   * DEG_TO_RAD;

                // transform the tile's corner coordinate from map's projection into buffer's projecton
                pj_transform(pjtar,buf.pjsrc,1,0,&pt1.rx(),&pt1.ry(),0);
                pj_transform(pjtar,buf.pjsrc,1,0,&pt2.rx(),&pt2.ry(),0);
                pj_transform(pjtar,buf.pjsrc,1,0,&pt3.rx(),&pt3.ry(),0);
                pj_transform(pjtar,buf.pjsrc,1,0,&pt4.rx(),&pt4.ry(),0);

                // adjust the tiles width and height to fit the buffer's scale
                qreal dx1   = pt1.x() - pt2.x();
                qreal dy1   = pt1.y() - pt2.y();
                qreal dx2   = pt1.x() - pt4.x();
                qreal dy2   = pt1.y() - pt4.y();
                qreal w    = ceil( sqrt(dx1*dx1 + dy1*dy1) / bufferScale.x());
                qreal h    = ceil(-sqrt(dx2*dx2 + dy2*dy2) / bufferScale.y());

                // calculate offset into buffer
                pt1 = (pt1 - pp) / (bufferScale);
                // calculate rotation. This is not really a reprojection but might be good enough for close zoom levels
                qreal a = atan(dy1/dx1) * RAD_TO_DEG;

                // finally scale, rotate and draw tile
                p.translate(pt1);
                p.scale(w/imgw, h/imgh);
                p.rotate(-a);
                p.drawImage(0,0,img);
                p.resetTransform();
                continue;
            }
        }
    }
}
