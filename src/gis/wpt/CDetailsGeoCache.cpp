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

#include "GeoMath.h"
#include "gis/wpt/CDetailsGeoCache.h"
#include "gis/wpt/CGisItemWpt.h"

#include <QtWebKitWidgets>
#include <QtWidgets>
#include <QtNetwork>


CDetailsGeoCache::CDetailsGeoCache(CGisItemWpt &wpt, QWidget *parent)
    : QDialog(parent)
    , wpt(wpt)
{
    setupUi(this);
    setWindowTitle(wpt.getName());

    QString val, unit;
    QString strPos;
    QPointF pos = wpt.getPosition();
    GPS_Math_Deg_To_Str(pos.x(), pos.y(), strPos);

    const CGisItemWpt::geocache_t& geocache = wpt.getGeoCache();

    labelName->setText(geocache.name);
    labelPositon->setText(strPos);

    qreal d = geocache.difficulty;
    labelD1->setPixmap(QPixmap(d < 0.5 ? "://icons/cache/32x32/star_empty.png" : d < 1.0 ? "://icons/cache/32x32/halfstar.png" : "://icons/cache/32x32/star.png").scaled(16,16,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    labelD2->setPixmap(QPixmap(d < 1.5 ? "://icons/cache/32x32/star_empty.png" : d < 2.0 ? "://icons/cache/32x32/halfstar.png" : "://icons/cache/32x32/star.png").scaled(16,16,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    labelD3->setPixmap(QPixmap(d < 2.5 ? "://icons/cache/32x32/star_empty.png" : d < 3.0 ? "://icons/cache/32x32/halfstar.png" : "://icons/cache/32x32/star.png").scaled(16,16,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    labelD4->setPixmap(QPixmap(d < 3.5 ? "://icons/cache/32x32/star_empty.png" : d < 4.0 ? "://icons/cache/32x32/halfstar.png" : "://icons/cache/32x32/star.png").scaled(16,16,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    labelD5->setPixmap(QPixmap(d < 4.5 ? "://icons/cache/32x32/star_empty.png" : d < 5.0 ? "://icons/cache/32x32/halfstar.png" : "://icons/cache/32x32/star.png").scaled(16,16,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    qreal t = geocache.terrain;
    labelT1->setPixmap(QPixmap(t < 0.5 ? "://icons/cache/32x32/star_empty.png" : t < 1.0 ? "://icons/cache/32x32/halfstar.png" : "://icons/cache/32x32/star.png").scaled(16,16,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    labelT2->setPixmap(QPixmap(t < 1.5 ? "://icons/cache/32x32/star_empty.png" : t < 2.0 ? "://icons/cache/32x32/halfstar.png" : "://icons/cache/32x32/star.png").scaled(16,16,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    labelT3->setPixmap(QPixmap(t < 2.5 ? "://icons/cache/32x32/star_empty.png" : t < 3.0 ? "://icons/cache/32x32/halfstar.png" : "://icons/cache/32x32/star.png").scaled(16,16,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    labelT4->setPixmap(QPixmap(t < 3.5 ? "://icons/cache/32x32/star_empty.png" : t < 4.0 ? "://icons/cache/32x32/halfstar.png" : "://icons/cache/32x32/star.png").scaled(16,16,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    labelT5->setPixmap(QPixmap(t < 4.5 ? "://icons/cache/32x32/star_empty.png" : t < 5.0 ? "://icons/cache/32x32/halfstar.png" : "://icons/cache/32x32/star.png").scaled(16,16,Qt::KeepAspectRatio, Qt::SmoothTransformation));

    checkHint->setEnabled(!geocache.hint.isEmpty());
    labelHint->setText(geocache.hint.isEmpty() ? tr("none") : tr("???"));
    toolIcon->setIcon(wpt.getIcon());

    QString desc;
    if(geocache.shortDescIsHtml)
    {
        desc += geocache.shortDesc;
    }
    else
    {
        QString str = geocache.shortDesc;
        desc += "<p>" + str.replace("\n","<br/>") + "</p>";
    }

    if(geocache.longDescIsHtml)
    {
        desc += geocache.longDesc;
    }
    else
    {
        QString str = geocache.longDesc;
        desc += "<p>" + str.replace("\n","<br/>") + "</p>";
    }
    webDesc->setHtml(desc);
    webDesc->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    photoAlbum->reload(wpt.getImages());

    connect(checkHint, SIGNAL(toggled(bool)), this, SLOT(slotHintChanged(bool)));
    connect(webDesc, SIGNAL(linkClicked(QUrl)), this, SLOT(slotLinkClicked(QUrl)));

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(slotRequestFinished(QNetworkReply*)));

    slotCollectSpoiler();
}

CDetailsGeoCache::~CDetailsGeoCache()
{
}

void CDetailsGeoCache::slotHintChanged(bool on)
{
    if(on)
    {
        labelHint->setText(wpt.getGeoCache().hint);
    }
    else
    {
        labelHint->setText(tr("???"));
    }
}

void CDetailsGeoCache::slotLinkClicked(const QUrl& url)
{
    QDesktopServices::openUrl(url);
}

void CDetailsGeoCache::slotCollectSpoiler()
{
    const QList<IGisItem::link_t>& links = wpt.getLinks();
    if(links.isEmpty())
    {
        return;
    }

    QNetworkRequest request;
    request.setUrl(links.first().uri);
    networkManager->get(request);
}

void CDetailsGeoCache::slotRequestFinished(QNetworkReply * reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << reply->errorString();
        return;
    }

    QString asw = reply->readAll();
    reply->deleteLater();

    if(asw.isEmpty())
    {
        return;
    }

    qDebug() << asw;

    static int cnt = 0;
    QFile f(QString("page%1,html").arg(cnt++));
    f.open(QIODevice::WriteOnly);
    f.write(asw.toUtf8(), asw.size());
    f.close();

    QRegExp re0(".*Object moved to <a href=\"(.*)\".*");
    QRegExp re1(".*CachePageImages.*");
    QRegExp re2("(http://.*\\.jpg).*>(.*)</a>");
    re2.setMinimal(true);

    bool watchOut       = false;
    bool spoilerFound   = false;
    QStringList lines   = asw.split("\n");
    foreach(const QString& line, lines)
    {
        if(re0.exactMatch(line))
        {
            QUrl url(re0.cap(1));

            QNetworkRequest request;
            request.setUrl(url);
            networkManager->get(request);
            return;
        }
        else if(!watchOut && re1.exactMatch(line))
        {
            watchOut = true;
        }
        else if(watchOut)
        {
            int pos = 0;
            while ((pos = re2.indexIn(line, pos)) != -1)
            {
                spoilerFound = true;

                QString url  = re2.cap(1);
                QString text = re2.cap(2);

                qDebug() << url;
                qDebug() << text;
//                QNetworkRequest request;
//                request.setUrl(url);
//                pendingRequests[networkAccessManager->get(request)] = text;

                pos += re2.matchedLength();
            }

            watchOut = false;
        }

    }

}
