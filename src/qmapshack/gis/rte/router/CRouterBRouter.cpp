/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "gis/rte/router/CRouterBRouter.h"

#include <QtNetwork>
#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CGisWorkspace.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/rte/router/brouter/CRouterBRouterInfo.h"
#include "gis/rte/router/brouter/CRouterBRouterLocal.h"
#include "gis/rte/router/brouter/CRouterBRouterSetup.h"
#include "gis/rte/router/brouter/CRouterBRouterSetupWizard.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSettings.h"

CRouterBRouter* CRouterBRouter::pSelf;

CRouterBRouter::CRouterBRouter(QWidget* parent) : IRouter(false, parent) {
  pSelf = this;
  setupUi(this);

  labelBRouterWarning->hide();

  setup = new CRouterBRouterSetup(this);
  setup->load();

  connect(toolSetup, &QToolButton::clicked, this, &CRouterBRouter::slotToolSetupClicked);
  connect(toolProfileInfo, &QToolButton::clicked, this, &CRouterBRouter::slotToolProfileInfoClicked);
  connect(setup, &CRouterBRouterSetup::sigDisplayOnlineProfileFinished, this, &CRouterBRouter::slotDisplayProfileInfo);
  connect(setup, &CRouterBRouterSetup::sigError, this, &CRouterBRouter::slotDisplayError);

  comboAlternative->addItem(tr("original"), "0");
  comboAlternative->addItem(tr("first alternative"), "1");
  comboAlternative->addItem(tr("second alternative"), "2");
  comboAlternative->addItem(tr("third alternative"), "3");

  networkAccessManager = new QNetworkAccessManager(this);
  connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CRouterBRouter::slotRequestFinished);
  connect(setup, &CRouterBRouterSetup::sigVersionChanged, this, &CRouterBRouter::slotVersionChanged);

  timerCloseStatusMsg = new QTimer(this);
  timerCloseStatusMsg->setSingleShot(true);
  timerCloseStatusMsg->setInterval(5000);
  connect(timerCloseStatusMsg, &QTimer::timeout, this, &CRouterBRouter::slotCloseStatusMsg);

  routerSetup = dynamic_cast<CRouterSetup*>(parent);

  connect(toolConsole, &QToolButton::clicked, this, &CRouterBRouter::slotToggleConsole);
  connect(toolToggleBRouter, &QToolButton::clicked, this, &CRouterBRouter::slotToggleBRouter);
  connect(pushBRouterError, &QPushButton::clicked, this, &CRouterBRouter::slotClearError);

  textBRouterOutput->setVisible(false);
  textBRouterError->setVisible(false);
  pushBRouterError->setVisible(false);

  localBRouter = new CRouterBRouterLocal(*this);

  updateDialog();

  SETTINGS;

  cfg.beginGroup("Route/brouter");
  comboProfile->setCurrentIndex(cfg.value("profile", 0).toInt());
  checkFastRecalc->setChecked(cfg.value("fastRecalc", false).toBool() &&
                              (setup->installMode == CRouterBRouterSetup::eModeLocal));
  comboAlternative->setCurrentIndex(cfg.value("alternative", 0).toInt());
  cfg.endGroup();
}

CRouterBRouter::~CRouterBRouter() {
  isShutdown = true;
  if (!localBRouter->isBRouterNotRunning()) {
    localBRouter->stopBRouter();
  }
  SETTINGS;
  cfg.beginGroup("Route/brouter");
  cfg.setValue("profile", comboProfile->currentIndex());
  cfg.setValue("alternative", comboAlternative->currentIndex());
  cfg.setValue("fastRecalc", checkFastRecalc->isChecked());
  cfg.endGroup();
}

void CRouterBRouter::slotToolSetupClicked() {
  localBRouter->stopBRouter();
  CRouterBRouterSetupWizard setupWizard;
  setupWizard.exec();
  slotClearError();
  setup->load();
  getBRouterVersion();
  updateDialog();
}

void CRouterBRouter::slotToolProfileInfoClicked() const {
  const int index = comboProfile->currentIndex();
  if (index > -1) {
    setup->displayProfileAsync(setup->getProfiles().at(index));
  }
}

