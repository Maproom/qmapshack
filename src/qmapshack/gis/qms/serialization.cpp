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

#include "gis/CGisListWks.h"
#include "gis/db/CDBProject.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CLimit.h"
#include "helpers/CValue.h"

#include <QtWidgets>

#define VER_TRK         quint8(7)
#define VER_WPT         quint8(4)
#define VER_RTE         quint8(4)
#define VER_AREA        quint8(2)
#define VER_LINK        quint8(1)
#define VER_TRKSEG      quint8(1)
#define VER_TRKPT       quint8(3)
#define VER_RTEPT       quint8(2)
#define VER_RTESUBPT    quint8(2)
#define VER_WPT_T       quint8(1)
#define VER_GC_T        quint8(3)
#define VER_GCLOG_T     quint8(1)
#define VER_IMAGE       quint8(1)
#define VER_PROJECT     quint8(5)
#define VER_COPYRIGHT   quint8(1)
#define VER_PERSON      quint8(1)
#define VER_HIST        quint8(1)
#define VER_HIST_EVT    quint8(3)
#define VER_ITEM        quint8(3)
#define VER_CVALUE      quint8(1)
#define VER_CLIMIT      quint8(1)
#define VER_ENERGYCYCLE quint8(1)

#define MAGIC_SIZE      10
#define MAGIC_TRK       "QMTrk     "
#define MAGIC_WPT       "QMWpt     "
#define MAGIC_RTE       "QMRte     "
#define MAGIC_AREA      "QMArea    "
#define MAGIC_PROJ      "QMProj    "


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

