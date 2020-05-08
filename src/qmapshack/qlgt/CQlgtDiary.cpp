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

#include "CQlgtDiary.h"

struct diary_head_entry_t
{
    diary_head_entry_t() : type(CQlgtDiary::eEnd), offset(0)
    {
    }
    qint32 type;
    quint32 offset;
    QByteArray data;
};


QDataStream& operator >>(QDataStream& s, CQlgtDiary& diary)
{
    QIODevice * dev = s.device();
    qint64 pos = dev->pos();

    char magic[9];
    s.readRawData(magic, 9);

    if(strncmp(magic, "QLDry   ", 9))
    {
        dev->seek(pos);
        return s;
    }

    QList<diary_head_entry_t> entries;

    while(1)
    {
        diary_head_entry_t entry;
        s >> entry.type >> entry.offset;
        entries << entry;
        if(entry.type == CQlgtDiary::eEnd)
        {
            break;
        }
    }

    QList<diary_head_entry_t>::iterator entry = entries.begin();
    while(entry != entries.end())
    {
        qint64 o = pos + entry->offset;
        dev->seek(o);
        s >> entry->data;
        switch(entry->type)
        {
        case CQlgtDiary::eBase:
        {
            QString comment, name, key;
            QDataStream s1(&entry->data, QIODevice::ReadOnly);
            s1.setVersion(QDataStream::Qt_4_5);

            s1 >> diary.timestamp;
            s1 >> diary.comment;
            s1 >> diary.name;
            s1 >> diary.keyProjectGeoDB;
            s1 >> diary.key;

            break;
        }
//            case CQlgtDiary::eWpt:
//            {
//                int cnt;
//                QDataStream s1(&entry->data, QIODevice::ReadOnly);
//                s1.setVersion(QDataStream::Qt_4_5);

//                s1 >> cnt;
//                for(int i=0; i < cnt; i++)
//                {
//                    CWpt * wpt = new CWpt(&diary);
//                    s1 >> *wpt;
//                    diary.wpts << wpt;
//                }
//                break;

//            }
//            case CQlgtDiary::eTrk:
//            {
//                int cnt;
//                QDataStream s1(&entry->data, QIODevice::ReadOnly);
//                s1.setVersion(QDataStream::Qt_4_5);

//                s1 >> cnt;
//                for(int i=0; i < cnt; i++)
//                {
//                    CTrack * trk = new CTrack(&diary);
//                    s1 >> *trk;
//                    trk->rebuild(true);
//                    diary.trks << trk;
//                }
//                break;

//            }
//            case CQlgtDiary::eRte:
//            {
//                int cnt;
//                QDataStream s1(&entry->data, QIODevice::ReadOnly);
//                s1.setVersion(QDataStream::Qt_4_5);

//                s1 >> cnt;
//                for(int i=0; i < cnt; i++)
//                {
//                    CRoute * rte = new CRoute(&diary);
//                    s1 >> *rte;
//                    diary.rtes << rte;
//                }
//                break;

//            }
        default:
            ;
        }

        ++entry;
    }

    return s;
}

QDataStream& operator <<(QDataStream& s, CQlgtDiary& diary)
{
    QList<diary_head_entry_t> entries;


    //---------------------------------------
    // prepare base data
    //---------------------------------------
    diary_head_entry_t entryBase;
    entryBase.type = CQlgtDiary::eBase;
    QDataStream s1(&entryBase.data, QIODevice::WriteOnly);
    s1.setVersion(QDataStream::Qt_4_5);

    s1 << diary.timestamp;
    s1 << diary.comment;
    s1 << diary.name;
    s1 << diary.keyProjectGeoDB;
    s1 << diary.key;
    entries << entryBase;

//    //---------------------------------------
//    // prepare waypoint data
//    //---------------------------------------
//    diary_head_entry_t entryWpt;
//    entryWpt.type = CQlgtDiary::eWpt;
//    QDataStream s2(&entryWpt.data, QIODevice::WriteOnly);
//    s2.setVersion(QDataStream::Qt_4_5);

//    s2 << diary.wpts.count();
//    for(CWpt * wpt : diary.wpts)
//    {
//        s2 << *wpt;
//    }

//    entries << entryWpt;

//    //---------------------------------------
//    // prepare track data
//    //---------------------------------------
//    diary_head_entry_t entryTrk;
//    entryTrk.type = CQlgtDiary::eTrk;
//    QDataStream s3(&entryTrk.data, QIODevice::WriteOnly);
//    s3.setVersion(QDataStream::Qt_4_5);

//    s3 << diary.trks.count();
//    for(CTrack * trk : diary.trks)
//    {
//        s3 << *trk;
//    }

//    entries << entryTrk;

//    //---------------------------------------
//    // prepare route data
//    //---------------------------------------
//    diary_head_entry_t entryRte;
//    entryRte.type = CQlgtDiary::eRte;
//    QDataStream s4(&entryRte.data, QIODevice::WriteOnly);
//    s4.setVersion(QDataStream::Qt_4_5);

//    s4 << diary.rtes.count();
//    for(CRoute * rte : diary.rtes)
//    {
//        s4 << *rte;
//    }

//    entries << entryRte;

    //---------------------------------------
    // prepare terminator
    //---------------------------------------
    diary_head_entry_t entryEnd;
    entryEnd.type = CQlgtDiary::eEnd;
    entries << entryEnd;

    //---------------------------------------
    //---------------------------------------
    // now start to actually write data;
    //---------------------------------------
    //---------------------------------------
    // write magic key
    s.writeRawData("QLDry   ", 9);

    // calculate offset table
    quint32 offset = entries.count() * 8 + 9;

    QList<diary_head_entry_t>::iterator entry = entries.begin();
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

    return s;
}

CQlgtDiary::CQlgtDiary(quint64 id, QObject *parent)
    : QObject(parent)
    , IItem(id)
{
}

CQlgtDiary::~CQlgtDiary()
{
}

