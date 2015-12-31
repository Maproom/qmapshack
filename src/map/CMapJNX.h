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

#ifndef CMAPJNX_H
#define CMAPJNX_H

#include "map/IMap.h"

class CMapDraw;

class CMapJNX : public IMap
{
public:
    CMapJNX(const QString& filename, CMapDraw *parent);

    virtual void draw(IDrawContext::buffer_t& buf) override;

private:
    QString filename;

#pragma pack(1)
    struct hdr_t
    {
        quint32 version;     // byte 00000000..00000003
        quint32 devid;       // byte 00000004..00000007
        qint32  lat1;        // byte 00000010..00000013
        qint32  lon2;        // byte 00000014..00000017
        qint32  lat2;        // byte 00000008..0000000B
        qint32  lon1;        // byte 0000000C..0000000F
        quint32 details;     // byte 00000018..0000001B
        quint32 expire;      // byte 0000001C..0000001F
        qint32  productId;   // byte 00000020..00000023
        quint32 crc;         // byte 00000024..00000027
        quint32 signature;   // byte 00000028..0000002B
                             // byte 0000002C..0000002F
        quint32 signature_offset;
        qint32 zorder;       // byte 00000030..00000033
    };

#ifdef WIN32
#pragma pack()
#else
#pragma pack(0)
#endif
    struct tile_t
    {
        QRectF area;
        quint16 width;
        quint16 height;
        quint32 size;
        quint32 offset;
    };

    struct level_t
    {
        quint32 nTiles;
        quint32 offset;
        quint32 scale;
        QString copyright1;

        quint32 level;
        QString name1;
        QString name2;
        QString copyright2;

        QVector<tile_t> tiles;
    };


    struct file_t
    {
        qreal lon1;
        qreal lat1;
        qreal lon2;
        qreal lat2;
        QRectF bbox;

        QString filename;
        QVector<level_t> levels;
    };

    void readFile(const QString& fn, qint32& productId);
    qint32 scale2level(qreal s, const file_t& file);

    QList<file_t> files;

    qreal lon1 = 180.0;
    qreal lat1 = -90;
    qreal lon2 = -180;
    qreal lat2 = 90;
};

#endif // CMAPJNX_H
