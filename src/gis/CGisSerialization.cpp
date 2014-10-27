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

#include "gis/CGisSerialization.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/ovl/CGisItemOvlArea.h"

#include <QtWidgets>

#define VER_TRK     quint8(1)
#define VER_WPT     quint8(1)
#define VER_RTE     quint8(1)
#define VER_AREA    quint8(1)
#define VER_LINK    quint8(1)
#define VER_TRKSEG  quint8(1)
#define VER_TRKPT   quint8(1)
#define VER_WPT_T   quint8(1)
#define VER_GC_T    quint8(1)
#define VER_GCLOG_T quint8(1)

#define MAGIC_SIZE  10
#define MAGIC_TRK   "QMTrk     "
#define MAGIC_WPT   "QMWpt     "
#define MAGIC_RTE   "QMRte     "
#define MAGIC_AREA  "QMArea    "


QDataStream& operator<<(QDataStream& stream, const IGisItem::link_t& link)
{
    stream << VER_LINK << link.uri << link.text << link.type;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, IGisItem::link_t& link)
{
    quint8 version;
    stream >> version >> link.uri >> link.text >> link.type;
    return stream;
}

QDataStream& operator<<(QDataStream& stream, const IGisItem::wpt_t& wpt)
{
    stream << VER_WPT_T;
    stream << wpt.lat;
    stream << wpt.lon;
    stream << wpt.ele;
    stream << wpt.time;
    stream << wpt.magvar;
    stream << wpt.geoidheight;
    stream << wpt.name;
    stream << wpt.cmt;
    stream << wpt.desc;
    stream << wpt.src;
    stream << wpt.links;
    stream << wpt.sym;
    stream << wpt.type;
    stream << wpt.fix;
    stream << wpt.sat;
    stream << wpt.hdop;
    stream << wpt.vdop;
    stream << wpt.pdop;
    stream << wpt.ageofdgpsdata;
    stream << wpt.dgpsid;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, IGisItem::wpt_t& wpt)
{
    quint8 version;
    stream >> version;
    stream >> wpt.lat;
    stream >> wpt.lon;
    stream >> wpt.ele;
    stream >> wpt.time;
    stream >> wpt.magvar;
    stream >> wpt.geoidheight;
    stream >> wpt.name;
    stream >> wpt.cmt;
    stream >> wpt.desc;
    stream >> wpt.src;
    stream >> wpt.links;
    stream >> wpt.sym;
    stream >> wpt.type;
    stream >> wpt.fix;
    stream >> wpt.sat;
    stream >> wpt.hdop;
    stream >> wpt.vdop;
    stream >> wpt.pdop;
    stream >> wpt.ageofdgpsdata;
    stream >> wpt.dgpsid;

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const CGisItemWpt::geocachelog_t& log)
{
    stream << VER_GCLOG_T;
    stream << log.id;
    stream << log.date;
    stream << log.type;
    stream << log.finderId;
    stream << log.finder;
    stream << quint8(log.textIsHtml);
    stream << log.text;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, CGisItemWpt::geocachelog_t& log)
{
    quint8 version, tmp8;

    stream >> version;
    stream >> log.id;
    stream >> log.date;
    stream >> log.type;
    stream >> log.finderId;
    stream >> log.finder;
    stream >> tmp8;
    log.textIsHtml = tmp8;
    stream >> log.text;

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const CGisItemWpt::geocache_t& geocache)
{
    stream << VER_GC_T;
    stream << quint8(geocache.hasData);
    if(geocache.hasData)
    {
        stream << geocache.id;
        stream << quint8(geocache.available);
        stream << quint8(geocache.archived);
        stream << geocache.difficulty;
        stream << geocache.terrain;
        stream << geocache.status;
        stream << geocache.name;
        stream << geocache.owner;
        stream << geocache.ownerId;
        stream << geocache.type;
        stream << geocache.container;
        stream << quint8(geocache.shortDescIsHtml);
        stream << geocache.shortDesc;
        stream << quint8(geocache.longDescIsHtml);
        stream << geocache.longDesc;
        stream << geocache.hint;
        stream << geocache.country;
        stream << geocache.state;
        stream << geocache.locale;
        stream << geocache.logs;
    }
    return stream;
}

QDataStream& operator>>(QDataStream& stream, CGisItemWpt::geocache_t& geocache)
{
    quint8 version, tmp8;

    stream >> version;
    stream >> tmp8;
    geocache.hasData = tmp8;
    if(geocache.hasData)
    {
        stream >> geocache.id;
        stream >> tmp8;
        geocache.available = tmp8;
        stream >> tmp8;
        geocache.archived = tmp8;
        stream >> geocache.difficulty;
        stream >> geocache.terrain;
        stream >> geocache.status;
        stream >> geocache.name;
        stream >> geocache.owner;
        stream >> geocache.ownerId;
        stream >> geocache.type;
        stream >> geocache.container;
        stream >> tmp8;
        geocache.shortDescIsHtml = tmp8;
        stream >> geocache.shortDesc;
        stream >> tmp8;
        geocache.longDescIsHtml = tmp8;
        stream >> geocache.longDesc;
        stream >> geocache.hint;
        stream >> geocache.country;
        stream >> geocache.state;
        stream >> geocache.locale;
        stream >> geocache.logs;
    }
    return stream;
}

QDataStream& operator<<(QDataStream& stream, const CGisItemTrk::trkseg_t& seg)
{
    stream << VER_TRKSEG << seg.pts;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, CGisItemTrk::trkseg_t& seg)
{
    quint8 version;
    stream >> version >> seg.pts;
    return stream;
}

