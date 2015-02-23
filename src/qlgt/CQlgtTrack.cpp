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

#include "CMainWindow.h"
#include "CQlgtTrack.h"

#include <QtWidgets>

QDataStream& operator >>(QDataStream& s, CFlags& flag)
{
    quint32 f;
    s >> f;
    flag.setFlags(f);
    flag.setChanged(true);
    return s;
}


QDataStream& operator <<(QDataStream& s, CFlags& flag)
{
    s << flag.flag();
    return s;
}


struct trk_head_entry_t
{
    trk_head_entry_t() : type(CQlgtTrack::eEnd), offset(0)
    {
    }
    qint32 type;
    quint32 offset;
    QByteArray data;
};


QDataStream& operator >>(QDataStream& s, CQlgtTrack& track)
{
    quint32 nTrkPts = 0;
    QIODevice * dev = s.device();
    qint64 pos = dev->pos();

    char magic[9];
    s.readRawData(magic,9);

    if(strncmp(magic,"QLTrk   ",9))
    {
        dev->seek(pos);
        return s;
    }

    QList<trk_head_entry_t> entries;

    while(1)
    {
        trk_head_entry_t entry;
        s >> entry.type >> entry.offset;
        entries << entry;
        if(entry.type == CQlgtTrack::eEnd)
        {
            break;
        }
    }

    QList<trk_head_entry_t>::iterator entry = entries.begin();
    while(entry != entries.end())
    {
        qint64 o = pos + entry->offset;
        dev->seek(o);
        s >> entry->data;

        switch(entry->type)
        {
        case CQlgtTrack::eBase:
        {
            QString key;

            QDataStream s1(&entry->data, QIODevice::ReadOnly);
            s1.setVersion(QDataStream::Qt_4_5);

            s1 >> track.key;
            s1 >> track.timestamp;
            s1 >> track.name;
            s1 >> track.comment;
            s1 >> track.colorIdx;
            s1 >> track.parentWpt;
            if(!s1.atEnd())
            {
                s1 >> track.doScaleWpt2Track;
            }
            if(!s1.atEnd())
            {
                s1 >> track.cntMedianFilterApplied;
            }
            if(!s1.atEnd())
            {
                s1 >> track.useMultiColor;
            }
            if(!s1.atEnd())
            {
                s1 >> track.idMultiColor;
            }

            //track.setColor(track.colorIdx);

            break;
        }

        case CQlgtTrack::eTrkPts:
        {
            QDataStream s1(&entry->data, QIODevice::ReadOnly);
            s1.setVersion(QDataStream::Qt_4_5);
            quint32 n;

            track.track.clear();
            s1 >> nTrkPts;

            for(n = 0; n < nTrkPts; ++n)
            {
                CQlgtTrack::pt_t trkpt;
                s1 >> trkpt.lon;
                s1 >> trkpt.lat;
                s1 >> trkpt.ele;
                s1 >> trkpt.timestamp;
                s1 >> trkpt.flags;

                trkpt._lon = trkpt.lon;
                trkpt._lat = trkpt.lat;
                trkpt._ele = trkpt.ele;
                trkpt._timestamp = trkpt.timestamp;
                trkpt._timestamp_msec = trkpt.timestamp_msec;

                track << trkpt;
            }
            break;
        }

        case CQlgtTrack::eTrkPts2:
        {
            QDataStream s1(&entry->data, QIODevice::ReadOnly);
            s1.setVersion(QDataStream::Qt_4_5);

            quint32 nTrkPts1 = 0;

            s1 >> nTrkPts1;
            if(nTrkPts1 != nTrkPts)
            {
                QMessageBox::warning(&CMainWindow::self(), QObject::tr("Corrupt track ..."), QObject::tr("Number of trackpoints is not equal the number of training data trackpoints."), QMessageBox::Ignore,QMessageBox::Ignore);
                break;
            }

            QList<CQlgtTrack::pt_t>::iterator pt1 = track.track.begin();
            while (pt1 != track.track.end())
            {
                quint32 dummy;
                s1 >> pt1->timestamp_msec;
                s1 >> dummy;
                s1 >> dummy;
                s1 >> dummy;
                s1 >> dummy;
                s1 >> dummy;

                pt1++;
            }
            break;
        }

//            case CQlgtTrack::eTrain:
//            {
//                QDataStream s1(&entry->data, QIODevice::ReadOnly);
//                s1.setVersion(QDataStream::Qt_4_5);

//                quint32 nTrkPts1 = 0;

//                s1 >> nTrkPts1;
//                if(nTrkPts1 != nTrkPts)
//                {
//                    QMessageBox::warning(0, QObject::tr("Corrupt track ..."), QObject::tr("Number of trackpoints is not equal the number of training data trackpoints."), QMessageBox::Ignore,QMessageBox::Ignore);
//                    break;
//                }

//                QList<CQlgtTrack::pt_t>::iterator pt1 = track.track.begin();
//                while (pt1 != track.track.end())
//                {
//                    s1 >> pt1->heartReateBpm;
//                    s1 >> pt1->cadenceRpm;
//                    pt1++;
//                }

//                track.setTraineeData();
//                break;
//            }
        case CQlgtTrack::eTrkExt1:
        {
            QDataStream s1(&entry->data, QIODevice::ReadOnly);
            s1.setVersion(QDataStream::Qt_4_5);
            quint32 nTrkPts1 = 0;

            s1 >> nTrkPts1;
            if(nTrkPts1 != nTrkPts)
            {
                QMessageBox::warning(&CMainWindow::self(), QObject::tr("Corrupt track ..."), QObject::tr("Number of trackpoints is not equal the number of extended data trackpoints."), QMessageBox::Ignore,QMessageBox::Ignore);
                break;
            }

            QList<CQlgtTrack::pt_t>::iterator pt1 = track.track.begin();
            while (pt1 != track.track.end())
            {
                ///< [m]
                s1 >> pt1->altitude;
                ///< [m]
                s1 >> pt1->height;
                ///< [m/s]
                s1 >> pt1->velocity;
                ///< [deg]
                s1 >> pt1->heading;
                ///< [deg]
                s1 >> pt1->magnetic;
                s1 >> pt1->vdop;
                s1 >> pt1->hdop;
                s1 >> pt1->pdop;
                s1 >> pt1->x;    ///< [m] cartesian gps coordinate
                s1 >> pt1->y;    ///< [m] cartesian gps coordinate
                s1 >> pt1->z;    ///< [m] cartesian gps coordinate
                                 ///< [m/s] velocity
                s1 >> pt1->vx;
                ///< [m/s] velocity
                s1 >> pt1->vy;
                ///< [m/s] velocity
                s1 >> pt1->vz;
                pt1++;
            }

            track.setExt1Data();
            break;
        }

        case CQlgtTrack::eTrkShdw:
        {
            QDataStream s1(&entry->data, QIODevice::ReadOnly);
            s1.setVersion(QDataStream::Qt_4_5);
            quint32 n;

            quint32 nTrkPts1 = 0;

            s1 >> nTrkPts1;
            if(nTrkPts1 != nTrkPts)
            {
                QMessageBox::warning(&CMainWindow::self(), QObject::tr("Corrupt track ..."), QObject::tr("Number of trackpoints is not equal the number of shadow data trackpoints."), QMessageBox::Ignore,QMessageBox::Ignore);
                break;
            }

            for(n = 0; n < nTrkPts; ++n)
            {
                CQlgtTrack::pt_t& trkpt = track.track[n];
                s1 >> trkpt._lon;
                s1 >> trkpt._lat;
                s1 >> trkpt._ele;
            }
            track.hasShadow1 = true;
            break;
        }

        case CQlgtTrack::eTrkShdw2:
        {
            QDataStream s1(&entry->data, QIODevice::ReadOnly);
            s1.setVersion(QDataStream::Qt_4_5);
            quint32 n;

            quint32 nTrkPts1 = 0;

            s1 >> nTrkPts1;
            if(nTrkPts1 != nTrkPts)
            {
                QMessageBox::warning(&CMainWindow::self(), QObject::tr("Corrupt track ..."), QObject::tr("Number of trackpoints is not equal the number of shadow data trackpoints."), QMessageBox::Ignore,QMessageBox::Ignore);
                break;
            }

            for(n = 0; n < nTrkPts; ++n)
            {
                quint32 dummy;
                CQlgtTrack::pt_t& trkpt = track.track[n];
                s1 >> trkpt._timestamp;
                s1 >> trkpt._timestamp_msec;
                s1 >> dummy;
                s1 >> dummy;
                s1 >> dummy;
                s1 >> dummy;
                s1 >> dummy;
            }
            track.hasShadow2 = true;
            break;
        }

        default:;
        }

        ++entry;
    }

    return s;
}

QDataStream& operator <<(QDataStream& s, CQlgtTrack& trk)
{
    return s;
}

CQlgtTrack::CQlgtTrack(quint64 id, QObject *parent)
    : QObject(parent)
    , IItem(id)
    , ext1Data(false)
    , hasShadow1(false)
    , hasShadow2(false)
{
}

CQlgtTrack::~CQlgtTrack()
{
}

CQlgtTrack& CQlgtTrack::operator<<(const pt_t& pt)
{
    track.push_back(pt);
    track.last().idx     = track.size() - 1;
    track.last().flags  &= ~pt_t::eCursor;
    track.last().flags  &= ~pt_t::eFocus;
    track.last().flags  &= ~pt_t::eSelected;

    return *this;
}