QDataStream& operator<<(QDataStream& stream, const IGisItem::history_event_t& e)
{
    stream << VER_HIST_EVT;
    stream << e.time;
    stream << e.icon;
    stream << e.comment;
    stream << e.data;
    stream << e.hash;
    stream << e.who;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, IGisItem::history_event_t& e)
{
    quint8 version;
    stream >> version;
    stream >> e.time;
    stream >> e.icon;
    stream >> e.comment;
    stream >> e.data;
    if(version > 1)
    {
        stream >> e.hash;
    }
    if(version > 2)
    {
        stream >> e.who;
    }

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const IGisItem::history_t& h)
{
    stream << VER_HIST;
    stream << h.histIdxInitial;
    stream << h.histIdxCurrent;
    stream << h.events;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, IGisItem::history_t& h)
{
    quint8 version;
    stream >> version;
    stream >> h.histIdxInitial;
    stream >> h.histIdxCurrent;
    stream >> h.events;

    if(h.histIdxCurrent >= h.events.size())
    {
        h.histIdxCurrent = h.events.size() - 1;
    }

    if(h.histIdxInitial >= h.events.size())
    {
        h.histIdxInitial = h.events.size() - 1;
    }

    if(h.histIdxCurrent < 0)
    {
        h.histIdxInitial = NOIDX;
        h.histIdxCurrent = NOIDX;
        h.events.clear();
    }
    if(h.histIdxInitial < 0)
    {
        h.histIdxInitial = NOIDX;
        h.histIdxCurrent = NOIDX;
        h.events.clear();
    }

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
        stream << quint8(geocache.needsMaintenance);
        stream << geocache.difficulty;
        stream << geocache.terrain;
        stream << geocache.name;
        stream << geocache.owner;
        stream << geocache.ownerId;
        stream << geocache.type;
        stream << geocache.container;
        stream << geocache.attributes;
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
        if(version > 1)
        {
            stream >> tmp8;
            geocache.needsMaintenance = tmp8;
        }
        stream >> geocache.difficulty;
        stream >> geocache.terrain;
        if(version == 1)
        {
            QString trash;
            stream >> trash; //status string was only used in version 1
        }
        stream >> geocache.name;
        stream >> geocache.owner;
        stream >> geocache.ownerId;
        stream >> geocache.type;
        stream >> geocache.container;
        if(version > 2)
        {
            stream>>geocache.attributes;
        }
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

QDataStream& operator<<(QDataStream& stream, const CGisItemWpt::image_t& image)
{
    QBuffer imgBuf;
    image.pixmap.save(&imgBuf, "JPEG");

    stream << VER_IMAGE;
    stream << imgBuf.data();
    stream << image.direction;
    stream << image.info;
    stream << image.filePath;
    stream << image.fileName;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, CGisItemWpt::image_t& image)
{
    quint8 version;
    QBuffer imgBuf;

    stream >> version;
    stream >> imgBuf.buffer();
    stream >> image.direction;
    stream >> image.info;
    stream >> image.filePath;
    stream >> image.fileName;

    image.pixmap.load(&imgBuf, "JPEG");

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const CTrackData::trkseg_t& seg)
{
    stream << VER_TRKSEG << seg.pts;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, CTrackData::trkseg_t& seg)
{
    quint8 version;
    stream >> version >> seg.pts;
    return stream;
}

QDataStream& operator<<(QDataStream& stream, const CTrackData::trkpt_t& pt)
{
    stream << VER_TRKPT << pt.flags;
    stream << (const IGisItem::wpt_t&)pt;
    stream << pt.extensions;
    stream << qint16(pt.activity);
    return stream;
}

QDataStream& operator>>(QDataStream& stream, CTrackData::trkpt_t& pt)
{
    quint8 version;
    stream >> version >> pt.flags;
    stream >> (IGisItem::wpt_t&)pt;
    if(version > 1)
    {
        stream >> pt.extensions;
    }

    if(version > 2)
    {
        qint16 tmp16;
        stream >> tmp16;
        pt.activity = trkact_t(tmp16);
    }

    pt.sanitizeFlags();

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const CGisItemRte::subpt_t& pt)
{
    stream << VER_RTESUBPT;
    stream << pt.lon;
    stream << pt.lat;
    stream << pt.type;

    stream << pt.turn;
    stream << pt.bearing;
    stream << pt.streets;

    stream << pt.instruction;
    stream << pt.distance;
    stream << pt.time;
    stream << pt.ele;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, CGisItemRte::subpt_t& pt)
{
    quint8 version;

    stream >> version;
    stream >> pt.lon;
    stream >> pt.lat;
    stream >> pt.type;

    stream >> pt.turn;
    stream >> pt.bearing;
    stream >> pt.streets;

    stream >> pt.instruction;
    stream >> pt.distance;
    if(version < 2)
    {
        quint32 time;
        stream >> time;
    }
    else
    {
        stream >> pt.time;
        stream >> pt.ele;
    }

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const CGisItemRte::rtept_t& pt)
{
    stream << VER_RTEPT << pt.focus << pt.icon;
    stream << (const IGisItem::wpt_t&)pt;
    stream << pt.fakeSubpt;
    stream << pt.subpts;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, CGisItemRte::rtept_t& pt)
{
    quint8 version;
    stream >> version >> pt.focus >> pt.icon;
    stream >> (IGisItem::wpt_t&)pt;
    if(version > 1)
    {
        stream >> pt.fakeSubpt;
        stream >> pt.subpts;
    }
    return stream;
}


QDataStream& operator<<(QDataStream& stream, const IGisProject::copyright_t& c)
{
    stream << VER_COPYRIGHT << c.author << c.year << c.license;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, IGisProject::copyright_t& c)
{
    quint8 version;
    stream >> version >> c.author >> c.year >> c.license;
    return stream;
}

QDataStream& operator<<(QDataStream& stream, const IGisProject::person_t& p)
{
    stream << VER_PERSON << p.name << p.id << p.domain << p.link;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, IGisProject::person_t& p)
{
    quint8 version;
    stream >> version >> p.name >> p.id >> p.domain >> p.link;
    return stream;
}

QDataStream& operator<<(QDataStream& stream, const CValue& v)
{
    stream << VER_CVALUE << quint8(v.mode) << v.valUser;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, CValue& v)
{
    quint8 version, mode;
    stream >> version >> mode >> v.valUser;
    // lame trick to update object on mode correctly without
    // triggering a changed mark
    v.mode = CValue::mode_e(mode);
    v.setMode(CValue::mode_e(mode));
    return stream;
}

QDataStream& operator<<(QDataStream& stream, const CLimit& l)
{
    stream << VER_CLIMIT << quint8(l.mode) << l.source << l.minUser << l.maxUser;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, CLimit& l)
{
    quint8 version, mode;

    stream >> version >> mode >> l.source >> l.minUser >> l.maxUser;
    l.mode = CLimit::mode_e(mode);

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const CEnergyCycling::energy_set_t &e)
{
    stream << VER_ENERGYCYCLE << e.driverWeight << e.bikeWeight << e.airDensity
           << e.windSpeedIndex << e.windSpeed << e.windPositionIndex
           << e.frontalArea << e.windDragCoeff << e.groundIndex
           << e.rollingCoeff << e.pedalCadence << e.energyKcal;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, CEnergyCycling::energy_set_t &e)
{
    quint8 version;
    stream >> version >> e.driverWeight >> e.bikeWeight >> e.airDensity
    >> e.windSpeedIndex >> e.windSpeed >> e.windPositionIndex
    >> e.frontalArea >> e.windDragCoeff >> e.groundIndex
    >> e.rollingCoeff >> e.pedalCadence >> e.energyKcal;
    return stream;
}

// ---------------- main objects ---------------------------------

QDataStream& CGisItemTrk::operator>>(QDataStream& stream) const
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_5_2);

    out << key.item;
    out << flags;
    out << trk.name;
    out << trk.cmt;
    out << trk.desc;
    out << trk.src;
    out << trk.links;
    out << trk.number;
    out << trk.type;
    out << trk.color;
    out << rating;
    out << keywords;

    out << colorSourceLimit;
    out << lineScale;
    out << showArrows;

    out << limitsGraph1;
    out << limitsGraph2;
    out << limitsGraph3;

    out << energyCycling.getEnergyTrkSet();

    out << trk.segs;

    stream.writeRawData(MAGIC_TRK, MAGIC_SIZE);
    stream << VER_TRK;
    stream << qCompress(buffer, 9);
    return stream;
}