void CRouterBRouter::slotDisplayError(const QString& error, const QString& details) const {
  textBRouterError->setText(error);
  if (!details.isEmpty()) {
    textBRouterError->append(details);
  }
  QTextCursor cursor = textBRouterError->textCursor();
  cursor.movePosition(QTextCursor::Start);
  textBRouterError->setTextCursor(cursor);
  textBRouterError->setVisible(true);
  textBRouterOutput->setVisible(false);
  pushBRouterError->setVisible(true);
}

void CRouterBRouter::slotClearError() {
  textBRouterError->clear();
  textBRouterError->setVisible(false);
  pushBRouterError->setVisible(false);
  localBRouter->clearBRouterError();
}

void CRouterBRouter::slotDisplayProfileInfo(const QString& profile, const QString& content) {
  slotClearError();
  CRouterBRouterInfo info;
  info.setLabel(profile);
  info.setInfo(content);
  info.exec();
}

void CRouterBRouter::setupLocalDir(QString localDir) {
  if (setup->isLocalBRouterDefaultDir()) {
    setup->localDir = localDir;
    setup->save();
  }
}

void CRouterBRouter::updateDialog() const {
  if (setup->installMode == CRouterBRouterSetup::eModeLocal) {
    routerSetup->setRouterTitle(CRouterSetup::RouterBRouter, tr("BRouter (offline)"));
    labelCopyrightBRouter->setVisible(true);
    labelCopyrightBRouterWeb->setVisible(false);
  } else {
    Q_ASSERT(setup->installMode == CRouterBRouterSetup::eModeOnline);
    routerSetup->setRouterTitle(CRouterSetup::RouterBRouter, tr("BRouter (online)"));
    labelCopyrightBRouter->setVisible(false);
    labelCopyrightBRouterWeb->setVisible(true);
  }
  comboProfile->clear();
  bool hasItems = false;
  const QStringList& profiles = setup->getProfiles();
  for (const QString& profile : profiles) {
    comboProfile->addItem(profile, profile);
    hasItems = true;
  }
  comboProfile->setEnabled(hasItems);
  toolProfileInfo->setEnabled(hasItems);
  comboAlternative->setEnabled(hasItems);
  updateBRouterStatus();
}

void CRouterBRouter::slotCloseStatusMsg() const {
  timerCloseStatusMsg->stop();
  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  if (canvas) {
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    canvas->reportStatus("BRouter", "");
  }
}

QString CRouterBRouter::getOptions() {
  return QString(tr("profile: %1, alternative: %2")
                     .arg(comboProfile->currentData().toString())
                     .arg(comboAlternative->currentData().toInt() + 1));
}

void CRouterBRouter::routerSelected() { getBRouterVersion(); }

bool CRouterBRouter::hasFastRouting() {
  return setup->installMode == CRouterBRouterSetup::eModeLocal && setup->isLocalBRouterValid &&
         checkFastRecalc->isChecked();
}

