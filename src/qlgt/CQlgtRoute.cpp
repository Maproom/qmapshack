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

#include "CQlgtRoute.h"

struct rte_head_entry_t
{
    rte_head_entry_t() : type(CQlgtRoute::eEnd), offset(0)
    {
    }
    qint32 type;
    quint32 offset;
    QByteArray data;
};


QDataStream& operator >>(QDataStream& s, CQlgtRoute& route)
{
    quint32 nRtePts = 0;
    QIODevice * dev = s.device();
    qint64 pos = dev->pos();

    char magic[9];
    s.readRawData(magic,9);

    if(strncmp(magic,"QLRte   ",9))
    {
        dev->seek(pos);
        return(s);
    }

    QList<rte_head_entry_t> entries;

    while(1)
    {
        rte_head_entry_t entry;
        s >> entry.type >> entry.offset;
        entries << entry;
        if(entry.type == CQlgtRoute::eEnd)
        {
            break;
        }
    }

    QList<rte_head_entry_t>::iterator entry = entries.begin();
    while(entry != entries.end())
    {
        qint64 o = pos + entry->offset;
        dev->seek(o);
        s >> entry->data;

        switch(entry->type)
        {
        case CQlgtRoute::eBase:
        {
            QDataStream s1(&entry->data, QIODevice::ReadOnly);
            s1.setVersion(QDataStream::Qt_4_5);

            s1 >> route.key;
            s1 >> route.timestamp;
            s1 >> route.name;
            s1 >> route.iconString;
            s1 >> route.ttime;
            s1 >> route.parentWpt;

            break;
        }

        case CQlgtRoute::eRtePts:
        {
            QDataStream s1(&entry->data, QIODevice::ReadOnly);
            s1.setVersion(QDataStream::Qt_4_5);
            quint32 n;

            route.priRoute.clear();
            s1 >> nRtePts;

            for(n = 0; n < nRtePts; ++n)
            {
                CQlgtRoute::pt_t rtept;
                float u, v;
                QString action;

                s1 >> u;
                s1 >> v;
                s1 >> action;

                rtept.lon = u;
                rtept.lat = v;
                rtept.action = action;
                route.priRoute << rtept;
            }
            break;
        }
//            case CQlgtRoute::eRteSec:
//            {
//                QDataStream s1(&entry->data, QIODevice::ReadOnly);
//                s1.setVersion(QDataStream::Qt_4_5);
//                quint32 n;

//                route.secRoute.clear();
//                s1 >> nRtePts;

//                for(n = 0; n < nRtePts; ++n)
//                {
//                    CQlgtRoute::pt_t rtept;
//                    float u, v;
//                    QString action;

//                    s1 >> u;
//                    s1 >> v;
//                    s1 >> action;

//                    rtept.lon = u;
//                    rtept.lat = v;
//                    rtept.action = action;
//                    route.secRoute << rtept;
//                }
//                break;
//            }
        default:;
        }

        ++entry;
    }


    return(s);
}

QDataStream& operator <<(QDataStream& s, CQlgtRoute& route)
{
    QList<rte_head_entry_t> entries;

    //---------------------------------------
    // prepare base data
    //---------------------------------------
    rte_head_entry_t entryBase;
    entryBase.type = CQlgtRoute::eBase;
    QDataStream s1(&entryBase.data, QIODevice::WriteOnly);
    s1.setVersion(QDataStream::Qt_4_5);

    s1 << route.key;
    s1 << route.timestamp;
    s1 << route.name;
    s1 << route.iconString;
    s1 << route.ttime;
    s1 << route.parentWpt;

    entries << entryBase;

    //---------------------------------------
    // prepare primary routepoint data
    //---------------------------------------
    rte_head_entry_t entryPriRtePts;
    entryPriRtePts.type = CQlgtRoute::eRtePts;
    QDataStream s2(&entryPriRtePts.data, QIODevice::WriteOnly);
    s2.setVersion(QDataStream::Qt_4_5);

    {
        QVector<CQlgtRoute::pt_t>& rtepts           = route.priRoute;
        QVector<CQlgtRoute::pt_t>::iterator rtept   = rtepts.begin();

        s2 << (quint32)rtepts.size();
        while(rtept != rtepts.end())
        {
            s2 << (float)rtept->lon;
            s2 << (float)rtept->lat;
            s2 << rtept->action;
            ++rtept;
        }
    }
    entries << entryPriRtePts;

//    //---------------------------------------
//    // prepare secondary routepoint data
//    //---------------------------------------
//    rte_head_entry_t entrySecRtePts;
//    entrySecRtePts.type = CQlgtRoute::eRteSec;
//    QDataStream s3(&entrySecRtePts.data, QIODevice::WriteOnly);
//    s3.setVersion(QDataStream::Qt_4_5);

//    {
//        QVector<CQlgtRoute::pt_t>& rtepts           = route.getSecRtePoints();
//        QVector<CQlgtRoute::pt_t>::iterator rtept   = rtepts.begin();

//        if(!rtepts.isEmpty())
//        {
//            s3 << (quint32)rtepts.size();
//            while(rtept != rtepts.end())
//            {
//                s3 << (float)rtept->lon;
//                s3 << (float)rtept->lat;
//                s3 << rtept->action;
//                ++rtept;
//            }
//            entries << entrySecRtePts;
//        }

//    }

    //---------------------------------------
    // prepare terminator
    //---------------------------------------
    rte_head_entry_t entryEnd;
    entryEnd.type = CQlgtRoute::eEnd;
    entries << entryEnd;

    //---------------------------------------
    //---------------------------------------
    // now start to actually write data;
    //---------------------------------------
    //---------------------------------------
    // write magic key
    s.writeRawData("QLRte   ",9);

    // calculate offset table
    quint32 offset = entries.count() * 8 + 9;

    QList<rte_head_entry_t>::iterator entry = entries.begin();
    while(entry != entries.end())
    {
        entry->offset = offset;
        offset += entry->data.size() + sizeof(quint32);
        ++entry;
    }

    // write offset table
    entry = entries.begin();
    while(entry != entries.end())
    {
        s << entry->type << entry->offset;
        ++entry;
    }

    // write entry data
    entry = entries.begin();
    while(entry != entries.end())
    {
        s << entry->data;
        ++entry;
    }


    return(s);
}

CQlgtRoute::CQlgtRoute(quint64 id, QObject *parent)
    : QObject(parent)
    , IItem(id)
{
}

CQlgtRoute::~CQlgtRoute()
{
}

