
/*
 * Map Driver Implementation for GEMF Maps
 * SPECs --> http://www.cgtk.co.uk/gemf
 *
 *
*/

#include "CMainWindow.h"
#include "helpers/CDraw.h"
#include "map/CMapDraw.h"
#include "map/CMapGEMF.h"
#include "units/IUnit.h"

#include <QDebug>
#include <QtGui>
#include <QtWidgets>

inline int lon2tile(double lon, int z)
{
    return (int)(qRound(256*(lon + 180.0) / 360.0 * qPow(2.0, z)));
}

inline int lat2tile(double lat, int z)
{
    return (int)(qRound(256*(1.0 - log( qTan(lat * M_PI/180.0) + 1.0 / qCos(lat * M_PI/180.0)) / M_PI) / 2.0 * qPow(2.0, z)));
}

inline double tile2lon(int x, int z)
{
    return x / qPow(2.0, z) * 360.0 - 180;
}

inline double tile2lat(int y, int z)
{
    double n = M_PI - 2.0 * M_PI * y / qPow(2.0, z);
    return 180.0 / M_PI * qAtan(0.5 * (exp(n) - exp(-n)));
}

CMapGEMF::CMapGEMF(const QString &filename, CMapDraw *parent)
    : IMap(eFeatVisibility,parent)
    , filename(filename)
{
    qDebug() << "CMapGEMF: try to open " << filename;
    pjsrc = pj_init_plus("+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs");
    //qDebug() << "CMapGEMF:" << "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs";
    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::BigEndian);

    stream >> version ;
    stream >> tileSize;
    stream >> sourceNr;

    quint32 i;

    for(i=0; i<sourceNr; i++)
    {
        source_t source ;
        quint32 len;
        QByteArray name(NAMEBUFLEN,0);
        stream >> source.index ;
        stream >>len ;
        if(len>NAMEBUFLEN) qDebug() << "CMapGEMF: Name longer than 1024 byte";
        len = (len>NAMEBUFLEN)?NAMEBUFLEN:len;
        stream.readRawData(name.data(),len);
        source.name = QString().fromLocal8Bit(name,len);
        sources << source;
        qDebug() << "CMapGEMF: Read Source " << source.name;

    }
    qDebug() << "CMapGEMF: Read "<< i << "Sources";
    stream >> rangeNum;
    QList<range_t> ranges;
    quint64 tiles=0;
    for (i=0; i<rangeNum;i++)
    {
           range_t range;
           stream >> range.zoomlevel;
           stream >> range.minX;
           stream >> range.maxX;
           stream >> range.minY;
           stream >> range.maxY;
           stream >> range.sourceIdx;
           stream >> range.offset;

           ranges << range;
           tiles += (range.maxX +1 -range.minX)*(range.maxY+1-range.minY);
    }
    qDebug() << "CMapGEMF: Read "<< i << "Ranges with " << tiles << " Tiles" ;

    for(i=0;i<21;i++){
        QList<range_t> rangeZoom;
        QList<range_t>::iterator it;
        for(it=ranges.begin();it!=ranges.end(); it++){
            if(it->zoomlevel ==i) {
                rangeZoom << *it;
                minZoom = (i<minZoom)?i:minZoom;
                maxZoom = (i>maxZoom)?i:maxZoom;
            }
        }
        if(rangeZoom.length()!= 0) {
            RangesByZoom[i] = rangeZoom;
            qDebug() << "CMapGEMF: Found " << rangeZoom.length() << "ranges for " << " Zoomlevel " << i;
        }

    }
    QString partfile = filename;
    QFile f(partfile);
    f.open(QIODevice::ReadOnly);
    i=1;
    do{

        ;
        gemffile_t gf;
        gf.filename= partfile;
        gf.size = f.size();
        f.close();
        files << gf;

        partfile = filename + "-" + QString::number(i);
        i++;
        f.setFileName(partfile);

    }while(f.open(QIODevice::ReadOnly));

    isActivated = true;

}

