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

#include "map/CMapTMS.h"
#include "map/CMapDraw.h"
#include "map/cache/CDiskCache.h"

#include <QtWidgets>
#include <QtNetwork>

CMapTMS::CMapTMS(const QString &filename, CMapDraw *parent)
    : IMap(eFeatVisibility|eFeatTileCache, parent)
    , diskCache(0)
    , lastRequest(false)
{

    // create default cache path from filename
    QFileInfo fi(filename);
    slotSetCachePath(QDir::home().absoluteFilePath(".QMapShack/" + fi.baseName()));

    accessManager   = new QNetworkAccessManager(parent->thread());
    connect(this, SIGNAL(sigQueueChanged()), this, SLOT(slotQueueChanged()));
    connect(accessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(slotRequestFinished(QNetworkReply*)));

    name = fi.baseName().replace("_", " ");

    isActivated = true;
}

CMapTMS::~CMapTMS()
{

}

void CMapTMS::getLayers(QListWidget& list)
{

}

void CMapTMS::configureCache()
{
    delete diskCache;
    diskCache = new CDiskCache(getCachePath(), getCacheSize(), getCacheExpiration(), this);
}

void CMapTMS::slotQueueChanged()
{
    QMutexLocker lock(&mutex);

    if(!urlQueue.isEmpty() && urlPending.size() < 6)
    {
        // request up to 6 pending request
        for(int i = 0; i < (6 - urlPending.size()); i++)
        {
            QString url = urlQueue.dequeue();
            lastRequest = urlQueue.isEmpty();

            QNetworkRequest request;
            request.setUrl(url);
            accessManager->get(request);
            urlPending << url;

            if(lastRequest)
            {
                break;
            }
        }
    }
    else if(lastRequest && urlPending.isEmpty())
    {
        lastRequest = false;
        // if all tiles are received the map layer can be redrawn with all tiles from cache
        map->emitSigCanvasUpdate();
    }

    // report status of pending tiles
    int pending = urlQueue.size() + urlPending.size();
    if(pending)
    {
        map->reportStatusToCanvas(name, tr("<b>%1</b>: %2 tiles pending<br/>").arg(name).arg(pending));
    }
    else
    {
        map->reportStatusToCanvas(name, "");
    }
}

void CMapTMS::slotRequestFinished(QNetworkReply* reply)
{
    QString url = reply->url().toString();
    if(urlPending.contains(url))
    {
        QImage img;
        // only take good responses
        if(!reply->error())
        {
            // read image data
            img.loadFromData(reply->readAll());

        }
        // always store image to cache, the cache will take care of NULL images
        diskCache->store(url, img);

        urlPending.removeAll(url);
    }

    // debug output any error
    if(reply->error())
    {
        qDebug() << reply->errorString();
    }

    // delete reply object
    reply->deleteLater();

    // check for more items to be queued
    slotQueueChanged();
}

void CMapTMS::slotLayersChanged(QListWidgetItem * item)
{

}


void CMapTMS::draw(IDrawContext::buffer_t& buf)
{

}
