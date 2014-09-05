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

#include "map/CMapWMTS.h"
#include "map/CMapDraw.h"
#include "units/IUnit.h"


#include <QtWidgets>
#include <QtXml>
#include <QtNetwork>

#include <ogr_spatialref.h>
#include <proj_api.h>

CMapWMTS::CMapWMTS(const QString &filename, CMapDraw *parent)
    : IMap(eFeatVisibility, parent)
{
    qDebug() << "------------------------------";
    qDebug() << "WTMS: try to open" << filename;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, tr("Error..."), tr("Failed to open %1").arg(filename), QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    QString msg;
    int line, column;
    QDomDocument dom;
    if(!dom.setContent(&file, true, &msg, &line, &column))
    {
        file.close();
        QMessageBox::critical(0, tr("Error..."), tr("Failed to read: %1\nline %2, column %3:\n %4").arg(filename).arg(line).arg(column).arg(msg), QMessageBox::Abort, QMessageBox::Abort);
        return;
    }
    file.close();

    const QDomElement& xmlCapabilities = dom.documentElement();
    if(xmlCapabilities.tagName() != "Capabilities")
    {
        QMessageBox::critical(0, tr("Error..."), tr("Failed to read: %1\nUnknown structure.").arg(filename), QMessageBox::Abort, QMessageBox::Abort);
        return;
    }
    const QDomNode& xmlServiceIdentification = xmlCapabilities.namedItem("ServiceIdentification");
    QString ServiceType         = xmlServiceIdentification.firstChildElement("ServiceType").text();
    QString ServiceTypeVersion  = xmlServiceIdentification.firstChildElement("ServiceTypeVersion").text();

    if(ServiceType != "OGC WMTS" || ServiceTypeVersion != "1.0.0")
    {
        QMessageBox::critical(0, tr("Error..."), tr("Unexpexted service. 'OGC WMTS 1.0.0' is expected. '%1 %2' is read.").arg(ServiceType).arg(ServiceTypeVersion), QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    const QDomNode& xmlContents = xmlCapabilities.namedItem("Contents");
    const QDomNodeList& xmlLayers = xmlContents.toElement().elementsByTagName("Layer");

    const int N = xmlLayers.count();
    for(int n = 0; n < N; n++)
    {
        QString str;
        QStringList values;
        const QDomNode& xmlLayer = xmlLayers.at(n);
        layer_t layer;

        layer.title = xmlLayer.firstChildElement("Title").text();

        // read bounding box
        const QDomNode& xmlBoundingBox = xmlLayer.firstChildElement("WGS84BoundingBox");
        str = xmlBoundingBox.namedItem("LowerCorner").toElement().text();
        values = str.split(" ");
        QPointF bottomLeft(values[0].toDouble(), values[1].toDouble());

        str = xmlBoundingBox.namedItem("UpperCorner").toElement().text();
        values = str.split(" ");
        QPointF topRight(values[0].toDouble(), values[1].toDouble());

        layer.boundingBox.setBottomLeft(bottomLeft);
        layer.boundingBox.setTopRight(topRight);

        const QDomNode& xmlStyle = xmlLayer.firstChildElement("Style");
        layer.styles << xmlStyle.namedItem("Identifier").toElement().text();

        const QDomNode& xmlTileMatrixSetLink = xmlLayer.firstChildElement("TileMatrixSetLink");
        layer.tileMatrixSet = xmlTileMatrixSetLink.namedItem("TileMatrixSet").toElement().text();

        const QDomNode& xmlResourceURL = xmlLayer.firstChildElement("ResourceURL");
        const QDomNamedNodeMap& attr = xmlResourceURL.attributes();

        layer.resourceURL = attr.namedItem("template").nodeValue();
        layer.resourceURL = layer.resourceURL.replace("{Style}",layer.styles[0]);
        layer.resourceURL = layer.resourceURL.replace("{TileMatrixSet}",layer.tileMatrixSet);

        qDebug() << layer.resourceURL;
        layers << layer;
    }

    const QDomNodeList& xmlTileMatrixSets = xmlContents.childNodes();
    const int M = xmlTileMatrixSets.count();

    for(int m = 0; m < M; m++)
    {
        const QDomNode& xmlTileMatrixSet = xmlTileMatrixSets.at(m);
        if(xmlTileMatrixSet.nodeName() != "TileMatrixSet")
        {
            continue;
        }


        QString Identifier      = xmlTileMatrixSet.namedItem("Identifier").toElement().text();
        tilesets[Identifier]    = tileset_t();
        tileset_t& tileset      = tilesets[Identifier];

        QString str = xmlTileMatrixSet.namedItem("SupportedCRS").toElement().text();

        char * ptr = str.toLatin1().data();
        OGRSpatialReference oSRS;
        oSRS.importFromURN(ptr);
        oSRS.exportToProj4(&ptr);

        qDebug() << ptr;
        tileset.pjsrc = pj_init_plus(ptr);
        if(tileset.pjsrc == 0)
        {
            QMessageBox::warning(0, tr("Error..."), tr("No georeference information found."));
            return;
        }

        const QDomNodeList& xmlTileMatrixN = xmlTileMatrixSet.toElement().elementsByTagName("TileMatrix");
        const int N = xmlTileMatrixN.count();
        for(int n = 0; n < N; n++)
        {
            QString str;
            QStringList values;
            const QDomNode& xmlTileMatrix = xmlTileMatrixN.at(n);
            QString Identifier =  xmlTileMatrix.namedItem("Identifier").toElement().text();
            tileset.tilematrix[Identifier] = tilematrix_t();
            tilematrix_t& matrix = tileset.tilematrix[Identifier];

            str = xmlTileMatrix.namedItem("TopLeftCorner").toElement().text();
            values = str.split(" ");
            matrix.topLeft      = QPointF(values[0].toDouble(), values[1].toDouble());
            matrix.scale        = xmlTileMatrix.namedItem("ScaleDenominator").toElement().text().toDouble();
            matrix.tileWidth    = xmlTileMatrix.namedItem("TileWidth").toElement().text().toInt();
            matrix.tileHeight   = xmlTileMatrix.namedItem("TileHeight").toElement().text().toInt();
            matrix.matrixWidth  = xmlTileMatrix.namedItem("MatrixWidth").toElement().text().toInt();
            matrix.matrixHeight = xmlTileMatrix.namedItem("MatrixHeight").toElement().text().toInt();
        }

    }

    accessManager = new QNetworkAccessManager(parent->thread());

    isActivated = true;
}

CMapWMTS::~CMapWMTS()
{

}

void CMapWMTS::draw(IDrawContext::buffer_t& buf)
{
    if(map->needsRedraw())
    {
        return;
    }

    QPointF bufferScale = buf.scale * buf.zoomFactor;

    qreal x1 = buf.ref1.x() < buf.ref4.x() ? buf.ref1.x() : buf.ref4.x();
    qreal y1 = buf.ref1.y() > buf.ref2.y() ? buf.ref1.y() : buf.ref2.y();

    qreal x2 = buf.ref2.x() > buf.ref3.x() ? buf.ref2.x() : buf.ref3.x();
    qreal y2 = buf.ref3.y() < buf.ref4.y() ? buf.ref3.y() : buf.ref4.y();


    QRectF viewport(QPointF(x1,y1) * RAD_TO_DEG, QPointF(x2,y2) * RAD_TO_DEG);

    foreach(const layer_t& layer, layers)
    {
        qDebug() << layer.boundingBox << viewport;
        if(!layer.boundingBox.intersects(viewport))
        {
            continue;
        }

        const tileset_t& tileset = tilesets[layer.tileMatrixSet];

        QPointF pt1(x1,y1);
        QPointF pt2(x2,y2);

        pj_transform(pjtar, tileset.pjsrc, 1, 0, &pt1.rx(), &pt1.ry(), 0);
        pj_transform(pjtar, tileset.pjsrc, 1, 0, &pt2.rx(), &pt2.ry(), 0);

        QPointF s1 = (pt2 - pt1)/QPointF(buf.image.width(), buf.image.height());

//        qDebug() << pt1 << pt2  << s1;

        QString tileMatrixId;
        qreal d = NOFLOAT;
        foreach(const QString& key, tileset.tilematrix.keys())
        {
            const tilematrix_t& tilematrix = tileset.tilematrix[key];
            qreal s2 = tilematrix.scale * 0.28e-3;


            if(qAbs(s2 - s1.x()) < d)
            {
                tileMatrixId = key;
                d = qAbs(s2 - s1.x());
            }
        }

        qDebug() << tileMatrixId << s1 << tileset.tilematrix[tileMatrixId].scale* 0.28e-3;

        const tilematrix_t& tilematrix = tileset.tilematrix[tileMatrixId];
        quint32 col = qFloor(( pt1.x() - tilematrix.topLeft.x()) / (tilematrix.scale * 0.28e-3 * tilematrix.tileWidth));
        quint32 row = qFloor((-pt1.y() + tilematrix.topLeft.y()) / (tilematrix.scale * 0.28e-3 * tilematrix.tileHeight));

        QString url = layer.resourceURL;
        url = url.replace("{TileMatrix}",tileMatrixId);
        url = url.replace("{TileRow}",QString::number(row));
        url = url.replace("{TileCol}",QString::number(col));

        qDebug() << url;
        QNetworkRequest request(url);

        QNetworkReply * reply = accessManager->get(request);

        while(reply->isRunning())
        {
            qApp->processEvents();
        }

        QImage img;
        img.loadFromData(reply->readAll());

        img.save("test.png");
    }

}
