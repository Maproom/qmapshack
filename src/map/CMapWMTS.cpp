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

#include "CMapWMTS.h"

#include <QtWidgets>
#include <QtXml>

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


        QString Identifier = xmlTileMatrixSet.namedItem("Identifier").toElement().text();
        tilesets[Identifier] = tileset_t();
        tileset_t& tileset = tilesets[Identifier];

        QString str = xmlTileMatrixSet.namedItem("SupportedCRS").toElement().text();

        char * ptr = str.toLatin1().data();
        OGRSpatialReference oSRS;
        oSRS.importFromURN(ptr);
        oSRS.exportToProj4(&ptr);

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
            const QDomNode& xmlTileMatrix = xmlTileMatrixN.at(n);
            qDebug() << xmlTileMatrix.namedItem("Identifier").toElement().text();
        }

    }

    isActivated = true;
}

CMapWMTS::~CMapWMTS()
{

}

void CMapWMTS::draw(IDrawContext::buffer_t& buf)
{

}
