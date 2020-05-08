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

#include "gis/rte/router/IRouter.h"
#include "ui_IRouterBRouter.h"

#include <QNetworkAccessManager>
#include <QProcess>
#include <QTimer>

class CRouterBRouterLocal;
class CRouterBRouterSetup;
class CRouterBRouterSetupWizard;
class CRouterBRouterInfo;
class CRouterSetup;
class CProgressDialog;

class CRouterBRouter : public IRouter, private Ui::IRouterBRouter
{
    Q_OBJECT
public:
    CRouterBRouter(QWidget * parent);
    virtual ~CRouterBRouter();

    static CRouterBRouter& self()
    {
        return *pSelf;
    }

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
    void slotDisplayError(const QString &error, const QString &details) const;
    void slotDisplayProfileInfo(const QString &profile, const QString &content);
    void slotToggleBRouter() const;
    void slotToggleConsole() const;
    void slotClearError();

private:

    void updateDialog() const;
    void getBRouterVersion();
    bool isMinimumVersion(int major, int minor, int patch) const;
    void updateBRouterStatus() const;
    int synchronousRequest(const QVector<QPointF>& points, const QList<IGisItem *> &nogos, QPolygonF &coords, qreal *costs);
    QNetworkRequest getRequest(const QVector<QPointF>& routePoints, const QList<IGisItem *> &nogos) const;
    QUrl getServiceUrl() const;

    CRouterBRouterLocal * localBRouter;

    QNetworkAccessManager * networkAccessManager;
    QTimer * timerCloseStatusMsg;
    bool synchronous = false;
    QMutex mutex {QMutex::NonRecursive};
    CRouterBRouterSetup * setup;
    CRouterSetup * routerSetup;
    CRouterBRouterInfo * info;
    CProgressDialog * progress { nullptr };
    bool isShutdown { false };

    static CRouterBRouter * pSelf;
    friend class CRouterBRouterLocal;
};

#endif //CROUTERBROUTER_H