QDataStream& CGisItemTrk::operator<<(QDataStream& stream)
{
    quint8 version;
    QByteArray buffer;
    QIODevice * dev = stream.device();
    qint64 pos = dev->pos();

    char magic[10];
    stream.readRawData(magic, MAGIC_SIZE);

    if(strncmp(magic, MAGIC_TRK, MAGIC_SIZE))
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

    in >> key.item;
    in >> flags;
    in >> trk.name;
    in >> trk.cmt;
    in >> trk.desc;
    in >> trk.src;
    in >> trk.links;
    in >> trk.number;
    in >> trk.type;
    in >> trk.color;

    if(version > 6)
    {
        in >> rating;
        in >> keywords;
    }

    if(version > 1 && version <= 4)
    {
        QString colorSource;
        in >> colorSource;

        qreal limitLow, limitHigh;
        in >> limitLow;
        in >> limitHigh;

        colorSourceLimit.source     = colorSource;
        colorSourceLimit.mode       = CLimit::eModeAuto;
        colorSourceLimit.minUser    = limitLow;
        colorSourceLimit.maxUser    = limitHigh;
    }
    else if(version > 4)
    {
        in >> colorSourceLimit;
    }

    if(version > 2)
    {
        in >> lineScale;
        in >> showArrows;
    }

    if(version > 3)
    {
        in >> limitsGraph1;
        in >> limitsGraph2;
        in >> limitsGraph3;
    }

    if(version > 5)
    {
        CEnergyCycling::energy_set_t set;
        in >> set;
        energyCycling.setEnergyTrkSet(set, false);
    }

    trk.segs.clear();
    in >> trk.segs;

    /* [Issue #408] Export of a database is broken

        Exporting the database is done in a thread other than the main GUI thread.
        As deriveSecondaryData() might call some GUI elements it has to be bypassed
        when restoring a track within an thread.
     */
    if(QThread::currentThread() == qApp->thread())
    {
        deriveSecondaryData();
    }
    setColor(str2color(trk.color));
    setText(CGisListWks::eColumnName, getName());
    setToolTip(CGisListWks::eColumnName, getInfo(IGisItem::eFeatureShowName));

    checkForInvalidPoints();
    return stream;
}

