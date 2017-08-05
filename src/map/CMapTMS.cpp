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

#include "CMainWindow.h"
#include "helpers/CDraw.h"
#include "map/CMapDraw.h"
#include "map/CMapTMS.h"
#include "map/cache/CDiskCache.h"
#include "units/IUnit.h"

#include <QtNetwork>
#include <QtScript>
#include <QtWidgets>
#include <QtXml>

#include <ogr_spatialref.h>
#include <proj_api.h>

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

CMapTMS::CMapTMS(const QString &filename, CMapDraw *parent)
    : IMapOnline(parent)
{
    qDebug() << "------------------------------";
    qDebug() << "TMS: try to open" << filename;

    pjsrc = pj_init_plus("+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs");
    qDebug() << "tms:" << "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs";


    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to open %1").arg(filename), QMessageBox::Abort, QMessageBox::Abort);
        return;
    }

    QString msg;
    int line, column;
    QDomDocument dom;
    if(!dom.setContent(&file, true, &msg, &line, &column))
    {
        file.close();
        QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to read: %1\nline %2, column %3:\n %4").arg(filename).arg(line).arg(column).arg(msg), QMessageBox::Abort, QMessageBox::Abort);
        return;
    }
    file.close();

    const QDomElement& xmlTms =  dom.firstChildElement("TMS");
    name        = xmlTms.firstChildElement("Title").text();
    copyright   = xmlTms.firstChildElement("Copyright").text();

    if(xmlTms.firstChildElement("MaxZoomLevel").isElement())
    {
        maxZoomLevel = xmlTms.firstChildElement("MaxZoomLevel").text().toInt();
    }

    if(xmlTms.firstChildElement("MinZoomLevel").isElement())
    {
        minZoomLevel = xmlTms.firstChildElement("MinZoomLevel").text().toInt();
    }

    const QDomNodeList& xmlLayers = xmlTms.elementsByTagName("Layer");
    qint32 N = xmlLayers.count();
    layers.resize(N);

    for(qint32 n = 0; n < N; ++n)
    {
        const QDomNode& xmlLayer = xmlLayers.item(n);
        int idx = xmlLayer.attributes().namedItem("idx").nodeValue().toInt();
        layers[idx].strUrl       = xmlLayer.namedItem("ServerUrl").toElement().text();
        layers[idx].script       = xmlLayer.namedItem("Script").toElement().text();
        layers[idx].minZoomLevel = minZoomLevel;
        layers[idx].maxZoomLevel = maxZoomLevel;

        if(xmlLayer.namedItem("Title").isElement())
        {
            layers[idx].title = xmlLayer.namedItem("Title").toElement().text();
        }
        else
        {
            layers[idx].title = tr("Layer %1").arg(idx + 1);
        }

        if(xmlLayer.firstChildElement("MinZoomLevel").isElement())
        {
            layers[idx].minZoomLevel = xmlLayer.firstChildElement("MinZoomLevel").text().toInt();
        }

        if(xmlLayer.firstChildElement("MaxZoomLevel").isElement())
        {
            layers[idx].maxZoomLevel = xmlLayer.firstChildElement("MaxZoomLevel").text().toInt();
        }

        if(!httpsCheck(layers[idx].strUrl))
        {
            return;
        }
    }

    const QDomElement& xmlRawHeader = xmlTms.firstChildElement("RawHeader");
    const QDomNodeList& xmlValues   = xmlRawHeader.elementsByTagName("Value");
    N = xmlValues.count();
    for(qint32 n = 0; n < N; ++n)
    {
        const QDomNode& xmlValue = xmlValues.item(n);
        registerHeaderItem(
            xmlValue.attributes().namedItem("name").nodeValue(),
            xmlValue.toElement().text()
        );
    }

    // if there is more than one layer the layer list in the properties widget has to be enabled.
    if(layers.size() > 1)
    {
        flagsFeature |= eFeatLayers;
    }


    // create default cache path from filename
    QFileInfo fi(filename);
    slotSetCachePath(QDir(CMapDraw::getCacheRoot()).absoluteFilePath(fi.completeBaseName()));

    name = fi.completeBaseName().replace("_", " ");

    isActivated = true;
}

void CMapTMS::getLayers(QListWidget& list) /* override */
{
    QMutexLocker lock(&mutex);

    list.clear();
    if(layers.size() < 2)
    {
        return;
    }

    int i = 0;
    for(const layer_t &layer : layers)
    {
        QListWidgetItem * item = new QListWidgetItem(layer.title, &list);
        item->setCheckState(layer.enabled ? Qt::Checked : Qt::Unchecked);
        item->setData(Qt::UserRole, i++);
    }

    connect(&list, &QListWidget::itemChanged, this, &CMapTMS::slotLayersChanged);
}


void CMapTMS::saveConfig(QSettings& cfg) /* override */
{
    QMutexLocker lock(&mutex);

    IMapOnline::saveConfig(cfg);
    if(layers.size() < 2)
    {
        return;
    }

    // save indices of enabled layers
    QStringList enabled;
    for(int i = 0; i< layers.size(); i++)
    {
        if(layers[i].enabled)
        {
            enabled << QString::number(i);
        }
    }
    cfg.setValue("enabledLayers", enabled);
}

