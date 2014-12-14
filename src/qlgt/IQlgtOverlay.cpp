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

#include "IQlgtOverlay.h"

struct ovl_head_entry_t
{
    ovl_head_entry_t() : type(IQlgtOverlay::eEnd), offset(0) {}
    qint32      type;
    quint32     offset;
    QByteArray  data;
};


QDataStream& operator >>(QDataStream& s, IQlgtOverlay& ovl)
{
    QIODevice * dev = s.device();
    qint64      pos = dev->pos();

    char magic[9];
    s.readRawData(magic,9);

    if(strncmp(magic,"QLOvl   ",9))
    {
        dev->seek(pos);
        return s;
    }

    QList<ovl_head_entry_t> entries;

    while(1)
    {
        ovl_head_entry_t entry;
        s >> entry.type >> entry.offset;
        entries << entry;
        if(entry.type == IQlgtOverlay::eEnd) break;
    }

    QList<ovl_head_entry_t>::iterator entry = entries.begin();
    while(entry != entries.end())
    {
        qint64 o = pos + entry->offset;
        dev->seek(o);
        s >> entry->data;

        switch(entry->type)
        {
            case IQlgtOverlay::eBase:
            {

                QDataStream s1(&entry->data, QIODevice::ReadOnly);
                s1.setVersion(QDataStream::Qt_4_5);

                s1 >> ovl.type;
                if(ovl.type == "Text")
                {
                    QRect rect;
                    QString text;
                    s1 >> rect >> text >> ovl.key;
                }
                else if(ovl.type == "TextBox")
                {
                    QRect rect;
                    QPoint pt;
                    QString text;
                    double lon, lat;
                    s1 >> lon >> lat >> pt >> rect >> text >> ovl.key;
                }
                else if(ovl.type == "Distance")
                {
                    float speed;
                    QString name;
                    QString comment;
                    QString parentWpt;
                    int size, idx = 0;
                    IQlgtOverlay::pt_t pt;
                    s1 >> name >> comment >> size;
                    for(int i = 0; i < size; ++i)
                    {
                        s1 >> pt.u >> pt.v;
                        pt.idx = idx++;
                    }
                    s1 >> speed >> ovl.key >> parentWpt;
                }
                else if(ovl.type == "Area")
                {
                    int size, idx = 0;
                    IQlgtOverlay::pt_t pt;
                    s1 >> ovl.name >> ovl.comment >> size;
                    for(int i = 0; i < size; ++i)
                    {
                        s1 >> pt.u >> pt.v;
                        pt.idx = idx++;
                        ovl.points << pt;
                    }
                    s1 >> ovl.color >> ovl.key >> ovl.parentWpt >> ovl.style >> ovl.width >> ovl.opacity;
                }
                break;
            }

            default:;
        }
        ++entry;
    }

    return s;
}

QDataStream& operator <<(QDataStream& s, IQlgtOverlay& ovl)
{
    QList<ovl_head_entry_t> entries;

    //---------------------------------------
    // prepare base data
    //---------------------------------------
    ovl_head_entry_t entryBase;
    entryBase.type = IQlgtOverlay::eBase;
    QDataStream s1(&entryBase.data, QIODevice::WriteOnly);
    s1.setVersion(QDataStream::Qt_4_5);
    s1 << ovl.type;
//    ovl.save(s1);
    entries << entryBase;

    //---------------------------------------
    // prepare terminator
    //---------------------------------------
    ovl_head_entry_t entryEnd;
    entryEnd.type = IQlgtOverlay::eEnd;
    entries << entryEnd;

    //---------------------------------------
    //---------------------------------------
    // now start to actually write data;
    //---------------------------------------
    //---------------------------------------
    // write magic key
    s.writeRawData("QLOvl   ",9);

    // calculate offset table
    quint32 offset = entries.count() * 8 + 9;

    QList<ovl_head_entry_t>::iterator entry = entries.begin();
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


IQlgtOverlay::IQlgtOverlay(QObject *parent)
{

}

IQlgtOverlay::~IQlgtOverlay()
{

}

