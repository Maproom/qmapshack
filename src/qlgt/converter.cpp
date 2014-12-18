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

#include "gis/WptIcons.h"
#include "gis/db/CDBProject.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/trk/CGisItemTrk.h"

#include "qlgt/CQlgtFolder.h"
#include "qlgt/CQlgtWpt.h"
#include "qlgt/CQlgtTrack.h"
#include "qlgt/CQlgtDiary.h"
#include "units/IUnit.h"

inline qreal readFloat(float val)
{
    return val > NOFLOAT ? NOFLOAT : val;
}

CDBProject::CDBProject(CQlgtFolder& folder)
    : IGisProject(eTypeDb, "", 0)
{
    metadata.name = folder.name;
    if(folder.diary)
    {
        metadata.desc = folder.diary->comment;
    }
}

CGisItemWpt::CGisItemWpt(const CQlgtWpt& wpt1)
    : IGisItem(0, eTypeWpt, -1)
{
    qreal direction;
    QDateTime time = QDateTime::fromTime_t(wpt1.timestamp,QTimeZone("UTC"));

    wpt.time        = time.toUTC();
    wpt.name        = wpt1.name;
    wpt.cmt         = wpt1.comment;
    wpt.desc        = wpt1.description;
    wpt.sym         = wpt1.iconString;

    wpt.lat     = readFloat(wpt1.lat);
    wpt.lon     = readFloat(wpt1.lon);
    wpt.ele     = wpt1.ele == WPT_NOFLOAT ? NOINT : qRound(wpt1.ele);
    proximity   = readFloat(wpt1.prx);
    direction   = readFloat(wpt1.dir);
    if(!wpt1.link.isEmpty())
    {
        IGisItem::link_t link;
        link.uri = wpt1.link;
        wpt.links << link;
    }
    if(!wpt1.urlname.isEmpty())
    {
        IGisItem::link_t link;
        link.uri = wpt1.urlname;
        wpt.links << link;
    }
    wpt.type    = wpt1.type;

    if(wpt1.geocache.hasData)
    {

        geocache.service    = CGisItemWpt::geocacheservice_e(wpt1.geocache.service);
        geocache.hasData    = wpt1.geocache.hasData;
        geocache.id         = wpt1.geocache.id;
        geocache.available  = wpt1.geocache.available;
        geocache.archived   = wpt1.geocache.archived;
        geocache.difficulty = wpt1.geocache.difficulty;
        geocache.terrain    = wpt1.geocache.terrain;
        geocache.status     = wpt1.geocache.status;
        geocache.name       = wpt1.geocache.name;
        geocache.owner      = wpt1.geocache.owner;
        geocache.ownerId    = wpt1.geocache.ownerId;
        geocache.type       = wpt1.geocache.type;
        geocache.container  = wpt1.geocache.container;
        geocache.shortDescIsHtml = wpt1.geocache.shortDesc.contains("</");
        geocache.shortDesc  = wpt1.geocache.shortDesc;
        geocache.longDescIsHtml = wpt1.geocache.longDesc.contains("</");
        geocache.longDesc   = wpt1.geocache.longDesc;
        geocache.hint       = wpt1.geocache.hint;
        geocache.country    = wpt1.geocache.country;
        geocache.state      = wpt1.geocache.state;
        geocache.locale     = wpt1.geocache.locale;

        foreach(const CQlgtWpt::geocachelog_t& log1, wpt1.geocache.logs)
        {
            CGisItemWpt::geocachelog_t log;
            log.id          = log1.id;
            IUnit::parseTimestamp(log1.date, log.date);
            log.type        = log1.type;;
            log.finderId    = log1.finderId;;
            log.finder      = log1.finder;;
            log.textIsHtml  = log1.text.contains("</");
            log.text        = log1.text;
            geocache.logs << log;
        }
    }

    foreach(const CQlgtWpt::image_t& image1, wpt1.images)
    {
        CGisItemWpt::image_t image;

        image.pixmap        = image1.pixmap.toImage();
        image.direction     = direction;
        image.info          = image1.info;
        image.filePath      = image1.filePath;
        image.fileName      = image1.fileName;

        images << image;
    }
    setIcon();
    genKey();
    setupHistory();
}


CGisItemTrk::CGisItemTrk(const CQlgtTrack &trk1)
    : IGisItem(0, eTypeTrk, -1)
{

    trk.name        = trk1.name;
    trk.cmt         = trk1.comment;
    trk.desc        = trk1.description;
    trk.color       = lineColors[trk1.colorIdx].name();
    setColor(lineColors[trk1.colorIdx]);

    bool hasExtData = trk1.hasExt1Data();
    trkseg_t seg;
    foreach(const CQlgtTrack::pt_t& pt1, trk1.track)
    {
        trkpt_t pt;
        QDateTime time = QDateTime::fromTime_t(pt1._timestamp,QTimeZone("UTC"));
        time.addMSecs(pt1._timestamp_msec);

        pt.lon  = pt1._lon;
        pt.lat  = pt1._lat;
        pt.ele  = pt1._ele == WPT_NOFLOAT ? NOINT : qRound(pt1._ele);
        pt.time = time.toUTC();

        if(hasExtData)
        {
            pt.hdop = pt1.hdop;
            pt.vdop = pt1.vdop;
            pt.pdop = pt1.pdop;
        }

        seg.pts << pt;
    }

    trk.segs << seg;

    genKey();
    setupHistory();

    bool hasHiddenPoints = false;
    for(int i = 0; i < trk1.track.size(); i++)
    {
        trkpt_t&                pt  = trk.segs[0].pts[i];
        const CQlgtTrack::pt_t& pt1 = trk1.track[i];

        if(pt1.flags & CQlgtTrack::pt_t::eDeleted)
        {
            pt.flags |= trkpt_t::eHidden;
            hasHiddenPoints = true;
        }
    }

    deriveSecondaryData();

    if(hasHiddenPoints)
    {
        // append history by new entry
        history.events << history_event_t();
        history_event_t& event = history.events.last();
        event.time      = QDateTime::currentDateTimeUtc();
        event.comment   = QObject::tr("Copy flag information from QLandkarte GT track");
        event.icon      = "://icons/48x48/PointHide.png";

        QDataStream stream(&event.data, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setVersion(QDataStream::Qt_5_2);

        *this >> stream;

        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData(event.data);
        event.hash = md5.result().toHex();

        history.histIdxCurrent = history.events.size() - 1;
    }
}
