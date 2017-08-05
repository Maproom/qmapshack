#include "CMainWindow.h"
#include "map/cache/CDiskCache.h"
#include "map/CMapDraw.h"
#include "map/IMapOnline.h"

#include <QtNetwork>
#include <QMessageBox>

IMapOnline::IMapOnline(CMapDraw * parent)
    : IMap(eFeatVisibility | eFeatTileCache, parent) {

    accessManager = new QNetworkAccessManager(parent->thread());
    connect(accessManager, &QNetworkAccessManager::finished, this, &IMapOnline::slotRequestFinished);

    connect(this, &IMapOnline::sigQueueChanged, this, &IMapOnline::slotQueueChanged);
}

bool IMapOnline::httpsCheck(const QString &url) {
    if(url.toLower().startsWith("https") && !QSslSocket::supportsSsl())
    {
        QString msg = tr(
            "This map requires OpenSSL support. However due to legal restrictions in some countries "
            "OpenSSL is not packaged with QMapShack. You can have a look at the "
            "<a href='https://www.openssl.org/community/binaries.html'>OpenSSL Homepage</a> "
            "for binaries. You have to copy libeay32.dll and ssleay32.dll into the QMapShack program directory."
            );
        QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Error..."), msg, QMessageBox::Abort);
        return false;
    }
    return true;
}


void IMapOnline::slotQueueChanged()
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

    if(timeLastUpdate.elapsed() > 2000)
    {
        timeLastUpdate.start();
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


void IMapOnline::slotRequestFinished(QNetworkReply* reply)
{
    QMutexLocker lock(&mutex);

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
        qDebug() << "Request to" << url << "failed:" << reply->errorString();
    }

    // delete reply object
    reply->deleteLater();

    // check for more items to be queued
    slotQueueChanged();
}


void IMapOnline::configureCache()
{
    QMutexLocker lock(&mutex);

    delete diskCache;
    diskCache = new CDiskCache(getCachePath(), getCacheSize(), getCacheExpiration(), this);
}

