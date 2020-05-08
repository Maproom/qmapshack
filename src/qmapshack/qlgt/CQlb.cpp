/**********************************************************************************************
    Copyright (C) 2007 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CQlb.h"
#include "qlgt/CQlgtDiary.h"
#include "qlgt/CQlgtRoute.h"
#include "qlgt/CQlgtTrack.h"
#include "qlgt/CQlgtWpt.h"
#include "qlgt/IQlgtOverlay.h"


#include <QtCore>

CQlb::CQlb(QObject * parent)
    : QObject(parent)
{
}


CQlb::~CQlb()
{
}


CQlb& CQlb::operator <<(CQlgtWpt& wpt)
{
    QDataStream stream(&wpts, QIODevice::Append);
    stream.setVersion(QDataStream::Qt_4_5);
    stream << wpt;

    return *this;
}


CQlb& CQlb::operator <<(CQlgtTrack& trk)
{
    QDataStream stream(&trks, QIODevice::Append);
    stream.setVersion(QDataStream::Qt_4_5);
    stream << trk;

    return *this;
}


CQlb& CQlb::operator <<(CQlgtRoute& rte)
{
    QDataStream stream(&rtes, QIODevice::Append);
    stream.setVersion(QDataStream::Qt_4_5);
    stream << rte;

    return *this;
}


CQlb& CQlb::operator <<(CQlgtDiary& dry)
{
    QDataStream stream(&drys, QIODevice::Append);
    stream.setVersion(QDataStream::Qt_4_5);
    stream << dry;

    return *this;
}


CQlb& CQlb::operator <<(IQlgtOverlay& ovl)
{
    QDataStream stream(&ovls, QIODevice::Append);
    stream.setVersion(QDataStream::Qt_4_5);
    stream << ovl;

    return *this;
}




void CQlb::load(const QString& filename)
{
    QFile file(filename);
    load(&file);
}


void CQlb::load(QIODevice* ioDevice)
{
    qint32 type;

    ioDevice->open(QIODevice::ReadOnly);
    QDataStream stream(ioDevice);
    stream.setVersion(QDataStream::Qt_4_5);

    stream >> type;
    while(type != eEnd)
    {
        switch(type)
        {
        case eWpt:
            stream >> wpts;
            break;

        case eTrack:
            stream >> trks;
            break;

        case eDiary:
            stream >> drys;
            break;

        case eOverlay:
            stream >> ovls;
            break;

        case eRoute:
            stream >> rtes;
            break;

        case eMapSel:
            stream >> sels;
            break;

        default:
            ioDevice->close();
            return;
        }

        stream >> type;
    }

    ioDevice->close();
}


void CQlb::save(const QString& filename)
{
    QFile file(filename);
    save(&file);
}


void CQlb::save(QIODevice* ioDevice)
{
    ioDevice->open(QIODevice::WriteOnly);
    QDataStream stream(ioDevice);
    stream.setVersion(QDataStream::Qt_4_5);

    stream << (qint32)eWpt << wpts;
    stream << (qint32)eTrack << trks;
    stream << (qint32)eRoute << rtes;
    stream << (qint32)eDiary << drys;
    stream << (qint32)eOverlay << ovls;
    stream << (qint32)eMapSel << sels;
    stream << (qint32)eEnd;

    ioDevice->close();
}
