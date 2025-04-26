/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Christian Eichler <code@christian-eichler.de>

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

#include "map/IMapOnline.h"

#include <QMessageBox>
#include <QtNetwork>

#include "CMainWindow.h"
#include "map/CMapDraw.h"
#include "map/cache/CDiskCache.h"

IMapOnline::IMapOnline(CMapDraw* parent) : IMap(eFeatVisibility | eFeatTileCache, parent) {
  accessManager = new QNetworkAccessManager(parent->thread());
  connect(accessManager, &QNetworkAccessManager::finished, this, &IMapOnline::slotRequestFinished);

  connect(this, &IMapOnline::sigQueueChanged, this, &IMapOnline::slotQueueChanged);
}

bool IMapOnline::httpsCheck(const QString& url) {
  if (url.startsWith("https", Qt::CaseInsensitive) && !QSslSocket::supportsSsl()) {
    QString msg =
        tr("This map requires OpenSSL support. However due to legal restrictions in some countries "
           "OpenSSL is not packaged with QMapShack. You can have a look at the "
           "<a href='https://www.openssl.org/community/binaries.html'>OpenSSL Homepage</a> "
           "for binaries. You have to copy libeay32.dll and ssleay32.dll into the QMapShack program directory.");
    QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Error..."), msg, QMessageBox::Abort);
    return false;
  }
  return true;
}

void IMapOnline::slotQueueChanged() {
  QMutexLocker lock(&mutex);

  if (!urlQueue.isEmpty() && urlPending.size() < 6) {
    // request up to 6 pending request
    for (int i = 0; i < (6 - urlPending.size()); i++) {
      QString url = urlQueue.dequeue();
      // remove all remaining duplicates from queue
      urlQueue.removeAll(url);
      lastRequest = urlQueue.isEmpty();

      if (!urlPending.contains(url)) {
        QNetworkRequest request;
        request.setUrl(url);
        for (const rawHeaderItem_t& item : std::as_const(rawHeaderItems)) {
          request.setRawHeader(item.name.toLatin1(), item.value.toLatin1());
        }
        // allow http(s) redirects
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        accessManager->get(request);
        urlPending << url;
      }

      if (lastRequest) {
        break;
      }
    }
  } else if (lastRequest && urlPending.isEmpty()) {
    lastRequest = false;
    // if all tiles are received the map layer can be redrawn with all tiles from cache
    map->emitSigCanvasUpdate();
  }

  if (timeLastUpdate.elapsed() > 2000) {
    timeLastUpdate.start();
    map->emitSigCanvasUpdate();
  }

  // report status of pending tiles
  int pending = urlQueue.size() + urlPending.size();
  if (pending) {
    map->reportStatusToCanvas(name, tr("<b>%1</b>: %2 tiles pending<br/>").arg(name).arg(pending));
  } else {
    map->reportStatusToCanvas(name, "");
  }
}

void IMapOnline::slotRequestFinished(QNetworkReply* reply) {
  QMutexLocker lock(&mutex);

  // use originally requested url due to possible redirects
  QString url = reply->request().url().toString();
  if (urlPending.contains(url)) {
    QImage img;
    // only take good responses
    if (!reply->error()) {
      // read image data
      img.loadFromData(reply->readAll());
    }
    // always store image to cache, the cache will take care of NULL images
    diskCache->store(url, img);

    urlPending.removeAll(url);
  }

  // debug output any error
  if (reply->error()) {
    qDebug() << "Request to" << url << "failed:" << reply->errorString();
  }

  // delete reply object
  reply->deleteLater();

  // check for more items to be queued
  slotQueueChanged();
}

void IMapOnline::configureCache() {
  QMutexLocker lock(&mutex);

  delete diskCache;
  diskCache = new CDiskCache(getCachePath(), getCacheSize(), getCacheExpiration(), this);
}