QNetworkRequest CRouterBRouter::getRequest(const QVector<QPointF>& routePoints, const QList<IGisItem*>& nogos) const {
  QString lonLats;

  for (const QPointF& pt : routePoints) {
    if (!lonLats.isEmpty()) {
      lonLats.append("|");
    }
    lonLats.append(QString("%1,%2").arg(pt.x(), 0, 'f', 6).arg(pt.y(), 0, 'f', 6));
  }

  QString nogoStr;
  QString nogoPolygons;
  QString nogoPolylines;

  for (IGisItem* const& item : nogos) {
    switch (item->type()) {
      case IGisItem::eTypeWpt: {
        CGisItemWpt* wpt = static_cast<CGisItemWpt*>(item);
        const qreal& rad = wpt->getProximity();
        if (rad != NOFLOAT && rad > 0.) {
          const QPointF& pos = wpt->getPosition();
          if (!nogoStr.isEmpty()) {
            nogoStr.append("|");
          }
          nogoStr.append(QString("%1,%2,%3").arg(pos.x(), 0, 'f', 6).arg(pos.y(), 0, 'f', 6).arg(rad, 0, 'f', 0));
        }
        break;
      }

      case IGisItem::eTypeOvl:
      case IGisItem::eTypeRte:
      case IGisItem::eTypeTrk: {
        IGisLine* line = dynamic_cast<IGisLine*>(item);
        Q_ASSERT(line != nullptr);
        QPolygonF polygon;
        line->getPolylineDegFromData(polygon);
        QString nogoPoints;
        for (const QPointF point : std::as_const(polygon)) {
          if (!nogoPoints.isEmpty()) {
            nogoPoints.append(",");
          }
          nogoPoints.append(QString("%1,%2").arg(point.x(), 0, 'f', 6).arg(point.y(), 0, 'f', 6));
        }
        if (item->type() == IGisItem::eTypeOvl) {
          if (!nogoPolygons.isEmpty()) {
            nogoPolygons.append("|");
          }
          nogoPolygons.append(QString("%1").arg(nogoPoints));
        } else {
          if (!nogoPolylines.isEmpty()) {
            nogoPolylines.append("|");
          }
          nogoPolylines.append(QString("%1").arg(nogoPoints));
        }
        break;
      }

      default: {
        Q_ASSERT(false);
      }
    }
  }

  QUrlQuery urlQuery;
  urlQuery.addQueryItem("lonlats", lonLats.toLatin1());
  if (!nogoStr.isEmpty()) {
    urlQuery.addQueryItem("nogos", nogoStr.toLatin1());
  }
  if (!nogoPolygons.isEmpty()) {
    urlQuery.addQueryItem("polygons", nogoPolygons.toLatin1());
  }
  if (!nogoPolylines.isEmpty()) {
    urlQuery.addQueryItem("polylines", nogoPolylines.toLatin1());
  }
  urlQuery.addQueryItem("profile", comboProfile->currentData().toString());
  urlQuery.addQueryItem("alternativeidx", comboAlternative->currentData().toString());
  urlQuery.addQueryItem("format", "gpx");

  QUrl url = setup->getServiceUrl();
  url.setQuery(urlQuery);

  return QNetworkRequest(url);
}

int CRouterBRouter::calcRoute(const QPointF& p1, const QPointF& p2, QPolygonF& coords, qreal* costs) {
  if (!hasFastRouting()) {
    return -1;
  }

  const QVector<QPointF> points = {p1 * RAD_TO_DEG, p2 * RAD_TO_DEG};

  QList<IGisItem*> nogos;
  CGisWorkspace::self().getNogoAreas(nogos);

  return synchronousRequest(points, nogos, coords, costs);
}

