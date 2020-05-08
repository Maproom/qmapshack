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

#include "CMainWindow.h"
#include "helpers/CDraw.h"
#include "map/CMapDraw.h"
#include "map/CMapRMAP.h"
#include "units/IUnit.h"

#include <QtGui>
#include <QtWidgets>

CMapRMAP::CMapRMAP(const QString &filename, CMapDraw *parent)
    : IMap(eFeatVisibility, parent)
    , filename(filename)
{
    qDebug() << "------------------------------";
    qDebug() << "RMAP: try to open" << filename;


    QFile file(filename);
    file.open(QIODevice::ReadOnly);
//    qDebug() << file.errorString();

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    QByteArray charbuf(20, 0);
    stream.readRawData(charbuf.data(), 19);

    if("CompeGPSRasterImage" != QString(charbuf))
    {
        QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("This is not a TwoNav RMAP file."), QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    quint32 tag1, tag2, tmp32;
    stream >> tag1 >> tag2 >> tmp32;

    if(tag1 != 10 || tag2 != 7)
    {
        QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Unknown sub-format."), QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    stream >> xsize_px >> ysize_px;
    stream >> tmp32 >> tmp32;
    stream >> tileSizeX >> tileSizeY;

    ysize_px = -ysize_px;

    quint64 mapDataOffset;
    stream >> mapDataOffset;
    stream >> tmp32;

    qint32 nZoomLevels;
    stream >> nZoomLevels;

    for(int i = 0; i < nZoomLevels; i++)
    {
        level_t level;
        stream >> level.offsetLevel;
        levels << level;
    }

    for(int i = 0; i < levels.size(); i++)
    {
        level_t& level = levels[i];
        file.seek(level.offsetLevel);

        stream >> level.width;
        stream >> level.height;
        stream >> level.xTiles;
        stream >> level.yTiles;

        for(int j = 0; j < (level.xTiles * level.yTiles); j++)
        {
            quint64 offset;
            stream >> offset;
            level.offsetJpegs << offset;
        }
    }

    file.seek(mapDataOffset);
    stream >> tmp32 >> tmp32;

    charbuf.resize(tmp32 + 1);
    charbuf.fill(0);
    stream.readRawData(charbuf.data(), tmp32);

    QPoint p0;
    QPoint p1;
    QPoint p2;
    QPoint p3;
    projXY c0 = {0, 0};
    projXY c1 = {0, 0};
    projXY c2 = {0, 0};
    projXY c3 = {0, 0};

    bool pointsAreLongLat = true;
    QString projection;
    QString datum;
    QStringList lines = QString(charbuf).split("\r\n");

    for(const QString &line : lines)
    {
//        qDebug() << line;
        if(line.startsWith("Version="))
        {
            if(line.split("=")[1] != "2")
            {
                QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Unknown version."), QMessageBox::Abort, QMessageBox::Abort);
                return;
            }
        }
        else if(line.startsWith("Projection="))
        {
            projection = line.split("=")[1];
        }
        else if(line.startsWith("Datum="))
        {
            datum = line.split("=")[1];
        }
        else if(line.startsWith("P0="))
        {
            QStringList vals = line.split("=")[1].split(",");
            if(vals.size() < 5)
            {
                QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to read reference point."), QMessageBox::Abort, QMessageBox::Abort);
                return;
            }

            p0 = QPoint(vals[0].toInt(), vals[1].toInt());
            if(vals[2] == "A")
            {
                c0.u = vals[3].toDouble() * DEG_TO_RAD;
                c0.v = vals[4].toDouble() * DEG_TO_RAD;
            }
            else
            {
                c0.u = vals[3].toDouble();
                c0.v = vals[4].toDouble();
            }
        }
        else if(line.startsWith("P1="))
        {
            QStringList vals = line.split("=")[1].split(",");
            if(vals.size() < 5)
            {
                QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to read reference point."), QMessageBox::Abort, QMessageBox::Abort);
                return;
            }

            p1 = QPoint(vals[0].toInt(), vals[1].toInt());
            if(vals[2] == "A")
            {
                c1.u = vals[3].toDouble() * DEG_TO_RAD;
                c1.v = vals[4].toDouble() * DEG_TO_RAD;
            }
            else
            {
                pointsAreLongLat = false;
                c1.u = vals[3].toDouble();
                c1.v = vals[4].toDouble();
            }
        }
        else if(line.startsWith("P2="))
        {
            QStringList vals = line.split("=")[1].split(",");
            if(vals.size() < 5)
            {
                QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to read reference point."), QMessageBox::Abort, QMessageBox::Abort);
                return;
            }

            p2 = QPoint(vals[0].toInt(), vals[1].toInt());
            if(vals[2] == "A")
            {
                c2.u = vals[3].toDouble() * DEG_TO_RAD;
                c2.v = vals[4].toDouble() * DEG_TO_RAD;
            }
            else
            {
                pointsAreLongLat = false;
                c2.u = vals[3].toDouble();
                c2.v = vals[4].toDouble();
            }
        }
        else if(line.startsWith("P3="))
        {
            QStringList vals = line.split("=")[1].split(",");
            if(vals.size() < 5)
            {
                QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to read reference point."), QMessageBox::Abort, QMessageBox::Abort);
                return;
            }

            p3 = QPoint(vals[0].toInt(), vals[1].toInt());
            if(vals[2] == "A")
            {
                c3.u = vals[3].toDouble() * DEG_TO_RAD;
                c3.v = vals[4].toDouble() * DEG_TO_RAD;
            }
            else
            {
                pointsAreLongLat = false;
                c3.u = vals[3].toDouble();
                c3.v = vals[4].toDouble();
            }
        }
        else
        {
            //            qDebug() << line;
        }
        //qDebug() << line;
    }

    if(!projection.isEmpty() && !datum.isEmpty())
    {
        if(!setProjection(projection, datum))
        {
            QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Unknown projection and datum (%1%2).").arg(projection).arg(datum), QMessageBox::Abort, QMessageBox::Abort);
            return;
        }
    }

    if(!pj_is_latlong(pjsrc))
    {
        if(pointsAreLongLat)
        {
            pj_transform(pjtar, pjsrc, 1, 0, &c0.u, &c0.v, 0);
            pj_transform(pjtar, pjsrc, 1, 0, &c1.u, &c1.v, 0);
            pj_transform(pjtar, pjsrc, 1, 0, &c2.u, &c2.v, 0);
            pj_transform(pjtar, pjsrc, 1, 0, &c3.u, &c3.v, 0);
        }

//        qDebug() << c0.u << c0.v;
//        qDebug() << c1.u << c1.v;
//        qDebug() << c2.u << c2.v;
//        qDebug() << c3.u << c3.v;

        xref1  =  NOFLOAT;
        yref1  = -NOFLOAT;
        xref2  = -NOFLOAT;
        yref2  =  NOFLOAT;
    }
    else
    {
        xref1  =  180 * DEG_TO_RAD;
        yref1  =  -90 * DEG_TO_RAD;
        xref2  = -180 * DEG_TO_RAD;
        yref2  =   90 * DEG_TO_RAD;
    }

    if(c0.u < xref1)
    {
        xref1 = c0.u;
    }
    if(c0.u > xref2)
    {
        xref2 = c0.u;
    }
    if(c1.u < xref1)
    {
        xref1 = c1.u;
    }
    if(c1.u > xref2)
    {
        xref2 = c1.u;
    }
    if(c2.u < xref1)
    {
        xref1 = c2.u;
    }
    if(c2.u > xref2)
    {
        xref2 = c2.u;
    }

    if(c0.v > yref1)
    {
        yref1 = c0.v;
    }
    if(c0.v < yref2)
    {
        yref2 = c0.v;
    }
    if(c1.v > yref1)
    {
        yref1 = c1.v;
    }
    if(c1.v < yref2)
    {
        yref2 = c1.v;
    }
    if(c2.v > yref1)
    {
        yref1 = c2.v;
    }
    if(c2.v < yref2)
    {
        yref2 = c2.v;
    }

    scale.rx() = (xref2 - xref1) / xsize_px;
    scale.ry() = (yref2 - yref1) / ysize_px;

    qreal widthL0  = levels[0].width;
    qreal heightL0 = levels[0].height;

    for(int i = 0; i < levels.size(); i++)
    {
        level_t& level = levels[i];

        level.xscale = scale.x() * widthL0  / level.width;
        level.yscale = scale.y() * heightL0 / level.height;

        //qDebug() << i << level.xscale << level.yscale;
    }

    isActivated = true;

//    qDebug() << "xref1:" << xref1 << "yref1:" << yref1;
//    qDebug() << "xref2:" << xref2 << "yref2:" << yref2;
//    qDebug() << "map  width:" << xsize_px << "height:" << ysize_px;
//    qDebug() << "tile width:" << tileSizeX << "height:" << tileSizeY;
//    qDebug() << "scale x:  " << scale.x() << "y:" << scale.y();
}

bool CMapRMAP::setProjection(const QString& projection, const QString& datum)
{
    QString projstr;
    if(projection.startsWith("0,UTM"))
    {
        QStringList vals    = projection.split(",");
        int zone           = vals[2].toInt();
        bool isSouth        = vals[3] != "N";

        projstr += QString("+proj=utm +zone=%1 %2").arg(zone).arg(isSouth ? "+south" : "");
    }
    if(projection.startsWith("1,"))
    {
        projstr += "+proj=longlat";
    }
    else if(projection.startsWith("2,"))
    {
        projstr += "+proj=merc";
    }
    else if(projection.startsWith("114,"))
    {
        projstr += "+proj=tmerc +lat_0=0 +lon_0=12 +k=1 +x_0=4500000 +y_0=0";
    }
    else if(projection.startsWith("117,"))
    {
        projstr += "+proj=tmerc +lat_0=0 +lon_0=9  +k=1 +x_0=3500000 +y_0=0";
    }

    if(datum == "WGS 84")
    {
        projstr += " +datum=WGS84 +units=m +no_defs";
    }
    else if(datum  == "Potsdam Rauenberg DHDN")
    {
        projstr += " +ellps=bessel +towgs84=606,23,413,0,0,0,0 +units=m +no_defs";
    }

    pjsrc = pj_init_plus(projstr.toLocal8Bit().data());
    if(pjsrc == 0)
    {
        return false;
    }

    char * ptr = pj_get_def(pjsrc, 0);
    qDebug() << "rmap:" << ptr;

    return true;
}

CMapRMAP::level_t& CMapRMAP::findBestLevel(const QPointF& s)
{
    int i = levels.size() - 1;
    if(s.x() < levels[0].xscale)
    {
        return levels[0];
    }
    if(s.x() > levels[i].xscale)
    {
        return levels[i];
    }

    int j = 0;
    qreal dsx = NOFLOAT;
    for(; j < levels.size(); j++)
    {
        level_t& level = levels[j];
        if(qAbs(level.xscale - s.x()) < dsx)
        {
            i = j;
            dsx = qAbs(level.xscale - s.x());
        }
    }

    return levels[i];
}


void CMapRMAP::draw(IDrawContext::buffer_t& buf) /* override */
{
    if(map->needsRedraw())
    {
        return;
    }

    // convert top left buffer corner
    // into buffer's coordinate system
    QPointF pp = buf.ref1;
    map->convertRad2Px(pp);

    // find best level for buffer's scale factor derived from base scale and zoom factor
    QPointF bufferScale = buf.scale * buf.zoomFactor;

    if(isOutOfScale(bufferScale))
    {
        return;
    }

    level_t& level      = findBestLevel(bufferScale);

    // convert top left and bottom right point of buffer to local coord. system
    QPointF p1 = buf.ref1;
    QPointF p2 = buf.ref3;

    convertRad2M(p1);
    convertRad2M(p2);

    // find indices into tile buffer
    int idxx1 = qFloor((p1.x() - xref1) / (level.xscale * tileSizeX));
    int idxy1 = qFloor((p1.y() - yref1) / (level.yscale * tileSizeY));
    int idxx2 =  qCeil((p2.x() - xref1) / (level.xscale * tileSizeX));
    int idxy2 =  qCeil((p2.y() - yref1) / (level.yscale * tileSizeY));

    if(idxx1 < 0)
    {
        idxx1 = 0;
    }
    if(idxx1 >= level.xTiles)
    {
        idxx1 = level.xTiles;
    }
    if(idxx2 < 0)
    {
        idxx2 = 0;
    }
    if(idxx2 >= level.xTiles)
    {
        idxx2 = level.xTiles;
    }

    if(idxy1 < 0)
    {
        idxy1 = 0;
    }
    if(idxy1 >= level.yTiles)
    {
        idxy1 = level.yTiles;
    }
    if(idxy2 < 0)
    {
        idxy2 = 0;
    }
    if(idxy2 >= level.yTiles)
    {
        idxy2 = level.yTiles;
    }

    // ----- start drawing -----
    QPainter p(&buf.image);
    USE_ANTI_ALIASING(p, true);
    p.setOpacity(getOpacity() / 100.0);
    p.translate(-pp);

    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    for(int idxy = idxy1; idxy < idxy2; idxy++)
    {
        if(map->needsRedraw())
        {
            break;
        }

        for(int idxx = idxx1; idxx < idxx2; idxx++)
        {
            if(map->needsRedraw())
            {
                break;
            }

            quint32 tag;
            quint32 len;
            quint64 offset = level.getOffsetJpeg(idxx, idxy);
            file.seek(offset);
            stream >> tag >> len;

            QImage img;
            img.load(&file, "JPG");

            if(img.isNull())
            {
                continue;
            }

            qreal imgw = img.width();
            qreal imgh = img.height();

            // derive tile's corner coordinate
            QPolygonF l(4);
            l[0].rx() = xref1 + idxx * tileSizeX * level.xscale;
            l[0].ry() = yref1 + idxy * tileSizeY * level.yscale;
            l[1].rx() = xref1 + (idxx * tileSizeX + imgw) * level.xscale;
            l[1].ry() = yref1 +  idxy * tileSizeY * level.yscale;
            l[2].rx() = xref1 + (idxx * tileSizeX + imgw) * level.xscale;
            l[2].ry() = yref1 + (idxy * tileSizeY + imgh) * level.yscale;
            l[3].rx() = xref1 +  idxx * tileSizeX * level.xscale;
            l[3].ry() = yref1 + (idxy * tileSizeY + imgh) * level.yscale;

            pj_transform(pjsrc, pjtar, 1, 0, &l[0].rx(), &l[0].ry(), 0);
            pj_transform(pjsrc, pjtar, 1, 0, &l[1].rx(), &l[1].ry(), 0);
            pj_transform(pjsrc, pjtar, 1, 0, &l[2].rx(), &l[2].ry(), 0);
            pj_transform(pjsrc, pjtar, 1, 0, &l[3].rx(), &l[3].ry(), 0);

            drawTile(img, l, p);
        }
    }
}

