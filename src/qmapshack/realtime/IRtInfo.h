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

#ifndef IRTINFO_H
#define IRTINFO_H

#include <realtime/IRtRecord.h>
#include <realtime/IRtSource.h>

#include <QPointer>
#include <QWidget>

class CTrackData;

class IRtInfo : public QWidget
{
    Q_OBJECT
public:
    IRtInfo(IRtSource* source, QWidget* parent);
    virtual ~IRtInfo() = default;

    virtual void draw(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw* rt);

protected slots:
    void slotSetFilename();
    void slotResetRecord();
    void slotToTrack();

protected:
    virtual void startRecord(const QString& filename) = 0;
    virtual void fillTrackData(CTrackData& data) = 0;

    QPointer<IRtSource> source;
    QPointer<IRtRecord> record;
};

#endif //IRTINFO_H

