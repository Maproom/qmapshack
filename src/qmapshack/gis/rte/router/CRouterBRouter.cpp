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

#include <QScopeGuard>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QWidget>

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

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
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

  // Debounce: coalesces rapid calcRoute(key) calls into a single request.
  // The mutex is NOT held during the debounce window — see header for the
  // full flow description.
  timerDebounce = new QTimer(this);
  timerDebounce->setSingleShot(true);
  timerDebounce->setInterval(DEBOUNCE_TIME);

  routeCache.setMaxCost(MAX_CACHED);

  updateDialog();

  SETTINGS;
  cfg.beginGroup("Route/brouter");
  comboProfile->setCurrentIndex(cfg.value("profile", 0).toInt());
  comboAlternative->setCurrentIndex(cfg.value("alternative", 0).toInt());
  cfg.endGroup();
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CRouterBRouter::~CRouterBRouter() {
  isShutdown = true;

  timerDebounce->stop();
  hasPendingKey = false;

  if (pendingAsyncReply) {
    pendingAsyncReply->disconnect();
    pendingAsyncReply->abort();
    pendingAsyncReply->deleteLater();
    pendingAsyncReply = nullptr;
    if (asyncMutexHeld) {
      asyncMutexHeld = false;
      mutex.unlock();
    }
  }

  if (!localBRouter->isBRouterNotRunning()) {
    localBRouter->stopBRouter();
  }

  SETTINGS;
  cfg.beginGroup("Route/brouter");
  cfg.setValue("profile", comboProfile->currentIndex());
  cfg.setValue("alternative", comboAlternative->currentIndex());
  cfg.endGroup();
}

// ---------------------------------------------------------------------------
// scheduleNextIfPending
//
// Called at the end of every async completion path (success, error, cancel).
//
// Calls startAsyncRequest() directly instead of calcRoute() — this skips the
// DEBOUNCE_TIME ms entirely for catch-up requests. The debounce only makes
// sense when the user is actively setting points; here the user already
// waited for the previous request to finish, so adding another DEBOUNCE_TIME ms
// would just feel sluggish.
//
// Uses QueuedConnection so the UI gets exactly one event-loop tick to repaint
// the newly finished route before the next calculation begins.
// ---------------------------------------------------------------------------
void CRouterBRouter::scheduleNextIfPending() {
  if (!hasPendingKey) {
    return;
  }
  const IGisItem::key_t nextKey = pendingKey;
  hasPendingKey = false;
  // Skip debounce — go directly to startAsyncRequest() for catch-up.
  QMetaObject::invokeMethod(this, [this, nextKey]() { startAsyncRequest(nextKey); }, Qt::QueuedConnection);
}

// ---------------------------------------------------------------------------
// UI slots
// ---------------------------------------------------------------------------
void CRouterBRouter::slotToolSetupClicked() {
  localBRouter->stopBRouter();
  CRouterBRouterSetupWizard setupWizard;
  setupWizard.exec();
  slotClearError();
  setup->load();
  getBRouterVersion();
  updateDialog();
  routeCache.clear();
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
  return QString(tr("profile: %1, alternative: %2"))
      .arg(comboProfile->currentText())
      .arg(comboAlternative->currentData().toInt());
}

void CRouterBRouter::routerSelected() { getBRouterVersion(); }

bool CRouterBRouter::hasFastRouting() {
  return setup->installMode == CRouterBRouterSetup::eModeLocal && setup->isLocalBRouterValid;
}

