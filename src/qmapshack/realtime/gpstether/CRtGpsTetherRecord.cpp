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

#include "CRtGpsTetherRecord.h"

CRtGpsTetherRecord::CRtGpsTetherRecord(QObject *parent)
    : IRtRecord(parent)
{
}

void CRtGpsTetherRecord::reset()
{
}

bool CRtGpsTetherRecord::setFile(const QString& filename)
{
    return false;
}

bool CRtGpsTetherRecord::writeEntry(qreal lon, qreal lat, qreal ele, const QDateTime& timestamp)
{
    return false;
}

void CRtGpsTetherRecord::draw(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt)
{
}

bool CRtGpsTetherRecord::readEntry(QByteArray& data)
{
    return false;
}
