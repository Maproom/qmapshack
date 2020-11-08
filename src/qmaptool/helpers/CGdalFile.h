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

#ifndef CGDALFILE_H
#define CGDALFILE_H


#include <proj_api.h>
#include <QCoreApplication>
#include <QPointF>
#include <QRgb>
#include <QTransform>
#include <QVector>

class GDALDataset;

class CGdalFile
{
    Q_DECLARE_TR_FUNCTIONS(CGdalFile)
public:
    enum type_e
    {
        eTypePixel,
        eTypeProj
    };

    CGdalFile(type_e type);
    virtual ~CGdalFile() = default;

    bool getIsValid() const
    {
        return isValid;
    }

    QString getProjection() const;

protected:
    virtual QString getInfo() const;
    virtual void load(const QString& filename);
    virtual void unload();

    type_e type;

    GDALDataset * dataset = nullptr;

    /// number of color bands used by the *vrt
    int rasterBandCount = 0;
    /// QT representation of the vrt's color table
    QVector<QRgb> colortable;

    // true if the map file has overviews
    qint32 hasOverviews = -1;
    qint32 hasNoData    = -1;


    /// true if the map file could be loaded
    bool isValid = false;

    /// width in number of px
    qreal xsize_px = 0;
    /// height in number of px
    qreal ysize_px = 0;

    /// scale [px/m]
    qreal xscale = 0;
    /// scale [px/m]
    qreal yscale = 0;

    qreal xrot = 0;
    qreal yrot = 0;

    QPointF ref1;
    QPointF ref2;
    QPointF ref3;
    QPointF ref4;

    QTransform trFwd;
    QTransform trInv;

    QTransform trFwdProj;
    QTransform trInvProj;

    QString proj4str;

    projPJ pjsrc = nullptr;
};

#endif //CGDALFILE_H