QDataStream& operator<<(QDataStream& stream, const CGisItemTrk::trkpt_t& pt)
{
    stream << VER_TRKPT << pt.flags << pt.shdwLon << pt.shdwLat << pt.shdwEle << pt.shdwTime;
    stream << (const IGisItem::wpt_t&)pt;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, CGisItemTrk::trkpt_t& pt)
{
    quint8 version;
    stream >> version >> pt.flags >> pt.shdwLon >> pt.shdwLat >> pt.shdwEle >> pt.shdwTime;
    stream >> (IGisItem::wpt_t&)pt;
    return stream;
}

// ---------------- main objects ---------------------------------

QDataStream& CGisItemTrk::operator>>(QDataStream& stream)
{
    QByteArray  buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_5_2);

    out << key;
    out << flags;
    out << trk.name;
    out << trk.cmt;
    out << trk.desc;
    out << trk.src;
    out << trk.links;
    out << trk.number;
    out << trk.type;
    out << trk.color;
    out << trk.segs;

    stream.writeRawData(MAGIC_TRK, MAGIC_SIZE);
    stream << VER_TRK;
    stream << qCompress(buffer,9);
    return stream;
}

QDataStream& CGisItemTrk::operator<<(QDataStream& stream)
{
    quint8      version;
    QByteArray  buffer;
    QIODevice * dev = stream.device();
    qint64      pos = dev->pos();

    char magic[10];
    stream.readRawData(magic,MAGIC_SIZE);

    if(strncmp(magic,MAGIC_TRK,MAGIC_SIZE))
    {
        dev->seek(pos);
        return stream;
    }

    stream >> version;
    stream >> buffer;
    buffer = qUncompress(buffer);

    QDataStream in(&buffer, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_5_2);

    in >> key;
    in >> flags;
    in >> trk.name;
    in >> trk.cmt;
    in >> trk.desc;
    in >> trk.src;
    in >> trk.links;
    in >> trk.number;
    in >> trk.type;
    in >> trk.color;

    trk.segs.clear();
    in >> trk.segs;

    deriveSecondaryData();
    setColor(trk.color);
    setText(0, trk.name);
    setToolTip(0, getInfo());

    return stream;
}

QDataStream& CGisItemWpt::operator<<(QDataStream& stream)
{
    quint8      version;
    QByteArray  buffer;
    QIODevice * dev = stream.device();
    qint64      pos = dev->pos();

    char magic[10];
    stream.readRawData(magic,MAGIC_SIZE);

    if(strncmp(magic,MAGIC_WPT,MAGIC_SIZE))
    {
        dev->seek(pos);
        return stream;
    }

    stream >> version;
    stream >> buffer;
    buffer = qUncompress(buffer);

    QDataStream in(&buffer, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_5_2);

    in >> key;
    in >> flags;
    in >> proximity;
    in >> wpt;
    in >> geocache;

    setIcon();
    setText(0, wpt.name);
    setToolTip(0, getInfo());

    return stream;
}

QDataStream& CGisItemWpt::operator>>(QDataStream& stream)
{
    QByteArray  buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_5_2);

    out << key;
    out << flags;
    out << proximity;
    out << wpt;
    out << geocache;

    stream.writeRawData(MAGIC_WPT, MAGIC_SIZE);
    stream << VER_WPT;
    stream << qCompress(buffer,9);

    return stream;
}

QDataStream& CGisItemRte::operator<<(QDataStream& stream)
{
    return stream;
}

QDataStream& CGisItemRte::operator>>(QDataStream& stream)
{

    return stream;
}

QDataStream& CGisItemOvlArea::operator<<(QDataStream& stream)
{    
    quint8      version, tmp8;
    QByteArray  buffer;
    QIODevice * dev = stream.device();
    qint64      pos = dev->pos();

    char magic[10];
    stream.readRawData(magic,MAGIC_SIZE);

    if(strncmp(magic,MAGIC_AREA,MAGIC_SIZE))
    {
        dev->seek(pos);
        return stream;
    }

    stream >> version;
    stream >> buffer;
    buffer = qUncompress(buffer);

    QDataStream in(&buffer, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_5_2);

    in >> key;
    in >> flags;
    in >> area.name;
    in >> area.cmt;
    in >> area.desc;
    in >> area.src;
    in >> area.links;
    in >> area.number;
    in >> area.type;
    in >> area.pts;
    in >> area.color;
    in >> area.width;
    in >> area.style;
    in >> tmp8;
    area.opacity = tmp8;

    setColor(str2color(area.color));
    setText(0, area.name);
    setToolTip(0, getInfo());

    return stream;
}

QDataStream& CGisItemOvlArea::operator>>(QDataStream& stream)
{
    QByteArray  buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_5_2);

    out << key;
    out << flags;
    out << area.name;
    out << area.cmt;
    out << area.desc;
    out << area.src;
    out << area.links;
    out << area.number;
    out << area.type;
    out << area.pts;
    out << area.color;
    out << area.width;
    out << area.style;
    out << quint8(area.opacity);

    stream.writeRawData(MAGIC_AREA, MAGIC_SIZE);
    stream << VER_AREA;
    stream << qCompress(buffer,9);

    return stream;
}


CGisSerialization::CGisSerialization()
{

}

CGisSerialization::~CGisSerialization()
{

}

