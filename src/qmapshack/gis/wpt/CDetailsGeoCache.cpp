/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2019 Henri Hornburg <hrnbg@t-online.de>

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

#include "gis/wpt/CDetailsGeoCache.h"

#include <QtNetwork>
#include <QtWidgets>

#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CWebPage.h"

#define HTTP_ATTR_WHAT QNetworkRequest::Attribute(QNetworkRequest::User + 1)
#define HTTP_ATTR_INFO QNetworkRequest::Attribute(QNetworkRequest::User + 2)

CDetailsGeoCache::CDetailsGeoCache(CGisItemWpt& wpt, QWidget* parent) : QDialog(parent), wpt(wpt) {
  setupUi(this);
  setWindowTitle(wpt.getName());

  QString strPos;
  QPointF pos = wpt.getPosition();
  IUnit::degToStr(pos.x(), pos.y(), strPos);

  const CGisItemWpt::geocache_t& geocache = wpt.getGeoCache();

  QString status;
  if (geocache.archived) {
    status = tr("Archived");
  } else if (geocache.available) {
    status = tr("Available");
  } else {
    status = tr("Not Available");
  }

  if (geocache.needsMaintenance) {
    status += ", " + tr("Needs Maintenance");
  }

  labelName->setText(geocache.name + " - " + status);
  labelPositon->setText(strPos);
  labelOwner->setText(geocache.owner);
  labelSize->setText(geocache.container);
  const QString& format = QLocale().dateTimeFormat(QLocale::ShortFormat);
  labelHiddenDate->setText(wpt.getTimestamp().date().toString(format));
  // Last found is set below to only loop logs once

  qreal d = geocache.difficulty;
  labelD1->setPixmap(QPixmap(d < 0.5   ? "://icons/cache/32x32/star_empty.png"
                             : d < 1.0 ? "://icons/cache/32x32/halfstar.png"
                                       : "://icons/cache/32x32/star.png")
                         .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  labelD2->setPixmap(QPixmap(d < 1.5   ? "://icons/cache/32x32/star_empty.png"
                             : d < 2.0 ? "://icons/cache/32x32/halfstar.png"
                                       : "://icons/cache/32x32/star.png")
                         .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  labelD3->setPixmap(QPixmap(d < 2.5   ? "://icons/cache/32x32/star_empty.png"
                             : d < 3.0 ? "://icons/cache/32x32/halfstar.png"
                                       : "://icons/cache/32x32/star.png")
                         .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  labelD4->setPixmap(QPixmap(d < 3.5   ? "://icons/cache/32x32/star_empty.png"
                             : d < 4.0 ? "://icons/cache/32x32/halfstar.png"
                                       : "://icons/cache/32x32/star.png")
                         .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  labelD5->setPixmap(QPixmap(d < 4.5   ? "://icons/cache/32x32/star_empty.png"
                             : d < 5.0 ? "://icons/cache/32x32/halfstar.png"
                                       : "://icons/cache/32x32/star.png")
                         .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  qreal t = geocache.terrain;
  labelT1->setPixmap(QPixmap(t < 0.5   ? "://icons/cache/32x32/star_empty.png"
                             : t < 1.0 ? "://icons/cache/32x32/halfstar.png"
                                       : "://icons/cache/32x32/star.png")
                         .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  labelT2->setPixmap(QPixmap(t < 1.5   ? "://icons/cache/32x32/star_empty.png"
                             : t < 2.0 ? "://icons/cache/32x32/halfstar.png"
                                       : "://icons/cache/32x32/star.png")
                         .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  labelT3->setPixmap(QPixmap(t < 2.5   ? "://icons/cache/32x32/star_empty.png"
                             : t < 3.0 ? "://icons/cache/32x32/halfstar.png"
                                       : "://icons/cache/32x32/star.png")
                         .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  labelT4->setPixmap(QPixmap(t < 3.5   ? "://icons/cache/32x32/star_empty.png"
                             : t < 4.0 ? "://icons/cache/32x32/halfstar.png"
                                       : "://icons/cache/32x32/star.png")
                         .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  labelT5->setPixmap(QPixmap(t < 4.5   ? "://icons/cache/32x32/star_empty.png"
                             : t < 5.0 ? "://icons/cache/32x32/halfstar.png"
                                       : "://icons/cache/32x32/star.png")
                         .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));

  checkHint->setEnabled(!geocache.hint.isEmpty());
  labelHint->setText(geocache.hint.isEmpty() ? tr("none") : tr("???"));
  toolIcon->setIcon(wpt.getIcon());
  labelStatus->hide();

  if (!geocache.attributes.isEmpty()) {
    const QList<quint8>& keys = geocache.attributes.keys();
    for (quint8 attribute : keys) {
      QString filepath = "://icons/geocaching/attributes/";
      if (!geocache.attributes[attribute]) {
        filepath.append('_');
      }
      filepath += QString::number(attribute) + ".png";

      QLabel* attrLabel = new QLabel(this);  // To avoid memory leak
      attrLabel->setAlignment(Qt::AlignLeft);
      attrLabel->setPixmap(QPixmap(filepath));
      attributeHorLayout->insertWidget(0, attrLabel);
    }

    labelNoAttr->setHidden(true);
  }

  QString desc;
  if (geocache.shortDescIsHtml) {
    desc += geocache.shortDesc;
  } else {
    QString str = geocache.shortDesc;
    desc += "<p>" + str.replace("\n", "<br/>") + "</p>";
  }

  if (geocache.longDescIsHtml) {
    desc += geocache.longDesc;
  } else {
    QString str = geocache.longDesc;
    desc += "<p>" + str.replace("\n", "<br/>") + "</p>";
  }

  CWebPage* webDescPage = new CWebPage(webDesc);
  webDesc->setPage(webDescPage);
  webDesc->setHtml(desc);

  const QDateTime& lastFound = geocache.getLastFound();
  labelLastFound->setText(lastFound.date().toString(format));

  CWebPage* webLogPage = new CWebPage(webLogs);
  webLogs->setPage(webLogPage);
  webLogs->setHtml(geocache.getLogs());

  timerDownload = new QTimer(this);
  timerDownload->setSingleShot(true);
  connect(timerDownload, &QTimer::timeout, this, &CDetailsGeoCache::slotDownloadDone);
  connect(buttonVisitWebsite, &QPushButton::clicked, this, &CDetailsGeoCache::slotVisitWebsite);
  connect(checkHint, &QCheckBox::toggled, this, &CDetailsGeoCache::slotHintChanged);
  connect(webDescPage, &CWebPage::linkClicked, this, &CDetailsGeoCache::slotLinkClicked);
  connect(toolUpdateSpoiler, &QToolButton::clicked, this, &CDetailsGeoCache::slotCollectSpoiler);

  networkManager = new QNetworkAccessManager(this);
  connect(networkManager, &QNetworkAccessManager::finished, this, &CDetailsGeoCache::slotRequestFinished);

  const QList<CGisItemWpt::image_t>& images = wpt.getImages();
  photoAlbum->reload(images);
  if (images.isEmpty()) {
    slotCollectSpoiler();
    toolUpdateSpoiler->setEnabled(false);
  } else {
    toolUpdateSpoiler->setEnabled(true);
  }
  if (wpt.isOnDevice()) {
    toolUpdateSpoiler->setEnabled(false);
  }

  listHistory->setEnabled(false);
  listHistory->setupHistory(wpt);
}

CDetailsGeoCache::~CDetailsGeoCache() {}

void CDetailsGeoCache::slotVisitWebsite() { QDesktopServices::openUrl("https://www.coord.info/" + wpt.getName()); }

void CDetailsGeoCache::slotHintChanged(bool on) {
  if (on) {
    labelHint->setText(wpt.getGeoCache().hint);
  } else {
    labelHint->setText(tr("???"));
  }
}

void CDetailsGeoCache::slotLinkClicked(const QUrl& url) { QDesktopServices::openUrl(url); }

void CDetailsGeoCache::slotCollectSpoiler() {
  const QList<IGisItem::link_t>& links = wpt.getLinks();
  if (links.isEmpty()) {
    return;
  }

  wpt.loadHistory(0);
  photoAlbum->reload(wpt.getImages());
  listHistory->setupHistory(wpt);

  QNetworkRequest request;
  request.setUrl(links.first().uri);
  networkManager->get(request);

  timerDownload->start(10000);
  labelStatus->show();
  labelStatus->setText(tr("Searching for images..."));
}

void CDetailsGeoCache::slotRequestFinished(QNetworkReply* reply) {
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << reply->errorString();
    reply->deleteLater();
    return;
  }

  qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute);
  qDebug() << reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

  qint32 httpStatusCodeAttribute = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  if (reply->property("whatfor") == "image") {
    QString info = reply->property("info").toString();

    if ((httpStatusCodeAttribute == 301) || (httpStatusCodeAttribute == 302)) {
      QNetworkRequest request;
      request.setUrl(reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl());
      QNetworkReply* reply = networkManager->get(request);
      reply->setProperty("whatfor", "image");
      reply->setProperty("info", info);
    } else if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
      CGisItemWpt::image_t image;
      image.info = info;
      image.pixmap.loadFromData(reply->readAll());
      wpt.addImage(image);

      photoAlbum->reload(wpt.getImages());
      listHistory->setupHistory(wpt);

      cntSpoiler--;
      if (cntSpoiler == 0) {
        slotDownloadDone();
      }
    }
  } else {
    if ((httpStatusCodeAttribute == 301) || (httpStatusCodeAttribute == 302)) {
      QNetworkRequest request;
      request.setUrl(reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl());
      networkManager->get(request);
    }
  }

  QString asw = reply->readAll();
  reply->deleteLater();

  if (asw.isEmpty()) {
    return;
  }

  static const QRegularExpression re1(QRegularExpression::anchoredPattern(".*CachePageImages.*"));
  static const QRegularExpression re2("(https://.*\\.jpg).*>(.*)</a>",
                                      QRegularExpression::PatternOption::InvertedGreedinessOption);

  bool watchOut = false;
  QStringList lines = asw.split("\n");
  for (const QString& line : std::as_const(lines)) {
    if (!watchOut && re1.match(line).hasMatch()) {
      watchOut = true;
    } else if (watchOut) {
      QRegularExpressionMatchIterator i = re2.globalMatch(line);
      while (i.hasNext()) {
        const QRegularExpressionMatch& match = i.next();
        QString url = match.captured(1);
        QString info = match.captured(2);

        QNetworkRequest request;
        request.setUrl(url);
        QNetworkReply* reply = networkManager->get(request);
        reply->setProperty("whatfor", "image");
        reply->setProperty("info", info);
        cntSpoiler++;
      }

      watchOut = false;
    }
  }
}

void CDetailsGeoCache::slotDownloadDone() {
  timerDownload->stop();
  cntSpoiler = 0;
  if (wpt.getImages().isEmpty()) {
    labelStatus->setText(tr("No images found"));
  } else {
    labelStatus->hide();
  }
}