int CRouterBRouter::synchronousRequest(const QVector<QPointF>& points, const QList<IGisItem*>& nogos, QPolygonF& coords,
                                       qreal* costs = nullptr) {
  if (!mutex.tryLock()) {
    // skip further on-the-fly-requests as long a previous request is still running
    return -1;
  }

  if (setup->installMode == CRouterBRouterSetup::eModeLocal && localBRouter->isBRouterNotRunning()) {
    localBRouter->startBRouter();
  }

  synchronous = true;

  QNetworkReply* reply = networkAccessManager->get(getRequest(points, nogos));

  try {
    reply->setProperty("options", getOptions());
    reply->setProperty("time", QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());

    CProgressDialog progress(tr("Calculate route with %1").arg(getOptions()), 0, NOINT, nullptr);

    QEventLoop eventLoop;
    connect(&progress, &CProgressDialog::rejected, reply, &QNetworkReply::abort);
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::AllEvents);

    const QNetworkReply::NetworkError& netErr = reply->error();
    if (netErr == QNetworkReply::RemoteHostClosedError && nogos.size() > 1 && !isMinimumVersion(1, 4, 10)) {
      throw tr("this version of BRouter does not support more then 1 nogo-area");
    } else if (netErr != QNetworkReply::NoError) {
      throw reply->errorString();
    }
    slotClearError();

    const QByteArray& res = reply->readAll();

    if (res.isEmpty()) {
      throw tr("response is empty");
    }

    QDomDocument xml;
    xml.setContent(res);
    const QDomElement& xmlGpx = xml.documentElement();

    if (xmlGpx.isNull() || xmlGpx.tagName() != "gpx") {
      throw QString(res);
    }
    setup->parseBRouterVersion(xmlGpx.attribute("creator"));

    // read the shape
    const QDomNodeList& xmlLatLng =
        xmlGpx.firstChildElement("trk").firstChildElement("trkseg").elementsByTagName("trkpt");
    for (int n = 0; n < xmlLatLng.size(); n++) {
      const QDomElement& elem = xmlLatLng.item(n).toElement();
      coords << QPointF();
      QPointF& point = coords.last();
      point.setX(elem.attribute("lon").toFloat() * DEG_TO_RAD);
      point.setY(elem.attribute("lat").toFloat() * DEG_TO_RAD);
    }

    // find costs of route (copied and adapted from CGisItemRte::setResultFromBrouter)
    if (costs != nullptr) {
      const QDomNodeList& nodes = xml.childNodes();
      for (int i = 0; i < nodes.count(); i++) {
        const QDomNode& node = nodes.at(i);
        if (!node.isComment()) {
          continue;
        }
        const QString& commentTxt = node.toComment().data();
        // ' track-length = 180864 filtered ascend = 428 plain-ascend = -172 cost=270249 '
        static const QRegularExpression rxAscDes(
            "(\\s*track-length\\s*=\\s*)(-?\\d+)(\\s*)(filtered "
            "ascend\\s*=\\s*-?\\d+)(\\s*)(plain-ascend\\s*=\\s*-?\\d+)(\\s*)(cost\\s*=\\s*)(-?\\d+)(\\s*)");
        const QRegularExpressionMatch& match = rxAscDes.match(commentTxt);
        if (match.hasMatch()) {
          bool ok;
          *costs = match.captured(9).toDouble(&ok);
          if (!ok) {
            *costs = -1;
          }
        }
        break;
      }
    }
  } catch (const QString& msg) {
    coords.clear();
    if (!msg.isEmpty()) {
      reply->deleteLater();
      mutex.unlock();
      throw tr("Bad response from server: %1").arg(msg);
    }
  }

  reply->deleteLater();
  slotCloseStatusMsg();
  mutex.unlock();
  return coords.size();
}

void CRouterBRouter::calcRoute(const IGisItem::key_t& key) {
  mutex.lock();
  if (setup->installMode == CRouterBRouterSetup::eModeLocal && localBRouter->isBRouterNotRunning()) {
    localBRouter->startBRouter();
  }
  CGisItemRte* rte = dynamic_cast<CGisItemRte*>(CGisWorkspace::self().getItemByKey(key));
  if (nullptr == rte) {
    mutex.unlock();
    return;
  }

  QList<IGisItem*> nogos;
  CGisWorkspace::self().getNogoAreas(nogos);

  rte->reset();

  slotCloseStatusMsg();

  QVector<QPointF> points;
  for (const CGisItemRte::rtept_t& pt : rte->getRoute().pts) {
    points << QPointF(pt.lon, pt.lat);
  }

  synchronous = false;

  QNetworkReply* reply = networkAccessManager->get(getRequest(points, nogos));

  reply->setProperty("key.item", key.item);
  reply->setProperty("key.project", key.project);
  reply->setProperty("key.device", key.device);
  reply->setProperty("options", getOptions());
  reply->setProperty("time", QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());

  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  if (canvas) {
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    canvas->reportStatus("BRouter", tr("<b>BRouter</b><br/>Routing request sent to server. Please wait..."));
  }

  progress = new CProgressDialog(tr("Calculate route with %1").arg(getOptions()), 0, NOINT, this);

  connect(progress, &CProgressDialog::rejected, reply, &QNetworkReply::abort);
}