QDataStream& CGisItemWpt::operator<<(QDataStream& stream)
{
    quint8 version;
    QByteArray buffer;
    QIODevice * dev = stream.device();
    qint64 pos = dev->pos();

    char magic[10];
    stream.readRawData(magic, MAGIC_SIZE);

    if(strncmp(magic, MAGIC_WPT, MAGIC_SIZE))
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

    in >> key.item;
    in >> flags;
    in >> proximity;
    in >> wpt;
    in >> geocache;
    in >> images;
    if(version > 1)
    {
        in >> offsetBubble;
        in >> widthBubble;
    }
    if(version > 3)
    {
        in >> rating;
        in >> keywords;
    }

    if(version <= 2 && geocache.hasData)
    {
        //If the geocache was saved with an old Version of QMS recalculate it's key to make sure geocaches with the same id are treated as being the same
        key.item="";
        genKey();
    }
    //Also sets icon, tooltip and text
    IGisItem::updateDecoration(eMarkNone, eMarkNone);

    detBoundingRect();
    radius = NOFLOAT;

    return stream;
}

QDataStream& CGisItemWpt::operator>>(QDataStream& stream) const
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_5_2);

    out << key.item;
    out << flags;
    out << proximity;
    out << wpt;
    out << geocache;
    out << images;
    out << offsetBubble;
    out << widthBubble;
    out << rating;
    out << keywords;

    stream.writeRawData(MAGIC_WPT, MAGIC_SIZE);
    stream << VER_WPT;
    stream << qCompress(buffer, 9);

    return stream;
}

QDataStream& CGisItemRte::operator<<(QDataStream& stream)
{
    quint8 version;
    QByteArray buffer;
    QIODevice * dev = stream.device();
    qint64 pos = dev->pos();

    char magic[10];
    stream.readRawData(magic, MAGIC_SIZE);

    if(strncmp(magic, MAGIC_RTE, MAGIC_SIZE))
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

    in >> key.item;
    in >> flags;
    in >> rte.name;
    in >> rte.cmt;
    in >> rte.desc;
    in >> rte.src;
    in >> rte.links;
    in >> rte.number;
    in >> rte.type;
    in >> rte.pts;
    if(version > 1)
    {
        in >> rte.lastRoutedWith;
        in >> rte.lastRoutedTime;
        in >> rte.totalDistance;
        in >> rte.totalTime;
    }
    if(version > 2)
    {
        in >> rte.ascent;
        in >> rte.descent;
    }
    if(version > 3)
    {
        in >> rating;
        in >> keywords;
    }


    setSymbol();
    deriveSecondaryData();
    setText   (CGisListWks::eColumnName, getName());
    setToolTip(CGisListWks::eColumnName, getInfo(IGisItem::eFeatureShowName));

    return stream;
}

QDataStream& CGisItemRte::operator>>(QDataStream& stream) const
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_5_2);

    out << key.item;
    out << flags;
    out << rte.name;
    out << rte.cmt;
    out << rte.desc;
    out << rte.src;
    out << rte.links;
    out << rte.number;
    out << rte.type;
    out << rte.pts;
    out << rte.lastRoutedWith;
    out << rte.lastRoutedTime;
    out << rte.totalDistance;
    out << rte.totalTime;
    out << rte.ascent;
    out << rte.descent;
    out << rating;
    out << keywords;

    stream.writeRawData(MAGIC_RTE, MAGIC_SIZE);
    stream << VER_RTE;
    stream << qCompress(buffer, 9);

    return stream;
}

QDataStream& CGisItemOvlArea::operator<<(QDataStream& stream)
{
    quint8 version, tmp8;
    QByteArray buffer;
    QIODevice * dev = stream.device();
    qint64 pos = dev->pos();

    char magic[10];
    stream.readRawData(magic, MAGIC_SIZE);

    if(strncmp(magic, MAGIC_AREA, MAGIC_SIZE))
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

    in >> key.item;
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
    if(version > 1)
    {
        in >> rating;
        in >> keywords;
    }

    deriveSecondaryData();

    setColor(str2color(area.color));
    setText   (CGisListWks::eColumnName, getName());
    setToolTip(CGisListWks::eColumnName, getInfo(IGisItem::eFeatureShowName));

    return stream;
}