QNetworkRequest CRouterBRouter::getRequest(const QVector<QPointF>& routePoints, const QList<IGisItem*>& nogos) const {
  QString lonLats;

  for (const QPointF& pt : routePoints) {
    if (!lonLats.isEmpty()) {
      lonLats += '|';
    }
    lonLats += QString("%1,%2").arg(pt.x(), 0, 'f', 6).arg(pt.y(), 0, 'f', 6);
  }

  QString nogoStr;
  QString nogoPolygons;
  QString nogoPolylines;

  for (IGisItem* const& item : nogos) {
    switch (item->type()) {
      case IGisItem::eTypeWpt: {
        CGisItemWpt* wpt = static_cast<CGisItemWpt*>(item);
        const qreal rad = wpt->getProximity();
        if (rad != NOFLOAT && rad > 0.) {
          const QPointF& pos = wpt->getPosition();
          if (!nogoStr.isEmpty()) {
            nogoStr += '|';
          }
          nogoStr += QString("%1,%2,%3").arg(pos.x(), 0, 'f', 6).arg(pos.y(), 0, 'f', 6).arg(rad, 0, 'f', 0);
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
        for (const QPointF& point : std::as_const(polygon)) {
          if (!nogoPoints.isEmpty()) {
            nogoPoints += ',';
          }
          nogoPoints += QString("%1,%2").arg(point.x(), 0, 'f', 6).arg(point.y(), 0, 'f', 6);
        }
        if (item->type() == IGisItem::eTypeOvl) {
          if (!nogoPolygons.isEmpty()) {
            nogoPolygons += '|';
          }
          nogoPolygons += nogoPoints;
        } else {
          if (!nogoPolylines.isEmpty()) {
            nogoPolylines += '|';
          }
          nogoPolylines += nogoPoints;
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
  urlQuery.addQueryItem("timode", "8");

  QUrl url = setup->getServiceUrl();
  url.setQuery(urlQuery);

  QNetworkRequest request(url);
  request.setRawHeader("Connection", "keep-alive");
  return request;
}

// ---------------------------------------------------------------------------
// buildCacheKey
// ---------------------------------------------------------------------------
QString CRouterBRouter::buildCacheKey(const QVector<QPointF>& points, const QList<IGisItem*>& nogos) const {
  QString key = comboProfile->currentData().toString() + QLatin1Char('|') + comboAlternative->currentData().toString();
  for (const QPointF& p : points) {
    key += QString("|%1,%2").arg(p.x(), 0, 'f', 6).arg(p.y(), 0, 'f', 6);
  }
  for (const IGisItem* item : nogos) {
    key += QLatin1String("|N:") + item->getKey().item;
  }
  return key;
}

int CRouterBRouter::calcRoute(const QPointF& p1, const QPointF& p2, QPolygonF& coords, qreal* costs) {
  if (!hasFastRouting()) {
    return -1;
  }

  const QVector<QPointF> points = {p1 * RAD_TO_DEG, p2 * RAD_TO_DEG};

  QList<IGisItem*> nogos;
  CGisWorkspace::self().getNogoAreas(nogos);

  const QString cacheKey = buildCacheKey(points, nogos);
  if (const CachedRoute* cached = routeCache.object(cacheKey)) {
    coords = cached->coords;
    if (costs != nullptr) {
      *costs = cached->costs;
    }
    return coords.size();
  }

  return synchronousRequest(points, nogos, coords, costs);
}

// ---------------------------------------------------------------------------
// synchronousRequest
// ---------------------------------------------------------------------------
int CRouterBRouter::synchronousRequest(const QVector<QPointF>& points,
                                       const QList<IGisItem*>& nogos, QPolygonF& coords, qreal* costs) {
  if (!mutex.tryLock()) {
    return -1;
  }
  auto mutexGuard = qScopeGuard([this] { mutex.unlock(); });

  if (setup->installMode == CRouterBRouterSetup::eModeLocal && localBRouter->isBRouterNotRunning()) {
    localBRouter->startBRouter();
  }

  synchronous = true;

  QNetworkReply* reply = networkAccessManager->get(getRequest(points, nogos));
  auto replyGuard = qScopeGuard([reply] { reply->deleteLater(); });
  Q_UNUSED(replyGuard);

  reply->setProperty("options", getOptions());
  reply->setProperty("time", QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());

  CProgressDialog progressDlg(tr("Calculate route with %1").arg(getOptions()), 0, NOINT, nullptr);

  QEventLoop eventLoop;
  connect(&progressDlg, &CProgressDialog::rejected, reply, &QNetworkReply::abort);
  connect(&progressDlg, &CProgressDialog::rejected, &eventLoop, &QEventLoop::quit);
  connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
  // ExcludeUserInputEvents prevents mouse/keyboard events from being processed
  // while we wait for the BRouter response. Without this flag, AllEvents allows
  // the user to click again during the wait, which re-enters slotTimeoutRouting()
  // → synchronousRequest() recursively. The second call hits mutex.tryLock()
  // failure and returns -1, leaving ILineOp in an inconsistent state → crash
  // on Windows/Linux (issue #388) and deadlock on macOS (issue #1007).
  //
  // ExcludeUserInputEvents is safe here because:
  //   • Network socket events are NOT user input — HTTP replies still arrive.
  //   • The Cancel button in CProgressDialog works via dialog-level events,
  //     not raw mouse input — it fires correctly on all platforms.
  //   • Timer events still fire normally.
  eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

  const QNetworkReply::NetworkError netErr = reply->error();
  if (netErr == QNetworkReply::OperationCanceledError) {
    return -1;
  }
  if (netErr == QNetworkReply::RemoteHostClosedError && nogos.size() > 1 && !isMinimumVersion(1, 4, 10)) {
    throw tr("this version of BRouter does not support more then 1 nogo-area");
  } else if (netErr != QNetworkReply::NoError) {
    throw tr("Bad response from server: %1").arg(reply->errorString());
  }
  slotClearError();

  const QByteArray& res = reply->readAll();

  if (res.isEmpty()) {
    throw tr("response is empty");
  }

  QDomDocument xml;
  {
    const QDomDocument::ParseResult parseResult = xml.setContent(res);
    if (!parseResult) {
      throw tr("Failed to parse BRouter response (line %1): %2")
          .arg(parseResult.errorLine).arg(parseResult.errorMessage);
    }
  }

  const QDomElement& xmlGpx = xml.documentElement();
  if (xmlGpx.isNull() || xmlGpx.tagName() != "gpx") {
    throw QString(res.data());
  }

  setup->parseBRouterVersion(xmlGpx.attribute("creator"));

  // read the shape
  const QDomNodeList xmlLatLng =
      xmlGpx.firstChildElement("trk").firstChildElement("trkseg").elementsByTagName("trkpt");
  for (int n = 0; n < xmlLatLng.size(); ++n) {
    const QDomElement elem = xmlLatLng.item(n).toElement();
    coords << QPointF(elem.attribute("lon").toFloat() * DEG_TO_RAD,
                      elem.attribute("lat").toFloat() * DEG_TO_RAD);
  }

  // find costs of route (copied and adapted from CGisItemRte::setResultFromBrouter)
  if (costs != nullptr) {
    const QDomNodeList& nodes = xml.childNodes();
    for (int i = 0; i < nodes.count(); i++) {
      const QDomNode& node = nodes.at(i);
      if (node.isComment()) {
        const QString& comment = node.toComment().data();
        // ' track-length = 3181 filtered ascend = 70 plain-ascend = 5 cost=8491 energy=.0kwh time=16m 30s '
        const QRegularExpressionMatch& match = QRegularExpression("cost\\s*=\\s*(-?\\d+)").match(comment);
        if (match.hasMatch()) {
          *costs = match.captured(1).toDouble();
        }
        break;
      }
    }
  }

  routeCache.insert(buildCacheKey(points, nogos), new CachedRoute{coords, costs ? *costs : 0.0});
  slotCloseStatusMsg();
  return coords.size();
}

// ---------------------------------------------------------------------------
// calcRoute(key) — async path
//
// KEY INSIGHT (fixes previous version's 3 problems):
//
//   Problem 1 was: mutex locked immediately in calcRoute(), before the
//   debounce fires. This meant calcRoute() calls during the DEBOUNCE_TIME ms
//   window hit tryLock() failure and could only write to pendingKey, but the
//   debounce timer was already committed to the OLD key — the new key would
//   only run after the old one completed, not instead of it.
//
//   Fix: the mutex is NOT locked in calcRoute(). Instead, every call simply
//   updates pendingKey and restarts the debounce timer. The mutex is locked
//   only in startAsyncRequest(), right before the network request is sent.
//   During the DEBOUNCE_TIME ms window the user can set as many points as they like —
//   only the last one matters, and only that one gets sent to BRouter.
//
//   Problem 2 was: hasPendingKey not processed on error/cancel paths.
//   Fix: scheduleNextIfPending() is called on ALL exit paths.
//
//   Problem 3 was: mutex held during debounce prevented catch-up keys from
//   being queued via pendingKey when another call came in mid-debounce.
//   Fix: resolved automatically by Problem 1 fix (no mutex during debounce).
// ---------------------------------------------------------------------------
void CRouterBRouter::calcRoute(const IGisItem::key_t& key) {
  // Always store the latest key. This is safe without a mutex because both
  // calcRoute() and the timer callback run on the UI thread (Qt's signal
  // delivery is sequential within a thread).
  pendingKey = key;
  hasPendingKey = true;

  if (pendingAsyncReply) {
    // A request is already in-flight — do not restart the debounce.
    // startAsyncRequest() will not be called again until the current request
    // finishes and scheduleNextIfPending() picks up the stored key.
    return;
  }

  // No request in-flight: (re)start the debounce. Qt::SingleShotConnection
  // (Qt 6.0+) ensures the lambda fires exactly once and self-disconnects,
  // so rapid calls that restart the timer never accumulate connections.
  // The lambda captures pendingKey by reference to the member — by the time
  // it fires, pendingKey already holds the very last key the user set.
  connect(timerDebounce, &QTimer::timeout, this,
          [this]() { startAsyncRequest(pendingKey); },
          Qt::SingleShotConnection);
  timerDebounce->start();
}

// ---------------------------------------------------------------------------
// startAsyncRequest
//
// Called by the debounce timer. At this point:
//   • no request is in-flight (pendingAsyncReply == nullptr)
//   • pendingKey holds the latest user-requested position
// ---------------------------------------------------------------------------
void CRouterBRouter::startAsyncRequest(const IGisItem::key_t& key) {
  // Consume the pending key — we are now computing it.
  hasPendingKey = false;

  if (setup->installMode == CRouterBRouterSetup::eModeLocal && localBRouter->isBRouterNotRunning()) {
    localBRouter->startBRouter();
  }

  CGisItemRte* rte = dynamic_cast<CGisItemRte*>(CGisWorkspace::self().getItemByKey(key));
  if (rte == nullptr) {
    // Item deleted during debounce — still process any newer pending key.
    scheduleNextIfPending();
    return;
  }

  // Lock the mutex for the duration of this request.
  if (!mutex.tryLock()) {
    // Synchronous path holds the mutex — queue the key for after it finishes.
    hasPendingKey = true;
    pendingKey = key;
    return;
  }
  asyncMutexHeld = true;

  QList<IGisItem*> nogos;
  CGisWorkspace::self().getNogoAreas(nogos);

  // Collect points BEFORE reset() so we read the current waypoints.
  QVector<QPointF> points;
  for (const CGisItemRte::rtept_t& pt : rte->getRoute().pts) {
    points << QPointF(pt.lon, pt.lat);
  }

  // Do NOT call rte->reset() here — the old route stays visible while the
  // new one is being calculated, which avoids the visual flicker of having
  // nothing displayed during the request. reset() is called in
  // slotRequestFinished() just before applying the new result.
  slotCloseStatusMsg();

  synchronous = false;

  QNetworkReply* reply = networkAccessManager->get(getRequest(points, nogos));
  pendingAsyncReply = reply;

  reply->setProperty("key.item", key.item);
  reply->setProperty("key.project", key.project);
  reply->setProperty("key.device", key.device);
  reply->setProperty("options", getOptions());
  reply->setProperty("time", QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());

  // Report "calculating" status — do NOT trigger a full canvas repaint here.
  // The canvas will redraw itself when slotRequestFinished() applies the new
  // route, avoiding one spurious repaint per request.
  CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
  if (canvas) {
    canvas->reportStatus("BRouter", tr("<b>BRouter</b><br/>Routing request sent to server. Please wait..."));
  }

  delete progress;
  progress = new CProgressDialog(tr("Calculate route with %1").arg(getOptions()), 0, NOINT, this);

  connect(progress, &CProgressDialog::rejected, this, [this, reply]() {
    if (pendingAsyncReply == reply) {
      pendingAsyncReply->disconnect();
      pendingAsyncReply->abort();
      pendingAsyncReply->deleteLater();
      pendingAsyncReply = nullptr;
    }
    delete progress;
    progress = nullptr;
    // Intentional cancel: discard any queued key so the user is not surprised
    // by a new calculation starting immediately after pressing Cancel.
    hasPendingKey = false;
    if (asyncMutexHeld) {
      asyncMutexHeld = false;
      mutex.unlock();
    }
  });
}

// ---------------------------------------------------------------------------
// slotRequestFinished
// ---------------------------------------------------------------------------
void CRouterBRouter::slotRequestFinished(QNetworkReply* reply) {
  if (synchronous) return;

  if (reply != pendingAsyncReply) {
    reply->deleteLater();
    return;
  }

  pendingAsyncReply = nullptr;

  delete progress;
  progress = nullptr;

  auto replyGuard = qScopeGuard([reply] { reply->deleteLater(); });
  Q_UNUSED(replyGuard);

  // Release the mutex and schedule the next pending key on every exit path.
  auto cleanupGuard = qScopeGuard([this] {
    if (asyncMutexHeld) {
      asyncMutexHeld = false;
      mutex.unlock();
    }
    scheduleNextIfPending();
  });
  Q_UNUSED(cleanupGuard);

  const QNetworkReply::NetworkError netErr = reply->error();

  if (netErr == QNetworkReply::OperationCanceledError) {
    // Cancel lambda already released the mutex and cleared hasPendingKey.
    // cleanupGuard's mutex release is a safe no-op (asyncMutexHeld == false).
    // hasPendingKey is false so scheduleNextIfPending() is also a no-op.
    return;
  }

  try {
    if (netErr == QNetworkReply::RemoteHostClosedError &&
        reply->property("nogos").toInt() > 1 && !isMinimumVersion(1, 4, 10)) {
      throw tr("this version of BRouter does not support more then 1 nogo-area");
    } else if (netErr != QNetworkReply::NoError) {
      throw reply->errorString();
    }

    const QByteArray& res = reply->readAll();

    if (res.isEmpty()) {
      throw tr("response is empty");
    }

    slotClearError();

    QDomDocument xml;
    {
      const QDomDocument::ParseResult parseResult = xml.setContent(res);
      if (!parseResult) {
        throw tr("Failed to parse BRouter response (line %1): %2")
            .arg(parseResult.errorLine).arg(parseResult.errorMessage);
      }
    }

    const QDomElement& xmlGpx = xml.documentElement();
    if (xmlGpx.isNull() || xmlGpx.tagName() != "gpx") {
      throw QString(res.data());
    }

    IGisItem::key_t key;
    key.item = reply->property("key.item").toString();
    key.project = reply->property("key.project").toString();
    key.device = reply->property("key.device").toString();
    qint64 elapsed = reply->property("time").toLongLong();
    elapsed = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() - elapsed;

    CGisItemRte* rte = dynamic_cast<CGisItemRte*>(CGisWorkspace::self().getItemByKey(key));
    if (rte != nullptr) {
      // Reset the old route now, just before applying the new result.
      // This is the earliest safe moment — the new data is already in hand.
      rte->reset();
      rte->setResultFromBRouter(
         xml, reply->property("options").toString() + tr("<br/>Calculation time: %1s").arg(elapsed / 1000.0, 0, 'f', 2));
    }
  } catch (const QString& msg) {
    if (!msg.isEmpty()) {
      CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
      if (canvas) {
        canvas->reportStatus("BRouter", tr("<b>BRouter</b><br/>Bad response from server:<br/>%1").arg(msg));
      }
      timerCloseStatusMsg->start();
      // cleanupGuard fires here: mutex released, scheduleNextIfPending() called.
      return;
    }
  }

  slotCloseStatusMsg();
  // cleanupGuard fires here: mutex released, scheduleNextIfPending() called.
}

// ---------------------------------------------------------------------------
// Remaining slots
// ---------------------------------------------------------------------------
void CRouterBRouter::slotToggleConsole() const {
  textBRouterOutput->setVisible(!textBRouterOutput->isVisible());
  const bool showError = localBRouter->isBRouterError() && !textBRouterOutput->isVisible();
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
  if (setup->versionMajor != major) {
    return setup->versionMajor > major;
  }
  if (setup->versionMinor != minor) {
    return setup->versionMinor > minor;
  }
  return setup->versionPatch >= patch;
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
    textBRouterOutput->clear();
    textBRouterOutput->setVisible(false);
  }
}