void CRouterBRouter::slotRequestFinished(QNetworkReply* reply) {
  if (synchronous) {
    return;
  }

  delete progress;

  try {
    const QNetworkReply::NetworkError& netErr = reply->error();
    if (netErr == QNetworkReply::RemoteHostClosedError && reply->property("nogos").toInt() > 1 &&
        !isMinimumVersion(1, 4, 10)) {
      throw tr("this version of BRouter does not support more then 1 nogo-area");
    } else if (netErr != QNetworkReply::NoError) {
      throw reply->errorString();
    }

    const QByteArray& res = reply->readAll();
    reply->deleteLater();

    if (res.isEmpty()) {
      throw tr("response is empty");
    }

    slotClearError();

    QDomDocument xml;
    xml.setContent(res);

    const QDomElement& xmlGpx = xml.documentElement();
    if (xmlGpx.isNull() || xmlGpx.tagName() != "gpx") {
      throw QString(res);
    }

    IGisItem::key_t key;
    key.item = reply->property("key.item").toString();
    key.project = reply->property("key.project").toString();
    key.device = reply->property("key.device").toString();
    qint64 time = reply->property("time").toLongLong();
    time = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() - time;

    CGisItemRte* rte = dynamic_cast<CGisItemRte*>(CGisWorkspace::self().getItemByKey(key));
    if (rte != nullptr) {
      rte->setResultFromBRouter(
          xml, reply->property("options").toString() + tr("<br/>Calculation time: %1s").arg(time / 1000.0, 0, 'f', 2));
    }
  } catch (const QString& msg) {
    if (!msg.isEmpty()) {
      CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
      if (canvas) {
        canvas->reportStatus("BRouter", tr("<b>BRouter</b><br/>Bad response from server:<br/>%1").arg(msg));
      }
      timerCloseStatusMsg->start();
      reply->deleteLater();
      mutex.unlock();
      return;
    }
  }

  slotCloseStatusMsg();
  mutex.unlock();
}

void CRouterBRouter::slotToggleConsole() const {
  textBRouterOutput->setVisible(!textBRouterOutput->isVisible());
  bool showError = localBRouter->isBRouterError() && !textBRouterOutput->isVisible();
  textBRouterError->setVisible(showError);
  pushBRouterError->setVisible(showError);
}

void CRouterBRouter::slotToggleBRouter() const {
  if (localBRouter->isBRouterNotRunning()) {
    localBRouter->startBRouter();
  } else {
    localBRouter->stopBRouter();
  }
}

void CRouterBRouter::getBRouterVersion() {
  if (setup->installMode == CRouterBRouterSetup::eModeLocal) {
    localBRouter->getBRouterVersion();
  } else {
    setup->loadOnlineVersion();
  }
}

void CRouterBRouter::slotVersionChanged() {
  if (setup->versionMajor != NOINT && setup->versionMinor != NOINT && setup->versionPatch != NOINT) {
    labelBRouter->setToolTip(
        tr("BRouter (Version %1.%2.%3)").arg(setup->versionMajor).arg(setup->versionMinor).arg(setup->versionPatch));
  } else {
    labelBRouter->setToolTip("BRouter: (failed to read version)");
  }
}

bool CRouterBRouter::isMinimumVersion(int major, int minor, int patch) const {
  if (setup->versionMajor == NOINT || setup->versionMinor == NOINT || setup->versionPatch == NOINT) {
    return false;
  }
  if (setup->versionMajor > major) {
    return true;
  }
  if (setup->versionMajor < major) {
    return false;
  }
  if (setup->versionMinor > minor) {
    return true;
  }
  if (setup->versionMinor < minor) {
    return false;
  }
  if (setup->versionPatch > patch) {
    return true;
  }
  if (setup->versionPatch < patch) {
    return false;
  }
  return true;
}

void CRouterBRouter::updateBRouterStatus() const {
  if (isShutdown) {
    return;
  }

  labelBRouterWarning->hide();
  if (setup->installMode == CRouterBRouterSetup::eModeLocal) {
    localBRouter->updateLocalBRouterStatus();
  } else {
    Q_ASSERT(setup->installMode == CRouterBRouterSetup::eModeOnline);
    labelStatus->setText(tr("online"));
    toolConsole->setVisible(false);
    toolToggleBRouter->setVisible(false);
    checkFastRecalc->setVisible(false);
    textBRouterOutput->clear();
    textBRouterOutput->setVisible(false);
  }
}
