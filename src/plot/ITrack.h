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

#ifndef ITRACK_H
#define ITRACK_H

#include <QImage>
#include <QPolygonF>
#include <proj_api.h>

class QRectF;
class QPainter;
class CGisItemTrk;

class ITrack
{
public:
    ITrack();
    virtual ~ITrack();

    void setSize(int w, int h);
    void setTrack(CGisItemTrk * track);
    void setTrack(const QPolygonF &track);

    void save(QImage& image);

protected:
    void setupProjection(const QRectF &boundingBox);
    void updateData();
    void draw(QPainter& p);
    void draw();

    projPJ pjsrc = 0;
    projPJ pjtar = 0;

    bool needsRedraw =true;
    CGisItemTrk * trk = nullptr;
    QPolygonF coords;
    QPolygonF line;

    QImage buffer;

    QPointF scale;
    qint32 xoff = 0;
    qint32 yoff = 0;
};

#endif //ITRACK_H

