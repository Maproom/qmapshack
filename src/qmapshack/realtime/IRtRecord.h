/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef IRTRECORD_H
#define IRTRECORD_H

#include "gis/trk/CTrackData.h"

#include <QDataStream>
#include <QFile>
#include <QObject>

class CRtDraw;
class QPainter;

class IRtRecord : public QObject
{
    Q_OBJECT
public:
    IRtRecord(QObject* parent);
    virtual ~IRtRecord() = default;

    /**
       @brief Set record file size to 0.
     */
    virtual void reset();
    /**
       @brief Set file name to record into

       If the file exists this will read the file and append new data.

       @param fn  the filename as string

       @return Return true on success.
     */
    virtual bool setFile(const QString& fn);

    virtual const QString& getError() const
    {
        return error;
    }

    /**
       @brief Draw the record data into the draw context

       This does nothing and you have to override it.

       @param p             the paint device
       @param viewport      the visible viewport
       @param blockedAreas  a list of blocked areas
       @param rt            the draw context
     */
    virtual void draw(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw* rt);

    virtual const QVector<CTrackData::trkpt_t>& getTrack() const
    {
        return track;
    }

protected:
    /**
       @brief Write block of data to file

       A crc16 is calculated and stored together with the byte array into the file.

       @param data  the byte array to store

       @return Return true on success.
     */
    virtual bool writeEntry(const QByteArray& data);

    /**
       @brief A block data has been read and needs further processing

       If a filename is set and the file exists, the data entries in the file are read
       one by one and passed to this API. If the method returns with false the file will
       be truncated to the last valid entry.

       @param data  the byte array with the data entry.

       @return Return true on success.
     */
    virtual bool readEntry(QByteArray& data);

protected:
    QVector<CTrackData::trkpt_t> track;

private:

    /**
       @brief Reads file content entry by entry and tests for the checksum

       @param filename  the file name to open and read.

       @return Return true on success.
     */
    virtual bool readFile(const QString& filename);

    QString filename;

    QString error;
};

#endif //IRTRECORD_H