QDataStream& CGisItemOvlArea::operator>>(QDataStream& stream) const
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_5_2);

    out << key.item;
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
    out << rating;
    out << keywords;

    stream.writeRawData(MAGIC_AREA, MAGIC_SIZE);
    stream << VER_AREA;
    stream << qCompress(buffer, 9);

    return stream;
}

QDataStream& IGisProject::operator<<(QDataStream& stream)
{
    quint8 version;
    QIODevice * dev = stream.device();
    qint64 pos = dev->pos();

    char magic[10];
    stream.readRawData(magic, MAGIC_SIZE);

    if(strncmp(magic, MAGIC_PROJ, MAGIC_SIZE))
    {
        dev->seek(pos);
        return stream;
    }

    blockUpdateItems(true);

    stream >> version;
    if(filename.isEmpty())
    {
        stream >> filename;
    }
    else
    {
        QString tmp;
        stream >> tmp;
    }
    stream >> metadata.name;
    stream >> metadata.desc;
    stream >> metadata.author;
    stream >> metadata.copyright;
    stream >> metadata.links;
    stream >> metadata.time;
    stream >> metadata.keywords;
    stream >> metadata.bounds;
    if(version > 1)
    {
        stream >> key;
    }
    if(version > 2)
    {
        qint32 tmp;
        stream >> tmp;
        sortingRoadbook = (sorting_roadbook_e)tmp;
    }
    if(version > 3)
    {
        qint8 tmp;
        stream >> tmp;
        noCorrelation   = (tmp & eFlagNoCorrelation) != 0;
        autoSave        = (tmp & eFlagAutoSave) != 0;
        invalidDataOk   = (tmp & eFlagInvalidDataOk) != 0;
    }

    if(version > 4)
    {
        qint32 tmp;
        stream >> tmp;
        sortingFolder = (sorting_folder_e)tmp;
    }

    while(!stream.atEnd())
    {
        QString lastDatabaseHash;
        IGisItem::history_t history;
        quint8 changed = 0;
        quint8 version, type;
        stream >> version;
        stream >> type;
        stream >> history;
        if(version > 1)
        {
            stream >> changed;
        }

        if(version > 2)
        {
            stream >> lastDatabaseHash;
        }


        IGisItem *item = nullptr;
        switch(type)
        {
        case IGisItem::eTypeWpt:
            item = new CGisItemWpt(history, lastDatabaseHash, this);
            break;

        case IGisItem::eTypeTrk:
            item = new CGisItemTrk(history, lastDatabaseHash, this);
            break;

        case IGisItem::eTypeRte:
            item = new CGisItemRte(history, lastDatabaseHash, this);
            break;

        case IGisItem::eTypeOvl:
            item = new CGisItemOvlArea(history, lastDatabaseHash, this);
            break;

        default:
            ;
        }

        if(item && changed)
        {
            item->updateDecoration(IGisItem::eMarkChanged, IGisItem::eMarkNone);
        }
    }

    sortItems();

    blockUpdateItems(false);
    return stream;
}

