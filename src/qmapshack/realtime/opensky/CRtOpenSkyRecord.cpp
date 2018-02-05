/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#include "realtime/CRtDraw.h"
#include "realtime/opensky/CRtOpenSkyRecord.h"

CRtOpenSkyRecord::CRtOpenSkyRecord(QObject *parent)
    : IRtRecord(parent)
{
}


bool CRtOpenSkyRecord::setFile(const QString& filename)
{
    track.clear();
    return IRtRecord::setFile(filename);
}

bool CRtOpenSkyRecord::writeEntry(const CRtOpenSky::aircraft_t& aircraft)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);
    stream.setByteOrder(QDataStream::LittleEndian);

    // it's always a good idea to start with a version tag for future changes.
    stream << quint8(1);

    CTrackData::trkpt_t trkpt;
    trkpt.lon   = aircraft.longitude;
    trkpt.lat   = aircraft.latitude;
    trkpt.ele   = aircraft.geoAltitude;
    trkpt.time  = QDateTime::fromTime_t(aircraft.timePosition);

    stream  << trkpt;
    track   << trkpt;

    return writeEntry(data);
}

bool CRtOpenSkyRecord::readEntry(QByteArray& data)
{
    QDataStream stream(&data, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_5_2);
    stream.setByteOrder(QDataStream::LittleEndian);

    quint8 version;
    stream >> version;

    CTrackData::trkpt_t trkpt;
    stream  >> trkpt;
    track   << trkpt;
    return true;
}

void CRtOpenSkyRecord::draw(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt)
{
    QPolygonF tmp;
    for(const CTrackData::trkpt_t& trkpt : track)
    {
        tmp << QPointF(trkpt.lon * DEG_TO_RAD, trkpt.lat * DEG_TO_RAD);
    }

    rt->convertRad2Px(tmp);
    p.setPen(QPen(Qt::black, 3));
    p.drawPolyline(tmp);
}

void CRtOpenSkyRecord::reset()
{
    track.clear();
    IRtRecord::reset();
}