void CMapTMS::loadConfig(QSettings& cfg)
{
    QMutexLocker lock(&mutex);

    IMapOnline::loadConfig(cfg);
    if(layers.size() < 2)
    {
        return;
    }

    QStringList enabled;
    // set all layers to disabled first
    for(int i = 0; i< layers.size(); i++)
    {
        layers[i].enabled = false;
        enabled << QString::number(i);
    }

    // enable layers stored in configuration
    enabled = cfg.value("enabledLayers", enabled).toStringList();
    for(const QString &str : enabled)
    {
        int idx = str.toInt();
        if(idx < layers.size())
        {
            layers[idx].enabled = true;
        }
    }
}


void CMapTMS::slotLayersChanged(QListWidgetItem * item)
{
    QMutexLocker lock(&mutex);

    bool isChecked = (item->checkState() == Qt::Checked);
    int idx = item->data(Qt::UserRole).toInt();
    if(idx < 0)
    {
        QListWidget * list = item->listWidget();
        list->blockSignals(true);

        for(int i = 0; i < layers.size(); i++)
        {
            list->item(i + 1)->setCheckState(isChecked ? Qt::Checked : Qt::Unchecked);
            layers[i].enabled = isChecked;
        }

        list->blockSignals(false);
    }
    else
    {
        layers[idx].enabled = isChecked;
    }

    map->emitSigCanvasUpdate();
}


QString CMapTMS::createUrl(const layer_t& layer, int x, int y, int z)
{
    QMutexLocker lock(&mutex);

    if(layer.strUrl.startsWith("script"))
    {
        QString filename = layer.strUrl.mid(9);
        QFile scriptFile(filename);
        if (!scriptFile.open(QIODevice::ReadOnly))
        {
            return "";
        }
        QTextStream stream(&scriptFile);
        QString contents = stream.readAll();
        scriptFile.close();

        QScriptEngine engine;
        QScriptValue fun = engine.evaluate(contents, filename);

        if(engine.hasUncaughtException())
        {
            int line = engine.uncaughtExceptionLineNumber();
            qDebug() << "uncaught exception at line" << line << ":" << fun.toString();
        }

        QScriptValueList args;
        args << z << x << y;
        QScriptValue res = fun.call(QScriptValue(), args);
        return res.toString();
    }
    else if(!layer.script.isEmpty())
    {
        QScriptEngine engine;
        QScriptValue fun = engine.evaluate(layer.script);
        QScriptValueList args;
        args << z << x << y;
        QScriptValue res = fun.call(QScriptValue(), args);
        return res.toString();
    }

    return layer.strUrl.arg(z).arg(x).arg(y);
}


void CMapTMS::draw(IDrawContext::buffer_t& buf) /* override */
{
    QMutexLocker lock(&mutex);

    timeLastUpdate.start();
    urlQueue.clear();

    if(map->needsRedraw())
    {
        return;
    }

    QPointF bufferScale = buf.scale * buf.zoomFactor;

    if(isOutOfScale(bufferScale))
    {
        return;
    }

    // get pixel offset of top left buffer corner
    QPointF pp = buf.ref1;
    map->convertRad2Px(pp);

    // start to draw the map
    QPainter p(&buf.image);
    USE_ANTI_ALIASING(p,true);
    p.setOpacity(getOpacity()/100.0);
    p.translate(-pp);

    // calculate maximum viewport
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

    // draw layers
    for(const layer_t &layer : layers)
    {
        if(!layer.enabled)
        {
            continue;
        }

        qint32 z    = 20;
        QPointF s1  = buf.scale * buf.zoomFactor;
        qreal d     = NOFLOAT;

        for(qint32 i = layer.minZoomLevel; i < 21; i++)
        {
            qreal s2 = 0.055 * (1<<i);
            if(qAbs(s2 - s1.x()) < d)
            {
                z = i;
                d = qAbs(s2 - s1.x());
            }
        }

        if(z > layer.maxZoomLevel)
        {
            continue;
        }

        z = 21 - z;


        qint32 row1, row2, col1, col2;

        col1 = lon2tile(x1 * RAD_TO_DEG, z) / 256;
        col2 = lon2tile(x2 * RAD_TO_DEG, z) / 256;
        row1 = lat2tile(y1 * RAD_TO_DEG, z) / 256;
        row2 = lat2tile(y2 * RAD_TO_DEG, z) / 256;

//        qDebug() << col1 << col2 << row1 << row2 << (col2 - col1) << (row2 - row1) << ((col2 - col1) * (row2 - row1));

        // start to request tiles. draw tiles in cache, queue urls of tile yet to be requested
        for(qint32 row = row1; row <= row2; row++)
        {
            for(qint32 col = col1; col <= col2; col++)
            {
                QString url = createUrl(layer, col, row, z);
//                qDebug() << url;

                if(diskCache->contains(url))
                {
                    QImage img;
                    diskCache->restore(url, img);

                    QPolygonF l;

                    qreal xx1 = tile2lon(col, z) * DEG_TO_RAD;
                    qreal yy1 = tile2lat(row, z) * DEG_TO_RAD;
                    qreal xx2 = tile2lon(col + 1, z) * DEG_TO_RAD;
                    qreal yy2 = tile2lat(row + 1, z) * DEG_TO_RAD;

                    l << QPointF(xx1, yy1) << QPointF(xx2, yy1) << QPointF(xx2, yy2) << QPointF(xx1, yy2);
                    drawTile(img, l, p);
                }
                else
                {
                    urlQueue << url;
                }
            }
        }

        emit sigQueueChanged();
    }
}