QDataStream& IGisProject::operator>>(QDataStream& stream) const
{
    stream.writeRawData(MAGIC_PROJ, MAGIC_SIZE);
    stream << VER_PROJECT;

    stream << filename;
    stream << metadata.name;
    stream << metadata.desc;
    stream << metadata.author;
    stream << metadata.copyright;
    stream << metadata.links;
    stream << metadata.time;
    stream << metadata.keywords;
    stream << metadata.bounds;
    stream << key;
    stream << qint32(sortingRoadbook);
    stream << qint8((noCorrelation ? eFlagNoCorrelation : 0) | (autoSave ? eFlagAutoSave : 0) | (invalidDataOk ? eFlagInvalidDataOk : 0)); // collect trivial flags in one field.
    stream << qint32(sortingFolder);

    for(int i = 0; i < childCount(); i++)
    {
        CGisItemTrk * item = dynamic_cast<CGisItemTrk*>(child(i));
        if(nullptr == item)
        {
            continue;
        }
        stream << VER_ITEM;
        stream << quint8(item->type());
        stream << item->getHistory();
        stream << quint8(item->data(1, Qt::UserRole).toUInt() & IGisItem::eMarkChanged);
        stream << item->getLastDatabaseHash();
    }
    for(int i = 0; i < childCount(); i++)
    {
        CGisItemRte * item = dynamic_cast<CGisItemRte*>(child(i));
        if(nullptr == item)
        {
            continue;
        }
        stream << VER_ITEM;
        stream << quint8(item->type());
        stream << item->getHistory();
        stream << quint8(item->data(1, Qt::UserRole).toUInt() & IGisItem::eMarkChanged);
        stream << item->getLastDatabaseHash();
    }
    for(int i = 0; i < childCount(); i++)
    {
        CGisItemWpt * item = dynamic_cast<CGisItemWpt*>(child(i));
        if(nullptr == item)
        {
            continue;
        }
        stream << VER_ITEM;
        stream << quint8(item->type());
        stream << item->getHistory();
        stream << quint8(item->data(1, Qt::UserRole).toUInt() & IGisItem::eMarkChanged);
        stream << item->getLastDatabaseHash();
    }
    for(int i = 0; i < childCount(); i++)
    {
        CGisItemOvlArea * item = dynamic_cast<CGisItemOvlArea*>(child(i));
        if(nullptr == item)
        {
            continue;
        }
        stream << VER_ITEM;
        stream << quint8(item->type());
        stream << item->getHistory();
        stream << quint8(item->data(1, Qt::UserRole).toUInt() & IGisItem::eMarkChanged);
        stream << item->getLastDatabaseHash();
    }

    return stream;
}

QDataStream& CDBProject::operator<<(QDataStream& stream)
{
    quint8 version;
    QIODevice * dev = stream.device();
    qint64 pos = dev->pos();

    char magic[10];
    stream.readRawData(magic, MAGIC_SIZE);

    if(strncmp(magic, MAGIC_PROJ, MAGIC_SIZE))
    {
        dev->seek(pos);
        return stream;
    }

    stream >> version;
    stream >> filename;
    stream >> metadata.name;
    stream >> metadata.desc;
    stream >> metadata.author;
    stream >> metadata.copyright;
    stream >> metadata.links;
    stream >> metadata.time;
    stream >> metadata.keywords;
    stream >> metadata.bounds;
    if(version > 1)
    {
        stream >> key;
    }
    if(version > 2)
    {
        qint32 tmp;
        stream >> tmp;
        sortingRoadbook = (sorting_roadbook_e)tmp;
    }
    if(version > 3)
    {
        qint8 tmp;
        stream >> tmp;
        noCorrelation   = (tmp & eFlagNoCorrelation) != 0;
        autoSave        = (tmp & eFlagAutoSave) != 0;
        invalidDataOk   = (tmp & eFlagInvalidDataOk) != 0;
    }
    if(version > 4)
    {
        qint32 tmp;
        stream >> tmp;
        sortingFolder = (sorting_folder_e)tmp;
    }

    return stream;
}

QDataStream& CDBProject::operator>>(QDataStream& stream) const
{
    stream.writeRawData(MAGIC_PROJ, MAGIC_SIZE);
    stream << VER_PROJECT;

    stream << filename;
    stream << metadata.name;
    stream << metadata.desc;
    stream << metadata.author;
    stream << metadata.copyright;
    stream << metadata.links;
    stream << metadata.time;
    stream << metadata.keywords;
    stream << metadata.bounds;
    stream << key;
    stream << qint32(sortingRoadbook);
    stream << qint8((noCorrelation ? eFlagNoCorrelation : 0) | (autoSave ? eFlagAutoSave : 0) | (invalidDataOk ? eFlagInvalidDataOk : 0)); // collect trivial flags in one field.
    stream << qint32(sortingFolder);

    return stream;
}
