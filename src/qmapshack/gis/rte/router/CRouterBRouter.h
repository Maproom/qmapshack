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

#ifndef CROUTERBROUTER_H
#define CROUTERBROUTER_H

#include <QCache>
#include <QNetworkAccessManager>
#include <QProcess>
#include <QTimer>

#include "gis/rte/router/IRouter.h"
#include "ui_IRouterBRouter.h"

#define DEBOUNCE_TIME 200
#define MAX_CACHED 50

class CRouterBRouterLocal;
class CRouterBRouterSetup;
class CRouterBRouterSetupWizard;
class CRouterBRouterInfo;
class CRouterSetup;
class CProgressDialog;

class CRouterBRouter : public IRouter, private Ui::IRouterBRouter {
  Q_OBJECT
 public:
  CRouterBRouter(QWidget* parent);
  virtual ~CRouterBRouter();

  static CRouterBRouter& self() { return *pSelf; }

  void calcRoute(const IGisItem::key_t& key) override;
  int calcRoute(const QPointF& p1, const QPointF& p2, QPolygonF& coords, qreal* costs = nullptr) override;
  bool hasFastRouting() override;
  QString getOptions() override;
  void routerSelected() override;

  void setupLocalDir(QString localDir);

 public slots:
  void slotToolSetupClicked();

 private slots:
  void slotVersionChanged();
  void slotRequestFinished(QNetworkReply* reply);
  void slotCloseStatusMsg() const;
  void slotToolProfileInfoClicked() const;
  void slotDisplayError(const QString& error, const QString& details) const;
  void slotDisplayProfileInfo(const QString& profile, const QString& content);
  void slotToggleBRouter() const;
  void slotToggleConsole() const;
  void slotClearError();

 private:
  void updateDialog() const;
  void getBRouterVersion();
  bool isMinimumVersion(int major, int minor, int patch) const;
  void updateBRouterStatus() const;
  int synchronousRequest(const QVector<QPointF>& points, const QList<IGisItem*>& nogos, QPolygonF& coords, qreal* costs);
  QNetworkRequest getRequest(const QVector<QPointF>& routePoints, const QList<IGisItem*>& nogos) const;
  QUrl getServiceUrl() const;
  QString buildCacheKey(const QVector<QPointF>& points, const QList<IGisItem*>& nogos) const;

  // Fires the actual HTTP request for the given key.
  // Called by the debounce timer — at this point no request is in-flight.
  void startAsyncRequest(const IGisItem::key_t& key);

  // Schedules a catch-up call for pendingKey (if set) after the mutex has
  // been released. Used on success, error, and cancel paths alike.
  void scheduleNextIfPending();

  CRouterBRouterLocal* localBRouter;

  QNetworkAccessManager* networkAccessManager;
  QTimer* timerCloseStatusMsg;
  bool synchronous = false;
  QMutex mutex;
  CRouterBRouterSetup* setup;
  CRouterSetup* routerSetup;
  CRouterBRouterInfo* info;
  CProgressDialog* progress{nullptr};
  bool isShutdown{false};

  // ── Async routing state ──────────────────────────────────────────────────
  //
  // The flow for rapid point-setting:
  //
  //   calcRoute(key) called N times while idle:
  //     → each call restarts timerDebounce (DEBOUNCE_TIME ms)
  //     → only the LAST key is kept in pendingKey
  //     → no mutex, no network request yet
  //
  //   timerDebounce fires:
  //     → startAsyncRequest(pendingKey) locks the mutex and sends the request
  //     → pendingKey is cleared (we are now computing it)
  //
  //   calcRoute(key) called while request is in-flight:
  //     → tryLock() fails → key is stored in pendingKey
  //     → the in-flight request is NOT aborted (it is already on the wire)
  //
  //   slotRequestFinished / cancel:
  //     → mutex released
  //     → scheduleNextIfPending() re-invokes calcRoute(pendingKey) if set
  //
  // Net effect: BRouter receives at most one request per DEBOUNCE_TIME ms idle window,
  // the last user-set position is always eventually computed, and no stale
  // intermediate results can overwrite a newer one.

  // The in-flight reply (nullptr when idle).
  QNetworkReply* pendingAsyncReply{nullptr};

  // Debounce timer — DEBOUNCE_TIME ms idle before firing startAsyncRequest().
  // During the debounce window the mutex is NOT held, so calcRoute() can
  // freely update pendingKey with the newest user position.
  QTimer* timerDebounce{nullptr};

  // The most-recently requested key that has not yet been sent to BRouter.
  // Set by calcRoute() whenever a newer call arrives; cleared by
  // startAsyncRequest() when that key is dispatched.
  // hasPendingKey is true whenever pendingKey contains valid data.
  bool hasPendingKey{false};
  IGisItem::key_t pendingKey;

  // True while the async path holds the mutex (between startAsyncRequest()
  // and the corresponding release in slotRequestFinished / cancel).
  bool asyncMutexHeld{false};

  // ── Route result cache ────────────────────────────────────────────────────
  struct CachedRoute {
    QPolygonF coords;
    qreal costs{0.0};
  };
  // MAX_CACHED entries max; negligible memory, eliminates redundant requests
  // when the user nudges a waypoint back and forth.
  QCache<QString, CachedRoute> routeCache;

  static CRouterBRouter* pSelf;
  friend class CRouterBRouterLocal;
};

#endif  // CROUTERBROUTER_H