void CMapGEMF::draw(IDrawContext::buffer_t &buf)
{
    if(map->needsRedraw())
    {
        return;
    }

    QPointF bufferScale = buf.scale * buf.zoomFactor;
    if(isOutOfScale(bufferScale))
    {
        return;
    }
    QPointF pp = buf.ref1;
    map->convertRad2Px(pp);

    // start to draw the map
    QPainter p(&buf.image);
    USE_ANTI_ALIASING(p,true);
    p.setOpacity(getOpacity()/100.0);
    p.translate(-pp);

    qreal x1 = buf.ref1.x() < buf.ref4.x() ? buf.ref1.x() : buf.ref4.x();
    qreal y1 = buf.ref1.y() > buf.ref2.y() ? buf.ref1.y() : buf.ref2.y();

    qreal x2 = buf.ref2.x() > buf.ref3.x() ? buf.ref2.x() : buf.ref3.x();
    qreal y2 = buf.ref3.y() < buf.ref4.y() ? buf.ref3.y() : buf.ref4.y();

    if(x1 < -180.0*DEG_TO_RAD)
    {
        x1 = -180*DEG_TO_RAD;
    }
    if(x2 >  180.0*DEG_TO_RAD)
    {
        x2 =  180*DEG_TO_RAD;
    }

    quint32 z    = 20;
    QPointF s1  = buf.scale * buf.zoomFactor;
    qreal d     = NOFLOAT;

    for(quint32 i = minZoom; i < 21; i++)
    {
        qreal s2 = 0.055 * (1<<i);
        if(qAbs(s2 - s1.x()) < d)
        {
            z = i;
            d = qAbs(s2 - s1.x());
        }
    }

    z = 21 - z;

    if(z > maxZoom)
    {

        return;
    }



    qint32 row1, row2, col1, col2;

    col1 = lon2tile(x1 * RAD_TO_DEG, z) / 256;
    col2 = lon2tile(x2 * RAD_TO_DEG, z) / 256;
    row1 = lat2tile(y1 * RAD_TO_DEG, z) / 256;
    row2 = lat2tile(y2 * RAD_TO_DEG, z) / 256;
    for(qint32 row = row1; row <= row2; row++)
    {
        for(qint32 col = col1; col <= col2; col++)
        {
            QPolygonF l;

            qreal xx1 = tile2lon(col, z) * DEG_TO_RAD;
            qreal yy1 = tile2lat(row, z) * DEG_TO_RAD;
            qreal xx2 = tile2lon(col + 1, z) * DEG_TO_RAD;
            qreal yy2 = tile2lat(row + 1, z) * DEG_TO_RAD;

            l << QPointF(xx1, yy1) << QPointF(xx2, yy1) << QPointF(xx2, yy2) << QPointF(xx1, yy2);

            QImage img = getTile(col,row,z);
            drawTile(img, l, p);
        }
    }


}

quint64 CMapGEMF::getFilenameFromAddress(quint64 offset,QString &filename)
{

    quint64 temp = offset;

    foreach (gemffile_t gf, files) {
        if(temp < gf.size)
        {
            filename = gf.filename;

            return temp;
        }else temp -= gf.size;

    }

    qDebug() << "CMAPGemf: ImageAddress was wrong "<< offset;
    return 0;  //

}

QImage CMapGEMF::getTile(quint32 row, quint32 col, quint32 z)
{
        if(z<minZoom || z> maxZoom)
        {
            qDebug() << "CMapGEMF: getTile out of Zoomlevels";
            return QImage();
        }
        QList<range_t> ranges = RangesByZoom[z];

        foreach (range_t r,ranges)
        {
                if(    row >= r.minX
                    && row <= r.maxX
                    && col >= r.minY
                    && col <= r.maxY)
                {
                    quint32 nrYVals, Xidx,Yidx,TileIdx;
                    quint64 offsetRange, offsetGEMF;
                    Xidx = row-r.minX;
                    Yidx = col-r.minY;
                    nrYVals = r.maxY + 1 - r.minY ;
                    TileIdx = Xidx*nrYVals + Yidx;
                    offsetRange = TileIdx*12; // 4 + 8
                    offsetGEMF = offsetRange +r.offset;
                    QString splitfile;
                    offsetGEMF = getFilenameFromAddress(offsetGEMF, splitfile);
                    QFile file(splitfile);
                    file.open(QIODevice::ReadOnly);
                    QDataStream dataFile(&file);
                    quint32 size;
                    quint64 imageDataAddress, imageDataOffset;
                    dataFile.skipRawData(offsetGEMF);
                    dataFile >> imageDataAddress;
                    dataFile >> size;
                    file.close();
                    imageDataOffset = getFilenameFromAddress(imageDataAddress, splitfile);
                    QFile imageFile(splitfile);
                    imageFile.open(QIODevice::ReadOnly);
                    imageFile.seek(imageDataOffset);
                    QByteArray imageData(size,0);
                    imageFile.read(imageData.data(),size);
                    return QImage::fromData((uchar *)imageData.data(),size,0)   ;


                }

        }
 return QImage();
}
