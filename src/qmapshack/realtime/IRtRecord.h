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

#ifndef IRTRECORD_H
#define IRTRECORD_H

#include <QDataStream>
#include <QFile>
#include <QObject>

class CRtDraw;
class QPainter;

class IRtRecord : public QObject
{
    Q_OBJECT
public:
    IRtRecord(QObject * parent);
    virtual ~IRtRecord() = default;

    virtual bool setFile(const QString& filename);
    virtual void draw(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt)
    {
    }

protected:
    virtual bool writeEntry(const QByteArray& data);
    virtual bool readEntry(QByteArray& data) = 0;

private:
    virtual bool readFile(const QString& filename);

    QFile file;
};

#endif //IRTRECORD_H

