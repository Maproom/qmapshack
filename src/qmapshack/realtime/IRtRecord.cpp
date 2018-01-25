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

#include "realtime/IRtRecord.h"

#include <QtCore>

IRtRecord::IRtRecord(QObject *parent)
    : QObject(parent)
{

    stream.setDevice(&file);
    stream.setVersion(QDataStream::Qt_5_2);
    stream.setByteOrder(QDataStream::LittleEndian);
}

IRtRecord::~IRtRecord()
{
    if(file.isOpen())
    {
        file.close();
    }
}

bool IRtRecord::open(const QString& filename)
{
    if(file.isOpen())
    {
        file.close();
    }

    if(QFile::exists(filename))
    {
        if(!readFile(filename))
        {
            return false;
        }
    }

    file.setFileName(filename);
    if(!file.open(QIODevice::Append))
    {
        return false;
    }

    return true;
}

bool IRtRecord::readFile(const QString& filename)
{
    QFile tmp(filename);
    if(!tmp.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QDataStream in(&tmp);
    in.setVersion(QDataStream::Qt_5_2);
    in.setByteOrder(QDataStream::LittleEndian);

    while(!in.atEnd())
    {
        quint16 crc;
        QByteArray data;
        in >> crc >> data;

        if(qChecksum(data.data(), data.size()) != crc)
        {
            return false;
        }

        readEntry(data);
    }

    tmp.close();
    return true;
}

bool IRtRecord::writeEntry(const QByteArray& data)
{
    if(!file.isOpen())
    {
        return false;
    }

    quint16 crc = qChecksum(data.data(), data.size());
    stream << crc << data;
    file.flush();
    return true;
}
